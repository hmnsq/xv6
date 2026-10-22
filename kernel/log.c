#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "fs.h"
#ضمن "buf.h"

// Simple logging that allows concurrent FS system calls.
//
// A log transaction contains the updates of multiple FS system
// calls. The logging system only commits when there are
// no FS system calls active. Thus there is never
// any reasoning required about whether a commit might
// write an uncommitted system call's updates to disk.
//
// A system call should call begin_op()/end_op() to mark
// its start and end. Usually begin_op() just increments
// the count of in-progress FS system calls and returns.
// But if it thinks the log is close to running out, it
// sleeps until the last outstanding end_op() commits.
//
// The log is a physical re-do log containing disk blocks.
// The on-disk log format:
//   header block, containing block #s for block A, B, C, ...
//   block A
//   block B
//   block C
//   ...
// Log appends are synchronous.

// Contents of the header block, used for both the on-disk header block
// and to keep track in memory of logged block# before commit.
بنية logheader {
  صحيح n;
  صحيح بلاڪ[LOGBLOCKS];
};

بنية لاگرٿم {
  بنية spinlock تالو;
  صحيح شروعات;
  صحيح outstanding; // how many FS sys calls are executing.
  صحيح committing;  // in commit(), please wait.
  صحيح dev;
  صحيح ncommit;
  بنية logheader lh;
};
بنية لاگرٿم لاگرٿم;

ساكن فراغ recover_from_log(فراغ);
ساكن فراغ commit();

فراغ
initlog(صحيح dev, بنية superblock *sb)
{
  إذا (حجم(بنية logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&لاگرٿم.تالو, "log");
  لاگرٿم.شروعات = sb->logstart;
  لاگرٿم.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساكن فراغ
install_trans(صحيح recovering)
{
  صحيح پڇ;

  لكل (پڇ = 0; پڇ < لاگرٿم.lh.n; پڇ++) {
    إذا (recovering) {
      printk("recovering tail %d dst %d\n", پڇ, لاگرٿم.lh.بلاڪ[پڇ]);
    }
    بنية buf *lbuf = bread(لاگرٿم.dev, لاگرٿم.شروعات + پڇ + 1); // read log block
    بنية buf *dbuf = bread(لاگرٿم.dev, لاگرٿم.lh.بلاڪ[پڇ]);   // read dst
    memmove(dbuf->معلومات, lbuf->معلومات, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    إذا (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ساكن فراغ
read_سر(فراغ)
{
  بنية buf *buf = bread(لاگرٿم.dev, لاگرٿم.شروعات);
  بنية logheader *lh = (بنية logheader *)(buf->معلومات);
  صحيح i;
  لاگرٿم.lh.n = lh->n;
  لكل (i = 0; i < لاگرٿم.lh.n; i++) {
    لاگرٿم.lh.بلاڪ[i] = lh->بلاڪ[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساكن فراغ
write_سر(فراغ)
{
  بنية buf *buf = bread(لاگرٿم.dev, لاگرٿم.شروعات);
  بنية logheader *hb = (بنية logheader *)(buf->معلومات);
  صحيح i;
  hb->n = لاگرٿم.lh.n;
  لكل (i = 0; i < لاگرٿم.lh.n; i++) {
    hb->بلاڪ[i] = لاگرٿم.lh.بلاڪ[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساكن فراغ
recover_from_log(فراغ)
{
  read_سر();
  install_trans(1); // if committed, copy from log to disk
  لاگرٿم.lh.n = 0;
  write_سر(); // clear the log
}

// called at the start of each FS system call.
فراغ
begin_op(فراغ)
{
  acquire(&لاگرٿم.تالو);
  طالما (1) {
    إذا (لاگرٿم.committing) {
      sleep_prepare(&لاگرٿم);
      release(&لاگرٿم.تالو);
      sleep();
      acquire(&لاگرٿم.تالو);
    } وإلا إذا (لاگرٿم.lh.n + (لاگرٿم.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&لاگرٿم);
      release(&لاگرٿم.تالو);
      sleep();
      acquire(&لاگرٿم.تالو);
    } وإلا {
      لاگرٿم.outstanding += 1;
      release(&لاگرٿم.تالو);
      اكسر;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
فراغ
پڇاڙي_op(فراغ)
{
  صحيح do_commit = 0;

  acquire(&لاگرٿم.تالو);
  لاگرٿم.outstanding -= 1;
  إذا (لاگرٿم.committing)
    panic("log.committing");
  إذا (لاگرٿم.outstanding == 0) {
    do_commit = 1;
    لاگرٿم.committing = 1;
  } وإلا {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&لاگرٿم);
  }
  release(&لاگرٿم.تالو);

  إذا (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&لاگرٿم.تالو);
    لاگرٿم.committing = 0;
    لاگرٿم.ncommit += 1;
    wakeup(&لاگرٿم);
    release(&لاگرٿم.تالو);
  }
}

// Copy modified blocks from cache to log.
ساكن فراغ
write_log(فراغ)
{
  صحيح پڇ;

  لكل (پڇ = 0; پڇ < لاگرٿم.lh.n; پڇ++) {
    بنية buf *منزل = bread(لاگرٿم.dev, لاگرٿم.شروعات + پڇ + 1); // log block
    بنية buf *from = bread(لاگرٿم.dev, لاگرٿم.lh.بلاڪ[پڇ]); // cache block
    memmove(منزل->معلومات, from->معلومات, BSIZE);
    bwrite(منزل); // write the log
    brelse(from);
    brelse(منزل);
  }
}

ساكن فراغ
commit()
{
  إذا (لاگرٿم.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_سر();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    لاگرٿم.lh.n = 0;
    write_سر(); // Erase the transaction from the log
  }
}

// Caller has modified b->data and is done with the buffer.
// Record the block number and pin in the cache by increasing refcnt.
// commit()/write_log() will do the disk write.
//
// log_write() replaces bwrite(); a typical use is:
//   bp = bread(...)
//   modify bp->data[]
//   log_write(bp)
//   brelse(bp)
فراغ
log_write(بنية buf *b)
{
  صحيح i;

  acquire(&لاگرٿم.تالو);
  إذا (لاگرٿم.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  إذا (لاگرٿم.outstanding < 1)
    panic("log_write outside of trans");

  لكل (i = 0; i < لاگرٿم.lh.n; i++) {
    إذا (لاگرٿم.lh.بلاڪ[i] == b->blockno) // log absorption
      اكسر;
  }
  لاگرٿم.lh.بلاڪ[i] = b->blockno;
  إذا (i == لاگرٿم.lh.n) { // Add new block to log?
    bpin(b);
    لاگرٿم.lh.n++;
  }
  release(&لاگرٿم.تالو);
}

uint64
sys_sync(فراغ)
{
  acquire(&لاگرٿم.تالو);
  إذا (لاگرٿم.committing || لاگرٿم.outstanding > 0) {
    صحيح n = لاگرٿم.ncommit + 1;
    طالما (لاگرٿم.ncommit < n) {
      sleep_prepare(&لاگرٿم);
      release(&لاگرٿم.تالو);
      sleep();
      acquire(&لاگرٿم.تالو);
    }
  }
  release(&لاگرٿم.تالو);
  أرجع 0;
}
