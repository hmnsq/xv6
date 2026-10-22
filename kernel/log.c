#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "buf.h"

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
ਬਣਤਰ logheader {
  ਪੂਰਨ n;
  ਪੂਰਨ ਖੰਡ[LOGBLOCKS];
};

ਬਣਤਰ ਲਘੂਗਣਕ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਪੂਰਨ ਸ਼ੁਰੂ;
  ਪੂਰਨ outstanding; // how many FS sys calls are executing.
  ਪੂਰਨ committing;  // in commit(), please wait.
  ਪੂਰਨ dev;
  ਪੂਰਨ ncommit;
  ਬਣਤਰ logheader lh;
};
ਬਣਤਰ ਲਘੂਗਣਕ ਲਘੂਗਣਕ;

ਸਥਿਰ ਖਾਲੀ recover_from_log(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ commit();

ਖਾਲੀ
initlog(ਪੂਰਨ dev, ਬਣਤਰ superblock *sb)
{
  ਜੇ (ਆਕਾਰ(ਬਣਤਰ logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&ਲਘੂਗਣਕ.ਤਾਲਾ, "log");
  ਲਘੂਗਣਕ.ਸ਼ੁਰੂ = sb->logstart;
  ਲਘੂਗਣਕ.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ਸਥਿਰ ਖਾਲੀ
install_trans(ਪੂਰਨ recovering)
{
  ਪੂਰਨ ਪੂਛ;

  ਲਈ (ਪੂਛ = 0; ਪੂਛ < ਲਘੂਗਣਕ.lh.n; ਪੂਛ++) {
    ਜੇ (recovering) {
      printk("recovering tail %d dst %d\n", ਪੂਛ, ਲਘੂਗਣਕ.lh.ਖੰਡ[ਪੂਛ]);
    }
    ਬਣਤਰ buf *lbuf = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.ਸ਼ੁਰੂ + ਪੂਛ + 1); // read log block
    ਬਣਤਰ buf *dbuf = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.lh.ਖੰਡ[ਪੂਛ]);   // read dst
    memmove(dbuf->ਅੰਕੜੇ, lbuf->ਅੰਕੜੇ, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ਜੇ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ਸਥਿਰ ਖਾਲੀ
read_ਸਿਰ(ਖਾਲੀ)
{
  ਬਣਤਰ buf *buf = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.ਸ਼ੁਰੂ);
  ਬਣਤਰ logheader *lh = (ਬਣਤਰ logheader *)(buf->ਅੰਕੜੇ);
  ਪੂਰਨ i;
  ਲਘੂਗਣਕ.lh.n = lh->n;
  ਲਈ (i = 0; i < ਲਘੂਗਣਕ.lh.n; i++) {
    ਲਘੂਗਣਕ.lh.ਖੰਡ[i] = lh->ਖੰਡ[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ਸਥਿਰ ਖਾਲੀ
write_ਸਿਰ(ਖਾਲੀ)
{
  ਬਣਤਰ buf *buf = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.ਸ਼ੁਰੂ);
  ਬਣਤਰ logheader *hb = (ਬਣਤਰ logheader *)(buf->ਅੰਕੜੇ);
  ਪੂਰਨ i;
  hb->n = ਲਘੂਗਣਕ.lh.n;
  ਲਈ (i = 0; i < ਲਘੂਗਣਕ.lh.n; i++) {
    hb->ਖੰਡ[i] = ਲਘੂਗਣਕ.lh.ਖੰਡ[i];
  }
  bwrite(buf);
  brelse(buf);
}

ਸਥਿਰ ਖਾਲੀ
recover_from_log(ਖਾਲੀ)
{
  read_ਸਿਰ();
  install_trans(1); // if committed, copy from log to disk
  ਲਘੂਗਣਕ.lh.n = 0;
  write_ਸਿਰ(); // clear the log
}

// called at the start of each FS system call.
ਖਾਲੀ
begin_op(ਖਾਲੀ)
{
  acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  ਜਦੋਂ (1) {
    ਜੇ (ਲਘੂਗਣਕ.committing) {
      sleep_prepare(&ਲਘੂਗਣਕ);
      release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
      sleep();
      acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
    } ਨਹੀਂਤਾਂ ਜੇ (ਲਘੂਗਣਕ.lh.n + (ਲਘੂਗਣਕ.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&ਲਘੂਗਣਕ);
      release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
      sleep();
      acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
    } ਨਹੀਂਤਾਂ {
      ਲਘੂਗਣਕ.outstanding += 1;
      release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
      ਰੋਕੋ;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
ਖਾਲੀ
ਸਿਰਾ_op(ਖਾਲੀ)
{
  ਪੂਰਨ do_commit = 0;

  acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  ਲਘੂਗਣਕ.outstanding -= 1;
  ਜੇ (ਲਘੂਗਣਕ.committing)
    panic("log.committing");
  ਜੇ (ਲਘੂਗਣਕ.outstanding == 0) {
    do_commit = 1;
    ਲਘੂਗਣਕ.committing = 1;
  } ਨਹੀਂਤਾਂ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&ਲਘੂਗਣਕ);
  }
  release(&ਲਘੂਗਣਕ.ਤਾਲਾ);

  ਜੇ (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
    ਲਘੂਗਣਕ.committing = 0;
    ਲਘੂਗਣਕ.ncommit += 1;
    wakeup(&ਲਘੂਗਣਕ);
    release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  }
}

// Copy modified blocks from cache to log.
ਸਥਿਰ ਖਾਲੀ
write_log(ਖਾਲੀ)
{
  ਪੂਰਨ ਪੂਛ;

  ਲਈ (ਪੂਛ = 0; ਪੂਛ < ਲਘੂਗਣਕ.lh.n; ਪੂਛ++) {
    ਬਣਤਰ buf *ਮੰਜ਼ਿਲ = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.ਸ਼ੁਰੂ + ਪੂਛ + 1); // log block
    ਬਣਤਰ buf *from = bread(ਲਘੂਗਣਕ.dev, ਲਘੂਗਣਕ.lh.ਖੰਡ[ਪੂਛ]); // cache block
    memmove(ਮੰਜ਼ਿਲ->ਅੰਕੜੇ, from->ਅੰਕੜੇ, BSIZE);
    bwrite(ਮੰਜ਼ਿਲ); // write the log
    brelse(from);
    brelse(ਮੰਜ਼ਿਲ);
  }
}

ਸਥਿਰ ਖਾਲੀ
commit()
{
  ਜੇ (ਲਘੂਗਣਕ.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_ਸਿਰ();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    ਲਘੂਗਣਕ.lh.n = 0;
    write_ਸਿਰ(); // Erase the transaction from the log
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
ਖਾਲੀ
log_write(ਬਣਤਰ buf *b)
{
  ਪੂਰਨ i;

  acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  ਜੇ (ਲਘੂਗਣਕ.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ਜੇ (ਲਘੂਗਣਕ.outstanding < 1)
    panic("log_write outside of trans");

  ਲਈ (i = 0; i < ਲਘੂਗਣਕ.lh.n; i++) {
    ਜੇ (ਲਘੂਗਣਕ.lh.ਖੰਡ[i] == b->blockno) // log absorption
      ਰੋਕੋ;
  }
  ਲਘੂਗਣਕ.lh.ਖੰਡ[i] = b->blockno;
  ਜੇ (i == ਲਘੂਗਣਕ.lh.n) { // Add new block to log?
    bpin(b);
    ਲਘੂਗਣਕ.lh.n++;
  }
  release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
}

uint64
sys_sync(ਖਾਲੀ)
{
  acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  ਜੇ (ਲਘੂਗਣਕ.committing || ਲਘੂਗਣਕ.outstanding > 0) {
    ਪੂਰਨ n = ਲਘੂਗਣਕ.ncommit + 1;
    ਜਦੋਂ (ਲਘੂਗਣਕ.ncommit < n) {
      sleep_prepare(&ਲਘੂਗਣਕ);
      release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
      sleep();
      acquire(&ਲਘੂਗਣਕ.ਤਾਲਾ);
    }
  }
  release(&ਲਘੂਗਣਕ.ਤਾਲਾ);
  ਵਾਪਸ 0;
}
