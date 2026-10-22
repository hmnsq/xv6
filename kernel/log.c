#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "buf.h"

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
கட்டமைப்பு logheader {
  முழு n;
  முழு block[LOGBLOCKS];
};

கட்டமைப்பு log {
  கட்டமைப்பு spinlock lock;
  முழு start;
  முழு outstanding; // how many FS sys calls are executing.
  முழு committing;  // in commit(), please wait.
  முழு dev;
  முழு ncommit;
  கட்டமைப்பு logheader lh;
};
கட்டமைப்பு log log;

நிலையான வெறுமை recover_from_log(வெறுமை);
நிலையான வெறுமை commit();

வெறுமை
initlog(முழு dev, கட்டமைப்பு superblock *sb)
{
  எனில் (அளவு(கட்டமைப்பு logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
நிலையான வெறுமை
install_trans(முழு recovering)
{
  முழு tail;

  க்கு (tail = 0; tail < log.lh.n; tail++) {
    எனில் (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    கட்டமைப்பு buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    கட்டமைப்பு buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    எனில் (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
நிலையான வெறுமை
read_head(வெறுமை)
{
  கட்டமைப்பு buf *buf = bread(log.dev, log.start);
  கட்டமைப்பு logheader *lh = (கட்டமைப்பு logheader *)(buf->data);
  முழு i;
  log.lh.n = lh->n;
  க்கு (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
நிலையான வெறுமை
write_head(வெறுமை)
{
  கட்டமைப்பு buf *buf = bread(log.dev, log.start);
  கட்டமைப்பு logheader *hb = (கட்டமைப்பு logheader *)(buf->data);
  முழு i;
  hb->n = log.lh.n;
  க்கு (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

நிலையான வெறுமை
recover_from_log(வெறுமை)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
வெறுமை
begin_op(வெறுமை)
{
  acquire(&log.lock);
  போது (1) {
    எனில் (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } இல்லையெனில் எனில் (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } இல்லையெனில் {
      log.outstanding += 1;
      release(&log.lock);
      நிறுத்து;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
வெறுமை
end_op(வெறுமை)
{
  முழு do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  எனில் (log.committing)
    panic("log.committing");
  எனில் (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } இல்லையெனில் {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  எனில் (do_commit) {
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
நிலையான வெறுமை
write_log(வெறுமை)
{
  முழு tail;

  க்கு (tail = 0; tail < log.lh.n; tail++) {
    கட்டமைப்பு buf *to = bread(log.dev, log.start + tail + 1); // log block
    கட்டமைப்பு buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

நிலையான வெறுமை
commit()
{
  எனில் (log.lh.n > 0) {
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
வெறுமை
log_write(கட்டமைப்பு buf *b)
{
  முழு i;

  acquire(&log.lock);
  எனில் (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  எனில் (log.outstanding < 1)
    panic("log_write outside of trans");

  க்கு (i = 0; i < log.lh.n; i++) {
    எனில் (log.lh.block[i] == b->blockno) // log absorption
      நிறுத்து;
  }
  log.lh.block[i] = b->blockno;
  எனில் (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(வெறுமை)
{
  acquire(&log.lock);
  எனில் (log.committing || log.outstanding > 0) {
    முழு n = log.ncommit + 1;
    போது (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  திரும்பு 0;
}
