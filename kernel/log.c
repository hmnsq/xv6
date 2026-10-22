#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

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
sa_nracanaa logheader {
  puur_naa_nka n;
  puur_naa_nka block[LOGBLOCKS];
};

sa_nracanaa log {
  sa_nracanaa spinlock lock;
  puur_naa_nka start;
  puur_naa_nka outstanding; // how many FS sys calls are executing.
  puur_naa_nka committing;  // in commit(), please wait.
  puur_naa_nka dev;
  puur_naa_nka ncommit;
  sa_nracanaa logheader lh;
};
sa_nracanaa log log;

static void recover_from_log(void);
static void commit();

void
initlog(puur_naa_nka dev, sa_nracanaa superblock *sb)
{
  ᬬᬾᬦ᭄ (parimaa_na(sa_nracanaa logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
static void
install_trans(puur_naa_nka recovering)
{
  puur_naa_nka tail;

  for (tail = 0; tail < log.lh.n; tail++) {
    ᬬᬾᬦ᭄ (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    sa_nracanaa buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    sa_nracanaa buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    memmove(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ᬬᬾᬦ᭄ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
static void
read_head(void)
{
  sa_nracanaa buf *buf = bread(log.dev, log.start);
  sa_nracanaa logheader *lh = (sa_nracanaa logheader *)(buf->data);
  puur_naa_nka i;
  log.lh.n = lh->n;
  for (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
static void
write_head(void)
{
  sa_nracanaa buf *buf = bread(log.dev, log.start);
  sa_nracanaa logheader *hb = (sa_nracanaa logheader *)(buf->data);
  puur_naa_nka i;
  hb->n = log.lh.n;
  for (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

static void
recover_from_log(void)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
void
begin_op(void)
{
  acquire(&log.lock);
  ᬲᭂᬤᭂᬮ (1) {
    ᬬᬾᬦ᭄ (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } ᬮᬶᬬ {
      log.outstanding += 1;
      release(&log.lock);
      bha_nga;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
void
end_op(void)
{
  puur_naa_nka do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  ᬬᬾᬦ᭄ (log.committing)
    panic("log.committing");
  ᬬᬾᬦ᭄ (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } ᬮᬶᬬ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  ᬬᬾᬦ᭄ (do_commit) {
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
static void
write_log(void)
{
  puur_naa_nka tail;

  for (tail = 0; tail < log.lh.n; tail++) {
    sa_nracanaa buf *to = bread(log.dev, log.start + tail + 1); // log block
    sa_nracanaa buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    memmove(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

static void
commit()
{
  ᬬᬾᬦ᭄ (log.lh.n > 0) {
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
void
log_write(sa_nracanaa buf *b)
{
  puur_naa_nka i;

  acquire(&log.lock);
  ᬬᬾᬦ᭄ (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ᬬᬾᬦ᭄ (log.outstanding < 1)
    panic("log_write outside of trans");

  for (i = 0; i < log.lh.n; i++) {
    ᬬᬾᬦ᭄ (log.lh.block[i] == b->blockno) // log absorption
      bha_nga;
  }
  log.lh.block[i] = b->blockno;
  ᬬᬾᬦ᭄ (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(void)
{
  acquire(&log.lock);
  ᬬᬾᬦ᭄ (log.committing || log.outstanding > 0) {
    puur_naa_nka n = log.ncommit + 1;
    ᬲᭂᬤᭂᬮ (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  ᬯᬮᬶ 0;
}
