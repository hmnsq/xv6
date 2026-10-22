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
  صحيح بلوک[LOGBLOCKS];
};

بنية لگاریتم {
  بنية spinlock قفل;
  صحيح شروع;
  صحيح outstanding; // how many FS sys calls are executing.
  صحيح committing;  // in commit(), please wait.
  صحيح dev;
  صحيح ncommit;
  بنية logheader lh;
};
بنية لگاریتم لگاریتم;

ساكن فراغ recover_from_log(فراغ);
ساكن فراغ commit();

فراغ
initlog(صحيح dev, بنية superblock *sb)
{
  إذا (حجم(بنية logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&لگاریتم.قفل, "log");
  لگاریتم.شروع = sb->logstart;
  لگاریتم.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساكن فراغ
install_trans(صحيح recovering)
{
  صحيح دم;

  لكل (دم = 0; دم < لگاریتم.lh.n; دم++) {
    إذا (recovering) {
      printk("recovering tail %d dst %d\n", دم, لگاریتم.lh.بلوک[دم]);
    }
    بنية buf *lbuf = bread(لگاریتم.dev, لگاریتم.شروع + دم + 1); // read log block
    بنية buf *dbuf = bread(لگاریتم.dev, لگاریتم.lh.بلوک[دم]);   // read dst
    memmove(dbuf->داده, lbuf->داده, BSIZE); // copy block to dst
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
  بنية buf *buf = bread(لگاریتم.dev, لگاریتم.شروع);
  بنية logheader *lh = (بنية logheader *)(buf->داده);
  صحيح i;
  لگاریتم.lh.n = lh->n;
  لكل (i = 0; i < لگاریتم.lh.n; i++) {
    لگاریتم.lh.بلوک[i] = lh->بلوک[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساكن فراغ
write_سر(فراغ)
{
  بنية buf *buf = bread(لگاریتم.dev, لگاریتم.شروع);
  بنية logheader *hb = (بنية logheader *)(buf->داده);
  صحيح i;
  hb->n = لگاریتم.lh.n;
  لكل (i = 0; i < لگاریتم.lh.n; i++) {
    hb->بلوک[i] = لگاریتم.lh.بلوک[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساكن فراغ
recover_from_log(فراغ)
{
  read_سر();
  install_trans(1); // if committed, copy from log to disk
  لگاریتم.lh.n = 0;
  write_سر(); // clear the log
}

// called at the start of each FS system call.
فراغ
begin_op(فراغ)
{
  acquire(&لگاریتم.قفل);
  طالما (1) {
    إذا (لگاریتم.committing) {
      sleep_prepare(&لگاریتم);
      release(&لگاریتم.قفل);
      sleep();
      acquire(&لگاریتم.قفل);
    } وإلا إذا (لگاریتم.lh.n + (لگاریتم.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&لگاریتم);
      release(&لگاریتم.قفل);
      sleep();
      acquire(&لگاریتم.قفل);
    } وإلا {
      لگاریتم.outstanding += 1;
      release(&لگاریتم.قفل);
      اكسر;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
فراغ
انتها_op(فراغ)
{
  صحيح do_commit = 0;

  acquire(&لگاریتم.قفل);
  لگاریتم.outstanding -= 1;
  إذا (لگاریتم.committing)
    panic("log.committing");
  إذا (لگاریتم.outstanding == 0) {
    do_commit = 1;
    لگاریتم.committing = 1;
  } وإلا {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&لگاریتم);
  }
  release(&لگاریتم.قفل);

  إذا (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&لگاریتم.قفل);
    لگاریتم.committing = 0;
    لگاریتم.ncommit += 1;
    wakeup(&لگاریتم);
    release(&لگاریتم.قفل);
  }
}

// Copy modified blocks from cache to log.
ساكن فراغ
write_log(فراغ)
{
  صحيح دم;

  لكل (دم = 0; دم < لگاریتم.lh.n; دم++) {
    بنية buf *مقصد = bread(لگاریتم.dev, لگاریتم.شروع + دم + 1); // log block
    بنية buf *from = bread(لگاریتم.dev, لگاریتم.lh.بلوک[دم]); // cache block
    memmove(مقصد->داده, from->داده, BSIZE);
    bwrite(مقصد); // write the log
    brelse(from);
    brelse(مقصد);
  }
}

ساكن فراغ
commit()
{
  إذا (لگاریتم.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_سر();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    لگاریتم.lh.n = 0;
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

  acquire(&لگاریتم.قفل);
  إذا (لگاریتم.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  إذا (لگاریتم.outstanding < 1)
    panic("log_write outside of trans");

  لكل (i = 0; i < لگاریتم.lh.n; i++) {
    إذا (لگاریتم.lh.بلوک[i] == b->blockno) // log absorption
      اكسر;
  }
  لگاریتم.lh.بلوک[i] = b->blockno;
  إذا (i == لگاریتم.lh.n) { // Add new block to log?
    bpin(b);
    لگاریتم.lh.n++;
  }
  release(&لگاریتم.قفل);
}

uint64
sys_sync(فراغ)
{
  acquire(&لگاریتم.قفل);
  إذا (لگاریتم.committing || لگاریتم.outstanding > 0) {
    صحيح n = لگاریتم.ncommit + 1;
    طالما (لگاریتم.ncommit < n) {
      sleep_prepare(&لگاریتم);
      release(&لگاریتم.قفل);
      sleep();
      acquire(&لگاریتم.قفل);
    }
  }
  release(&لگاریتم.قفل);
  أرجع 0;
}
