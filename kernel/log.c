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
  صحيح بلاک[LOGBLOCKS];
};

بنية لوګاریتم {
  بنية spinlock قلف;
  صحيح پیل;
  صحيح outstanding; // how many FS sys calls are executing.
  صحيح committing;  // in commit(), please wait.
  صحيح dev;
  صحيح ncommit;
  بنية logheader lh;
};
بنية لوګاریتم لوګاریتم;

ساكن فراغ recover_from_log(فراغ);
ساكن فراغ commit();

فراغ
initlog(صحيح dev, بنية superblock *sb)
{
  إذا (حجم(بنية logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&لوګاریتم.قلف, "log");
  لوګاریتم.پیل = sb->logstart;
  لوګاریتم.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساكن فراغ
install_trans(صحيح recovering)
{
  صحيح لکۍ;

  لكل (لکۍ = 0; لکۍ < لوګاریتم.lh.n; لکۍ++) {
    إذا (recovering) {
      printk("recovering tail %d dst %d\n", لکۍ, لوګاریتم.lh.بلاک[لکۍ]);
    }
    بنية buf *lbuf = bread(لوګاریتم.dev, لوګاریتم.پیل + لکۍ + 1); // read log block
    بنية buf *dbuf = bread(لوګاریتم.dev, لوګاریتم.lh.بلاک[لکۍ]);   // read dst
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
  بنية buf *buf = bread(لوګاریتم.dev, لوګاریتم.پیل);
  بنية logheader *lh = (بنية logheader *)(buf->معلومات);
  صحيح i;
  لوګاریتم.lh.n = lh->n;
  لكل (i = 0; i < لوګاریتم.lh.n; i++) {
    لوګاریتم.lh.بلاک[i] = lh->بلاک[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساكن فراغ
write_سر(فراغ)
{
  بنية buf *buf = bread(لوګاریتم.dev, لوګاریتم.پیل);
  بنية logheader *hb = (بنية logheader *)(buf->معلومات);
  صحيح i;
  hb->n = لوګاریتم.lh.n;
  لكل (i = 0; i < لوګاریتم.lh.n; i++) {
    hb->بلاک[i] = لوګاریتم.lh.بلاک[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساكن فراغ
recover_from_log(فراغ)
{
  read_سر();
  install_trans(1); // if committed, copy from log to disk
  لوګاریتم.lh.n = 0;
  write_سر(); // clear the log
}

// called at the start of each FS system call.
فراغ
begin_op(فراغ)
{
  acquire(&لوګاریتم.قلف);
  طالما (1) {
    إذا (لوګاریتم.committing) {
      sleep_prepare(&لوګاریتم);
      release(&لوګاریتم.قلف);
      sleep();
      acquire(&لوګاریتم.قلف);
    } وإلا إذا (لوګاریتم.lh.n + (لوګاریتم.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&لوګاریتم);
      release(&لوګاریتم.قلف);
      sleep();
      acquire(&لوګاریتم.قلف);
    } وإلا {
      لوګاریتم.outstanding += 1;
      release(&لوګاریتم.قلف);
      اكسر;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
فراغ
پای_ټکی_op(فراغ)
{
  صحيح do_commit = 0;

  acquire(&لوګاریتم.قلف);
  لوګاریتم.outstanding -= 1;
  إذا (لوګاریتم.committing)
    panic("log.committing");
  إذا (لوګاریتم.outstanding == 0) {
    do_commit = 1;
    لوګاریتم.committing = 1;
  } وإلا {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&لوګاریتم);
  }
  release(&لوګاریتم.قلف);

  إذا (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&لوګاریتم.قلف);
    لوګاریتم.committing = 0;
    لوګاریتم.ncommit += 1;
    wakeup(&لوګاریتم);
    release(&لوګاریتم.قلف);
  }
}

// Copy modified blocks from cache to log.
ساكن فراغ
write_log(فراغ)
{
  صحيح لکۍ;

  لكل (لکۍ = 0; لکۍ < لوګاریتم.lh.n; لکۍ++) {
    بنية buf *منزل = bread(لوګاریتم.dev, لوګاریتم.پیل + لکۍ + 1); // log block
    بنية buf *from = bread(لوګاریتم.dev, لوګاریتم.lh.بلاک[لکۍ]); // cache block
    memmove(منزل->معلومات, from->معلومات, BSIZE);
    bwrite(منزل); // write the log
    brelse(from);
    brelse(منزل);
  }
}

ساكن فراغ
commit()
{
  إذا (لوګاریتم.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_سر();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    لوګاریتم.lh.n = 0;
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

  acquire(&لوګاریتم.قلف);
  إذا (لوګاریتم.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  إذا (لوګاریتم.outstanding < 1)
    panic("log_write outside of trans");

  لكل (i = 0; i < لوګاریتم.lh.n; i++) {
    إذا (لوګاریتم.lh.بلاک[i] == b->blockno) // log absorption
      اكسر;
  }
  لوګاریتم.lh.بلاک[i] = b->blockno;
  إذا (i == لوګاریتم.lh.n) { // Add new block to log?
    bpin(b);
    لوګاریتم.lh.n++;
  }
  release(&لوګاریتم.قلف);
}

uint64
sys_sync(فراغ)
{
  acquire(&لوګاریتم.قلف);
  إذا (لوګاریتم.committing || لوګاریتم.outstanding > 0) {
    صحيح n = لوګاریتم.ncommit + 1;
    طالما (لوګاریتم.ncommit < n) {
      sleep_prepare(&لوګاریتم);
      release(&لوګاریتم.قلف);
      sleep();
      acquire(&لوګاریتم.قلف);
    }
  }
  release(&لوګاریتم.قلف);
  أرجع 0;
}
