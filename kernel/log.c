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
সংরচনা logheader {
  পূর্ণাঙ্ক n;
  পূর্ণাঙ্ক block[LOGBLOCKS];
};

সংরচনা log {
  সংরচনা spinlock lock;
  পূর্ণাঙ্ক start;
  পূর্ণাঙ্ক outstanding; // how many FS sys calls are executing.
  পূর্ণাঙ্ক committing;  // in commit(), please wait.
  পূর্ণাঙ্ক dev;
  পূর্ণাঙ্ক ncommit;
  সংরচনা logheader lh;
};
সংরচনা log log;

স্থির শূন্য recover_from_log(শূন্য);
স্থির শূন্য commit();

শূন্য
initlog(পূর্ণাঙ্ক dev, সংরচনা superblock *sb)
{
  যদি (পরিমাণ(সংরচনা logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
স্থির শূন্য
install_trans(পূর্ণাঙ্ক recovering)
{
  পূর্ণাঙ্ক tail;

  প্রতি (tail = 0; tail < log.lh.n; tail++) {
    যদি (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    সংরচনা buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    সংরচনা buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    যদি (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
স্থির শূন্য
read_head(শূন্য)
{
  সংরচনা buf *buf = bread(log.dev, log.start);
  সংরচনা logheader *lh = (সংরচনা logheader *)(buf->data);
  পূর্ণাঙ্ক i;
  log.lh.n = lh->n;
  প্রতি (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
স্থির শূন্য
write_head(শূন্য)
{
  সংরচনা buf *buf = bread(log.dev, log.start);
  সংরচনা logheader *hb = (সংরচনা logheader *)(buf->data);
  পূর্ণাঙ্ক i;
  hb->n = log.lh.n;
  প্রতি (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

স্থির শূন্য
recover_from_log(শূন্য)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
শূন্য
begin_op(শূন্য)
{
  acquire(&log.lock);
  যাবত্ (1) {
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
      ভঙ্গ;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
শূন্য
end_op(শূন্য)
{
  পূর্ণাঙ্ক do_commit = 0;

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
  পূর্ণাঙ্ক tail;

  প্রতি (tail = 0; tail < log.lh.n; tail++) {
    সংরচনা buf *to = bread(log.dev, log.start + tail + 1); // log block
    সংরচনা buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
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
    write_head();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    log.lh.n = 0;
    write_head(); // Erase the transaction from the log
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
log_write(সংরচনা buf *b)
{
  পূর্ণাঙ্ক i;

  acquire(&log.lock);
  যদি (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  যদি (log.outstanding < 1)
    panic("log_write outside of trans");

  প্রতি (i = 0; i < log.lh.n; i++) {
    যদি (log.lh.block[i] == b->blockno) // log absorption
      ভঙ্গ;
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
    পূর্ণাঙ্ক n = log.ncommit + 1;
    যাবত্ (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  প্রত্যাগম 0;
}
