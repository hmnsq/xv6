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
  صحيح block[LOGBLOCKS];
};

بنية log {
  بنية spinlock lock;
  صحيح start;
  صحيح outstanding; // how many FS sys calls are executing.
  صحيح committing;  // in commit(), please wait.
  صحيح dev;
  صحيح ncommit;
  بنية logheader lh;
};
بنية log log;

ساكن فراغ recover_from_log(فراغ);
ساكن فراغ commit();

فراغ
initlog(صحيح dev, بنية superblock *sb)
{
  𐤀𐤌 (حجم(بنية logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساكن فراغ
install_trans(صحيح recovering)
{
  صحيح tail;

  لكل (tail = 0; tail < log.lh.n; tail++) {
    𐤀𐤌 (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    بنية buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    بنية buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    𐤀𐤌 (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ساكن فراغ
read_head(فراغ)
{
  بنية buf *buf = bread(log.dev, log.start);
  بنية logheader *lh = (بنية logheader *)(buf->data);
  صحيح i;
  log.lh.n = lh->n;
  لكل (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساكن فراغ
write_head(فراغ)
{
  بنية buf *buf = bread(log.dev, log.start);
  بنية logheader *hb = (بنية logheader *)(buf->data);
  صحيح i;
  hb->n = log.lh.n;
  لكل (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساكن فراغ
recover_from_log(فراغ)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
فراغ
begin_op(فراغ)
{
  acquire(&log.lock);
  طالما (1) {
    𐤀𐤌 (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } وإلا 𐤀𐤌 (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } وإلا {
      log.outstanding += 1;
      release(&log.lock);
      اكسر;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
فراغ
end_op(فراغ)
{
  صحيح do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  𐤀𐤌 (log.committing)
    panic("log.committing");
  𐤀𐤌 (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } وإلا {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  𐤀𐤌 (do_commit) {
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
ساكن فراغ
write_log(فراغ)
{
  صحيح tail;

  لكل (tail = 0; tail < log.lh.n; tail++) {
    بنية buf *to = bread(log.dev, log.start + tail + 1); // log block
    بنية buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

ساكن فراغ
commit()
{
  𐤀𐤌 (log.lh.n > 0) {
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
فراغ
log_write(بنية buf *b)
{
  صحيح i;

  acquire(&log.lock);
  𐤀𐤌 (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  𐤀𐤌 (log.outstanding < 1)
    panic("log_write outside of trans");

  لكل (i = 0; i < log.lh.n; i++) {
    𐤀𐤌 (log.lh.block[i] == b->blockno) // log absorption
      اكسر;
  }
  log.lh.block[i] = b->blockno;
  𐤀𐤌 (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(فراغ)
{
  acquire(&log.lock);
  𐤀𐤌 (log.committing || log.outstanding > 0) {
    صحيح n = log.ncommit + 1;
    طالما (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  أرجع 0;
}
