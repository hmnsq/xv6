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

بنية لوگارتھم {
  بنية spinlock جندرا;
  صحيح شروع;
  صحيح outstanding; // how many FS sys calls are executing.
  صحيح committing;  // in commit(), please wait.
  صحيح dev;
  صحيح ncommit;
  بنية logheader lh;
};
بنية لوگارتھم لوگارتھم;

ساكن فراغ recover_from_log(فراغ);
ساكن فراغ commit();

فراغ
initlog(صحيح dev, بنية superblock *sb)
{
  إذا (حجم(بنية logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&لوگارتھم.جندرا, "log");
  لوگارتھم.شروع = sb->logstart;
  لوگارتھم.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساكن فراغ
install_trans(صحيح recovering)
{
  صحيح پوچھ;

  لكل (پوچھ = 0; پوچھ < لوگارتھم.lh.n; پوچھ++) {
    إذا (recovering) {
      printk("recovering tail %d dst %d\n", پوچھ, لوگارتھم.lh.بلاک[پوچھ]);
    }
    بنية buf *lbuf = bread(لوگارتھم.dev, لوگارتھم.شروع + پوچھ + 1); // read log block
    بنية buf *dbuf = bread(لوگارتھم.dev, لوگارتھم.lh.بلاک[پوچھ]);   // read dst
    memmove(dbuf->انکڑے, lbuf->انکڑے, BSIZE); // copy block to dst
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
  بنية buf *buf = bread(لوگارتھم.dev, لوگارتھم.شروع);
  بنية logheader *lh = (بنية logheader *)(buf->انکڑے);
  صحيح i;
  لوگارتھم.lh.n = lh->n;
  لكل (i = 0; i < لوگارتھم.lh.n; i++) {
    لوگارتھم.lh.بلاک[i] = lh->بلاک[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساكن فراغ
write_سر(فراغ)
{
  بنية buf *buf = bread(لوگارتھم.dev, لوگارتھم.شروع);
  بنية logheader *hb = (بنية logheader *)(buf->انکڑے);
  صحيح i;
  hb->n = لوگارتھم.lh.n;
  لكل (i = 0; i < لوگارتھم.lh.n; i++) {
    hb->بلاک[i] = لوگارتھم.lh.بلاک[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساكن فراغ
recover_from_log(فراغ)
{
  read_سر();
  install_trans(1); // if committed, copy from log to disk
  لوگارتھم.lh.n = 0;
  write_سر(); // clear the log
}

// called at the start of each FS system call.
فراغ
begin_op(فراغ)
{
  acquire(&لوگارتھم.جندرا);
  طالما (1) {
    إذا (لوگارتھم.committing) {
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.جندرا);
      sleep();
      acquire(&لوگارتھم.جندرا);
    } وإلا إذا (لوگارتھم.lh.n + (لوگارتھم.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.جندرا);
      sleep();
      acquire(&لوگارتھم.جندرا);
    } وإلا {
      لوگارتھم.outstanding += 1;
      release(&لوگارتھم.جندرا);
      اكسر;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
فراغ
سرا_op(فراغ)
{
  صحيح do_commit = 0;

  acquire(&لوگارتھم.جندرا);
  لوگارتھم.outstanding -= 1;
  إذا (لوگارتھم.committing)
    panic("log.committing");
  إذا (لوگارتھم.outstanding == 0) {
    do_commit = 1;
    لوگارتھم.committing = 1;
  } وإلا {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&لوگارتھم);
  }
  release(&لوگارتھم.جندرا);

  إذا (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&لوگارتھم.جندرا);
    لوگارتھم.committing = 0;
    لوگارتھم.ncommit += 1;
    wakeup(&لوگارتھم);
    release(&لوگارتھم.جندرا);
  }
}

// Copy modified blocks from cache to log.
ساكن فراغ
write_log(فراغ)
{
  صحيح پوچھ;

  لكل (پوچھ = 0; پوچھ < لوگارتھم.lh.n; پوچھ++) {
    بنية buf *منزل = bread(لوگارتھم.dev, لوگارتھم.شروع + پوچھ + 1); // log block
    بنية buf *from = bread(لوگارتھم.dev, لوگارتھم.lh.بلاک[پوچھ]); // cache block
    memmove(منزل->انکڑے, from->انکڑے, BSIZE);
    bwrite(منزل); // write the log
    brelse(from);
    brelse(منزل);
  }
}

ساكن فراغ
commit()
{
  إذا (لوگارتھم.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_سر();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    لوگارتھم.lh.n = 0;
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

  acquire(&لوگارتھم.جندرا);
  إذا (لوگارتھم.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  إذا (لوگارتھم.outstanding < 1)
    panic("log_write outside of trans");

  لكل (i = 0; i < لوگارتھم.lh.n; i++) {
    إذا (لوگارتھم.lh.بلاک[i] == b->blockno) // log absorption
      اكسر;
  }
  لوگارتھم.lh.بلاک[i] = b->blockno;
  إذا (i == لوگارتھم.lh.n) { // Add new block to log?
    bpin(b);
    لوگارتھم.lh.n++;
  }
  release(&لوگارتھم.جندرا);
}

uint64
sys_sync(فراغ)
{
  acquire(&لوگارتھم.جندرا);
  إذا (لوگارتھم.committing || لوگارتھم.outstanding > 0) {
    صحيح n = لوگارتھم.ncommit + 1;
    طالما (لوگارتھم.ncommit < n) {
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.جندرا);
      sleep();
      acquire(&لوگارتھم.جندرا);
    }
  }
  release(&لوگارتھم.جندرا);
  أرجع 0;
}
