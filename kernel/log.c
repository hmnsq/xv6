#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "buf.h"

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
સંરચના logheader {
  પૂર્ણાંક n;
  પૂર્ણાંક block[LOGBLOCKS];
};

સંરચના log {
  સંરચના spinlock lock;
  પૂર્ણાંક start;
  પૂર્ણાંક outstanding; // how many FS sys calls are executing.
  પૂર્ણાંક committing;  // in commit(), please wait.
  પૂર્ણાંક dev;
  પૂર્ણાંક ncommit;
  સંરચના logheader lh;
};
સંરચના log log;

સ્થિર શૂન્ય recover_from_log(શૂન્ય);
સ્થિર શૂન્ય commit();

શૂન્ય
initlog(પૂર્ણાંક dev, સંરચના superblock *sb)
{
  જો (કદ(સંરચના logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
સ્થિર શૂન્ય
install_trans(પૂર્ણાંક recovering)
{
  પૂર્ણાંક tail;

  માટે (tail = 0; tail < log.lh.n; tail++) {
    જો (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    સંરચના buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    સંરચના buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    જો (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
સ્થિર શૂન્ય
read_head(શૂન્ય)
{
  સંરચના buf *buf = bread(log.dev, log.start);
  સંરચના logheader *lh = (સંરચના logheader *)(buf->data);
  પૂર્ણાંક i;
  log.lh.n = lh->n;
  માટે (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
સ્થિર શૂન્ય
write_head(શૂન્ય)
{
  સંરચના buf *buf = bread(log.dev, log.start);
  સંરચના logheader *hb = (સંરચના logheader *)(buf->data);
  પૂર્ણાંક i;
  hb->n = log.lh.n;
  માટે (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

સ્થિર શૂન્ય
recover_from_log(શૂન્ય)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
શૂન્ય
begin_op(શૂન્ય)
{
  acquire(&log.lock);
  જ્યારે (1) {
    જો (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } નહીંતર જો (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } નહીંતર {
      log.outstanding += 1;
      release(&log.lock);
      તોડો;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
શૂન્ય
end_op(શૂન્ય)
{
  પૂર્ણાંક do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  જો (log.committing)
    panic("log.committing");
  જો (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } નહીંતર {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  જો (do_commit) {
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
સ્થિર શૂન્ય
write_log(શૂન્ય)
{
  પૂર્ણાંક tail;

  માટે (tail = 0; tail < log.lh.n; tail++) {
    સંરચના buf *to = bread(log.dev, log.start + tail + 1); // log block
    સંરચના buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

સ્થિર શૂન્ય
commit()
{
  જો (log.lh.n > 0) {
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
શૂન્ય
log_write(સંરચના buf *b)
{
  પૂર્ણાંક i;

  acquire(&log.lock);
  જો (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  જો (log.outstanding < 1)
    panic("log_write outside of trans");

  માટે (i = 0; i < log.lh.n; i++) {
    જો (log.lh.block[i] == b->blockno) // log absorption
      તોડો;
  }
  log.lh.block[i] = b->blockno;
  જો (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(શૂન્ય)
{
  acquire(&log.lock);
  જો (log.committing || log.outstanding > 0) {
    પૂર્ણાંક n = log.ncommit + 1;
    જ્યારે (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  પાછા 0;
}
