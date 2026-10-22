#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "buf.h"

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
ഘടന logheader {
  പൂർണ്ണസംഖ്യ n;
  പൂർണ്ണസംഖ്യ block[LOGBLOCKS];
};

ഘടന log {
  ഘടന spinlock lock;
  പൂർണ്ണസംഖ്യ start;
  പൂർണ്ണസംഖ്യ outstanding; // how many FS sys calls are executing.
  പൂർണ്ണസംഖ്യ committing;  // in commit(), please wait.
  പൂർണ്ണസംഖ്യ dev;
  പൂർണ്ണസംഖ്യ ncommit;
  ഘടന logheader lh;
};
ഘടന log log;

സ്ഥിതം ശൂന്യം recover_from_log(ശൂന്യം);
സ്ഥിതം ശൂന്യം commit();

ശൂന്യം
initlog(പൂർണ്ണസംഖ്യ dev, ഘടന superblock *sb)
{
  എങ്കിൽ (വലുപ്പം(ഘടന logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
സ്ഥിതം ശൂന്യം
install_trans(പൂർണ്ണസംഖ്യ recovering)
{
  പൂർണ്ണസംഖ്യ tail;

  വേണ്ടി (tail = 0; tail < log.lh.n; tail++) {
    എങ്കിൽ (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    ഘടന buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    ഘടന buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    എങ്കിൽ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
സ്ഥിതം ശൂന്യം
read_head(ശൂന്യം)
{
  ഘടന buf *buf = bread(log.dev, log.start);
  ഘടന logheader *lh = (ഘടന logheader *)(buf->data);
  പൂർണ്ണസംഖ്യ i;
  log.lh.n = lh->n;
  വേണ്ടി (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
സ്ഥിതം ശൂന്യം
write_head(ശൂന്യം)
{
  ഘടന buf *buf = bread(log.dev, log.start);
  ഘടന logheader *hb = (ഘടന logheader *)(buf->data);
  പൂർണ്ണസംഖ്യ i;
  hb->n = log.lh.n;
  വേണ്ടി (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

സ്ഥിതം ശൂന്യം
recover_from_log(ശൂന്യം)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
ശൂന്യം
begin_op(ശൂന്യം)
{
  acquire(&log.lock);
  ആകുമ്പോൾ (1) {
    എങ്കിൽ (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } അല്ലെങ്കിൽ എങ്കിൽ (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } അല്ലെങ്കിൽ {
      log.outstanding += 1;
      release(&log.lock);
      നിർത്തുക;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
ശൂന്യം
end_op(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  എങ്കിൽ (log.committing)
    panic("log.committing");
  എങ്കിൽ (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } അല്ലെങ്കിൽ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  എങ്കിൽ (do_commit) {
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
സ്ഥിതം ശൂന്യം
write_log(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ tail;

  വേണ്ടി (tail = 0; tail < log.lh.n; tail++) {
    ഘടന buf *to = bread(log.dev, log.start + tail + 1); // log block
    ഘടന buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

സ്ഥിതം ശൂന്യം
commit()
{
  എങ്കിൽ (log.lh.n > 0) {
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
ശൂന്യം
log_write(ഘടന buf *b)
{
  പൂർണ്ണസംഖ്യ i;

  acquire(&log.lock);
  എങ്കിൽ (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  എങ്കിൽ (log.outstanding < 1)
    panic("log_write outside of trans");

  വേണ്ടി (i = 0; i < log.lh.n; i++) {
    എങ്കിൽ (log.lh.block[i] == b->blockno) // log absorption
      നിർത്തുക;
  }
  log.lh.block[i] = b->blockno;
  എങ്കിൽ (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(ശൂന്യം)
{
  acquire(&log.lock);
  എങ്കിൽ (log.committing || log.outstanding > 0) {
    പൂർണ്ണസംഖ്യ n = log.ncommit + 1;
    ആകുമ്പോൾ (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  തിരികെ 0;
}
