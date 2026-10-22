#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "buf.h"

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
ساختار logheader {
  صحیح n;
  صحیح block[LOGBLOCKS];
};

ساختار log {
  ساختار spinlock lock;
  صحیح start;
  صحیح outstanding; // how many FS sys calls are executing.
  صحیح committing;  // in commit(), please wait.
  صحیح dev;
  صحیح ncommit;
  ساختار logheader lh;
};
ساختار log log;

ایستا تهی recover_from_log(تهی);
ایستا تهی commit();

تهی
initlog(صحیح dev, ساختار superblock *sb)
{
  اگر (اندازه(ساختار logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ایستا تهی
install_trans(صحیح recovering)
{
  صحیح tail;

  برای (tail = 0; tail < log.lh.n; tail++) {
    اگر (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    ساختار buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    ساختار buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    اگر (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ایستا تهی
read_head(تهی)
{
  ساختار buf *buf = bread(log.dev, log.start);
  ساختار logheader *lh = (ساختار logheader *)(buf->data);
  صحیح i;
  log.lh.n = lh->n;
  برای (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ایستا تهی
write_head(تهی)
{
  ساختار buf *buf = bread(log.dev, log.start);
  ساختار logheader *hb = (ساختار logheader *)(buf->data);
  صحیح i;
  hb->n = log.lh.n;
  برای (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

ایستا تهی
recover_from_log(تهی)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
تهی
begin_op(تهی)
{
  acquire(&log.lock);
  وقتی (1) {
    اگر (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } وگرنه اگر (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } وگرنه {
      log.outstanding += 1;
      release(&log.lock);
      بشکن;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
تهی
end_op(تهی)
{
  صحیح do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  اگر (log.committing)
    panic("log.committing");
  اگر (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } وگرنه {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  اگر (do_commit) {
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
ایستا تهی
write_log(تهی)
{
  صحیح tail;

  برای (tail = 0; tail < log.lh.n; tail++) {
    ساختار buf *to = bread(log.dev, log.start + tail + 1); // log block
    ساختار buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

ایستا تهی
commit()
{
  اگر (log.lh.n > 0) {
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
تهی
log_write(ساختار buf *b)
{
  صحیح i;

  acquire(&log.lock);
  اگر (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  اگر (log.outstanding < 1)
    panic("log_write outside of trans");

  برای (i = 0; i < log.lh.n; i++) {
    اگر (log.lh.block[i] == b->blockno) // log absorption
      بشکن;
  }
  log.lh.block[i] = b->blockno;
  اگر (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(تهی)
{
  acquire(&log.lock);
  اگر (log.committing || log.outstanding > 0) {
    صحیح n = log.ncommit + 1;
    وقتی (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  بازگشت 0;
}
