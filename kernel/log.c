#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "buf.h"

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
ಸಂರಚನಾ logheader {
  ಪೂರ್ಣಾಙ್ಕ n;
  ಪೂರ್ಣಾಙ್ಕ block[LOGBLOCKS];
};

ಸಂರಚನಾ log {
  ಸಂರಚನಾ spinlock lock;
  ಪೂರ್ಣಾಙ್ಕ start;
  ಪೂರ್ಣಾಙ್ಕ outstanding; // how many FS sys calls are executing.
  ಪೂರ್ಣಾಙ್ಕ committing;  // in commit(), please wait.
  ಪೂರ್ಣಾಙ್ಕ dev;
  ಪೂರ್ಣಾಙ್ಕ ncommit;
  ಸಂರಚನಾ logheader lh;
};
ಸಂರಚನಾ log log;

ಸ್ಥಿರ ಶೂನ್ಯ recover_from_log(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ commit();

ಶೂನ್ಯ
initlog(ಪೂರ್ಣಾಙ್ಕ dev, ಸಂರಚನಾ superblock *sb)
{
  ಯದಿ (ಪರಿಮಾಣ(ಸಂರಚನಾ logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ಸ್ಥಿರ ಶೂನ್ಯ
install_trans(ಪೂರ್ಣಾಙ್ಕ recovering)
{
  ಪೂರ್ಣಾಙ್ಕ tail;

  ಪ್ರತಿ (tail = 0; tail < log.lh.n; tail++) {
    ಯದಿ (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    ಸಂರಚನಾ buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    ಸಂರಚನಾ buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ಯದಿ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ಸ್ಥಿರ ಶೂನ್ಯ
read_head(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ buf *buf = bread(log.dev, log.start);
  ಸಂರಚನಾ logheader *lh = (ಸಂರಚನಾ logheader *)(buf->data);
  ಪೂರ್ಣಾಙ್ಕ i;
  log.lh.n = lh->n;
  ಪ್ರತಿ (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ಸ್ಥಿರ ಶೂನ್ಯ
write_head(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ buf *buf = bread(log.dev, log.start);
  ಸಂರಚನಾ logheader *hb = (ಸಂರಚನಾ logheader *)(buf->data);
  ಪೂರ್ಣಾಙ್ಕ i;
  hb->n = log.lh.n;
  ಪ್ರತಿ (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

ಸ್ಥಿರ ಶೂನ್ಯ
recover_from_log(ಶೂನ್ಯ)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
ಶೂನ್ಯ
begin_op(ಶೂನ್ಯ)
{
  acquire(&log.lock);
  ಯಾವತ್ (1) {
    ಯದಿ (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ಅನ್ಯಥಾ ಯದಿ (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ಅನ್ಯಥಾ {
      log.outstanding += 1;
      release(&log.lock);
      ಭಙ್ಗ;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
ಶೂನ್ಯ
end_op(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  ಯದಿ (log.committing)
    panic("log.committing");
  ಯದಿ (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } ಅನ್ಯಥಾ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  ಯದಿ (do_commit) {
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
ಸ್ಥಿರ ಶೂನ್ಯ
write_log(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ tail;

  ಪ್ರತಿ (tail = 0; tail < log.lh.n; tail++) {
    ಸಂರಚನಾ buf *to = bread(log.dev, log.start + tail + 1); // log block
    ಸಂರಚನಾ buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ
commit()
{
  ಯದಿ (log.lh.n > 0) {
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
ಶೂನ್ಯ
log_write(ಸಂರಚನಾ buf *b)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  acquire(&log.lock);
  ಯದಿ (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ಯದಿ (log.outstanding < 1)
    panic("log_write outside of trans");

  ಪ್ರತಿ (i = 0; i < log.lh.n; i++) {
    ಯದಿ (log.lh.block[i] == b->blockno) // log absorption
      ಭಙ್ಗ;
  }
  log.lh.block[i] = b->blockno;
  ಯದಿ (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(ಶೂನ್ಯ)
{
  acquire(&log.lock);
  ಯದಿ (log.committing || log.outstanding > 0) {
    ಪೂರ್ಣಾಙ್ಕ n = log.ncommit + 1;
    ಯಾವತ್ (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  ಪ್ರತ್ಯಾಗಮ 0;
}
