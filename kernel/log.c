#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "fs.h"
#समावेश "buf.h"

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
काष्ठा logheader {
  पूर्णांक n;
  पूर्णांक block[LOGBLOCKS];
};

काष्ठा log {
  काष्ठा spinlock lock;
  पूर्णांक start;
  पूर्णांक outstanding; // how many FS sys calls are executing.
  पूर्णांक committing;  // in commit(), please wait.
  पूर्णांक dev;
  पूर्णांक ncommit;
  काष्ठा logheader lh;
};
काष्ठा log log;

अटल व्योम recover_from_log(व्योम);
अटल व्योम commit();

व्योम
initlog(पूर्णांक dev, काष्ठा superblock *sb)
{
  अगर (माप(काष्ठा logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
अटल व्योम
install_trans(पूर्णांक recovering)
{
  पूर्णांक पुच्छ;

  क्रम (पुच्छ = 0; पुच्छ < log.lh.n; पुच्छ++) {
    अगर (recovering) {
      printk("recovering tail %d dst %d\n", पुच्छ, log.lh.block[पुच्छ]);
    }
    काष्ठा buf *lbuf = bread(log.dev, log.start + पुच्छ + 1); // read log block
    काष्ठा buf *dbuf = bread(log.dev, log.lh.block[पुच्छ]);   // read dst
    स_हटाओ(dbuf->तथ्य, lbuf->तथ्य, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    अगर (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
अटल व्योम
read_शीर्ष(व्योम)
{
  काष्ठा buf *buf = bread(log.dev, log.start);
  काष्ठा logheader *lh = (काष्ठा logheader *)(buf->तथ्य);
  पूर्णांक i;
  log.lh.n = lh->n;
  क्रम (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
अटल व्योम
write_शीर्ष(व्योम)
{
  काष्ठा buf *buf = bread(log.dev, log.start);
  काष्ठा logheader *hb = (काष्ठा logheader *)(buf->तथ्य);
  पूर्णांक i;
  hb->n = log.lh.n;
  क्रम (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

अटल व्योम
recover_from_log(व्योम)
{
  read_शीर्ष();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_शीर्ष(); // clear the log
}

// called at the start of each FS system call.
व्योम
begin_op(व्योम)
{
  acquire(&log.lock);
  जबतक (1) {
    अगर (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } अन्यथा अगर (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } अन्यथा {
      log.outstanding += 1;
      release(&log.lock);
      अवरोध;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
व्योम
end_op(व्योम)
{
  पूर्णांक do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  अगर (log.committing)
    panic("log.committing");
  अगर (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } अन्यथा {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  अगर (do_commit) {
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
अटल व्योम
write_log(व्योम)
{
  पूर्णांक पुच्छ;

  क्रम (पुच्छ = 0; पुच्छ < log.lh.n; पुच्छ++) {
    काष्ठा buf *to = bread(log.dev, log.start + पुच्छ + 1); // log block
    काष्ठा buf *from = bread(log.dev, log.lh.block[पुच्छ]); // cache block
    स_हटाओ(to->तथ्य, from->तथ्य, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

अटल व्योम
commit()
{
  अगर (log.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_शीर्ष();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    log.lh.n = 0;
    write_शीर्ष(); // Erase the transaction from the log
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
व्योम
log_write(काष्ठा buf *b)
{
  पूर्णांक i;

  acquire(&log.lock);
  अगर (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  अगर (log.outstanding < 1)
    panic("log_write outside of trans");

  क्रम (i = 0; i < log.lh.n; i++) {
    अगर (log.lh.block[i] == b->blockno) // log absorption
      अवरोध;
  }
  log.lh.block[i] = b->blockno;
  अगर (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(व्योम)
{
  acquire(&log.lock);
  अगर (log.committing || log.outstanding > 0) {
    पूर्णांक n = log.ncommit + 1;
    जबतक (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  वापस 0;
}
