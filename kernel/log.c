#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "buf.h"

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
কাঠামো logheader {
  পূর্ণসংখ্যা n;
  পূর্ণসংখ্যা block[LOGBLOCKS];
};

কাঠামো log {
  কাঠামো spinlock lock;
  পূর্ণসংখ্যা start;
  পূর্ণসংখ্যা outstanding; // how many FS sys calls are executing.
  পূর্ণসংখ্যা committing;  // in commit(), please wait.
  পূর্ণসংখ্যা dev;
  পূর্ণসংখ্যা ncommit;
  কাঠামো logheader lh;
};
কাঠামো log log;

স্থির শূন্য recover_from_log(শূন্য);
স্থির শূন্য commit();

শূন্য
initlog(পূর্ণসংখ্যা dev, কাঠামো superblock *sb)
{
  যদি (আকার(কাঠামো logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
স্থির শূন্য
install_trans(পূর্ণসংখ্যা recovering)
{
  পূর্ণসংখ্যা লেজ;

  জন্য (লেজ = 0; লেজ < log.lh.n; লেজ++) {
    যদি (recovering) {
      printk("recovering tail %d dst %d\n", লেজ, log.lh.block[লেজ]);
    }
    কাঠামো buf *lbuf = bread(log.dev, log.start + লেজ + 1); // read log block
    কাঠামো buf *dbuf = bread(log.dev, log.lh.block[লেজ]);   // read dst
    memmove(dbuf->তথ্য, lbuf->তথ্য, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    যদি (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
স্থির শূন্য
read_শীর্ষ(শূন্য)
{
  কাঠামো buf *buf = bread(log.dev, log.start);
  কাঠামো logheader *lh = (কাঠামো logheader *)(buf->তথ্য);
  পূর্ণসংখ্যা i;
  log.lh.n = lh->n;
  জন্য (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
স্থির শূন্য
write_শীর্ষ(শূন্য)
{
  কাঠামো buf *buf = bread(log.dev, log.start);
  কাঠামো logheader *hb = (কাঠামো logheader *)(buf->তথ্য);
  পূর্ণসংখ্যা i;
  hb->n = log.lh.n;
  জন্য (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

স্থির শূন্য
recover_from_log(শূন্য)
{
  read_শীর্ষ();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_শীর্ষ(); // clear the log
}

// called at the start of each FS system call.
শূন্য
begin_op(শূন্য)
{
  acquire(&log.lock);
  যখন (1) {
    যদি (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } অন্যথা যদি (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } অন্যথা {
      log.outstanding += 1;
      release(&log.lock);
      বিরতি;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
শূন্য
end_op(শূন্য)
{
  পূর্ণসংখ্যা do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  যদি (log.committing)
    panic("log.committing");
  যদি (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } অন্যথা {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  যদি (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&log.lock);
    log.committing = 0;
    log.ncommit += 1;
    wakeup(&log);
    release(&log.lock);
  }
}

// Copy modified blocks from cache to log.
স্থির শূন্য
write_log(শূন্য)
{
  পূর্ণসংখ্যা লেজ;

  জন্য (লেজ = 0; লেজ < log.lh.n; লেজ++) {
    কাঠামো buf *to = bread(log.dev, log.start + লেজ + 1); // log block
    কাঠামো buf *from = bread(log.dev, log.lh.block[লেজ]); // cache block
    memmove(to->তথ্য, from->তথ্য, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

স্থির শূন্য
commit()
{
  যদি (log.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_শীর্ষ();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    log.lh.n = 0;
    write_শীর্ষ(); // Erase the transaction from the log
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
শূন্য
log_write(কাঠামো buf *b)
{
  পূর্ণসংখ্যা i;

  acquire(&log.lock);
  যদি (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  যদি (log.outstanding < 1)
    panic("log_write outside of trans");

  জন্য (i = 0; i < log.lh.n; i++) {
    যদি (log.lh.block[i] == b->blockno) // log absorption
      বিরতি;
  }
  log.lh.block[i] = b->blockno;
  যদি (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(শূন্য)
{
  acquire(&log.lock);
  যদি (log.committing || log.outstanding > 0) {
    পূর্ণসংখ্যা n = log.ncommit + 1;
    যখন (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  ফেরত 0;
}
