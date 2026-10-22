#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "buf.h"

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
ସଂରଚନା logheader {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା block[LOGBLOCKS];
};

ସଂରଚନା log {
  ସଂରଚନା spinlock lock;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା start;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା outstanding; // how many FS sys calls are executing.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା committing;  // in commit(), please wait.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା dev;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncommit;
  ସଂରଚନା logheader lh;
};
ସଂରଚନା log log;

ସ୍ଥିର ଶୂନ୍ୟ recover_from_log(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ commit();

ଶୂନ୍ୟ
initlog(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dev, ସଂରଚନା superblock *sb)
{
  ଯଦି (ଆକାର(ସଂରଚନା logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ସ୍ଥିର ଶୂନ୍ୟ
install_trans(ପୂର୍ଣ୍ଣସଂଖ୍ୟା recovering)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tail;

  ପାଇଁ (tail = 0; tail < log.lh.n; tail++) {
    ଯଦି (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    ସଂରଚନା buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    ସଂରଚନା buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ଯଦି (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ସ୍ଥିର ଶୂନ୍ୟ
read_head(ଶୂନ୍ୟ)
{
  ସଂରଚନା buf *buf = bread(log.dev, log.start);
  ସଂରଚନା logheader *lh = (ସଂରଚନା logheader *)(buf->data);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  log.lh.n = lh->n;
  ପାଇଁ (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ସ୍ଥିର ଶୂନ୍ୟ
write_head(ଶୂନ୍ୟ)
{
  ସଂରଚନା buf *buf = bread(log.dev, log.start);
  ସଂରଚନା logheader *hb = (ସଂରଚନା logheader *)(buf->data);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  hb->n = log.lh.n;
  ପାଇଁ (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

ସ୍ଥିର ଶୂନ୍ୟ
recover_from_log(ଶୂନ୍ୟ)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
ଶୂନ୍ୟ
begin_op(ଶୂନ୍ୟ)
{
  acquire(&log.lock);
  ଯେତେବେଳେ (1) {
    ଯଦି (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ଅନ୍ୟଥା ଯଦି (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ଅନ୍ୟଥା {
      log.outstanding += 1;
      release(&log.lock);
      ଭାଙ୍ଗ;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
ଶୂନ୍ୟ
end_op(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  ଯଦି (log.committing)
    panic("log.committing");
  ଯଦି (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } ଅନ୍ୟଥା {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  ଯଦି (do_commit) {
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
ସ୍ଥିର ଶୂନ୍ୟ
write_log(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tail;

  ପାଇଁ (tail = 0; tail < log.lh.n; tail++) {
    ସଂରଚନା buf *to = bread(log.dev, log.start + tail + 1); // log block
    ସଂରଚନା buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ
commit()
{
  ଯଦି (log.lh.n > 0) {
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
ଶୂନ୍ୟ
log_write(ସଂରଚନା buf *b)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  acquire(&log.lock);
  ଯଦି (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ଯଦି (log.outstanding < 1)
    panic("log_write outside of trans");

  ପାଇଁ (i = 0; i < log.lh.n; i++) {
    ଯଦି (log.lh.block[i] == b->blockno) // log absorption
      ଭାଙ୍ଗ;
  }
  log.lh.block[i] = b->blockno;
  ଯଦି (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(ଶୂନ୍ୟ)
{
  acquire(&log.lock);
  ଯଦି (log.committing || log.outstanding > 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = log.ncommit + 1;
    ଯେତେବେଳେ (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  ଫେରନ୍ତୁ 0;
}
