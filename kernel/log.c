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
ರಚನೆ logheader {
  ಪೂರ್ಣಾಂಕ n;
  ಪೂರ್ಣಾಂಕ ಖಂಡ[LOGBLOCKS];
};

ರಚನೆ ಲಘುಗಣಕ {
  ರಚನೆ spinlock ಬೀಗ;
  ಪೂರ್ಣಾಂಕ ಆರಂಭ;
  ಪೂರ್ಣಾಂಕ outstanding; // how many FS sys calls are executing.
  ಪೂರ್ಣಾಂಕ committing;  // in commit(), please wait.
  ಪೂರ್ಣಾಂಕ dev;
  ಪೂರ್ಣಾಂಕ ncommit;
  ರಚನೆ logheader lh;
};
ರಚನೆ ಲಘುಗಣಕ ಲಘುಗಣಕ;

ಸ್ಥಿರ ಶೂನ್ಯ recover_from_log(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ commit();

ಶೂನ್ಯ
initlog(ಪೂರ್ಣಾಂಕ dev, ರಚನೆ superblock *sb)
{
  ಆದರೆ (ಗಾತ್ರ(ರಚನೆ logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&ಲಘುಗಣಕ.ಬೀಗ, "log");
  ಲಘುಗಣಕ.ಆರಂಭ = sb->logstart;
  ಲಘುಗಣಕ.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ಸ್ಥಿರ ಶೂನ್ಯ
install_trans(ಪೂರ್ಣಾಂಕ recovering)
{
  ಪೂರ್ಣಾಂಕ ಬಾಲ;

  ಗಾಗಿ (ಬಾಲ = 0; ಬಾಲ < ಲಘುಗಣಕ.lh.n; ಬಾಲ++) {
    ಆದರೆ (recovering) {
      printk("recovering tail %d dst %d\n", ಬಾಲ, ಲಘುಗಣಕ.lh.ಖಂಡ[ಬಾಲ]);
    }
    ರಚನೆ buf *lbuf = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.ಆರಂಭ + ಬಾಲ + 1); // read log block
    ರಚನೆ buf *dbuf = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.lh.ಖಂಡ[ಬಾಲ]);   // read dst
    memmove(dbuf->ದತ್ತಾಂಶ, lbuf->ದತ್ತಾಂಶ, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ಆದರೆ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ಸ್ಥಿರ ಶೂನ್ಯ
read_ತಲೆ(ಶೂನ್ಯ)
{
  ರಚನೆ buf *buf = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.ಆರಂಭ);
  ರಚನೆ logheader *lh = (ರಚನೆ logheader *)(buf->ದತ್ತಾಂಶ);
  ಪೂರ್ಣಾಂಕ i;
  ಲಘುಗಣಕ.lh.n = lh->n;
  ಗಾಗಿ (i = 0; i < ಲಘುಗಣಕ.lh.n; i++) {
    ಲಘುಗಣಕ.lh.ಖಂಡ[i] = lh->ಖಂಡ[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ಸ್ಥಿರ ಶೂನ್ಯ
write_ತಲೆ(ಶೂನ್ಯ)
{
  ರಚನೆ buf *buf = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.ಆರಂಭ);
  ರಚನೆ logheader *hb = (ರಚನೆ logheader *)(buf->ದತ್ತಾಂಶ);
  ಪೂರ್ಣಾಂಕ i;
  hb->n = ಲಘುಗಣಕ.lh.n;
  ಗಾಗಿ (i = 0; i < ಲಘುಗಣಕ.lh.n; i++) {
    hb->ಖಂಡ[i] = ಲಘುಗಣಕ.lh.ಖಂಡ[i];
  }
  bwrite(buf);
  brelse(buf);
}

ಸ್ಥಿರ ಶೂನ್ಯ
recover_from_log(ಶೂನ್ಯ)
{
  read_ತಲೆ();
  install_trans(1); // if committed, copy from log to disk
  ಲಘುಗಣಕ.lh.n = 0;
  write_ತಲೆ(); // clear the log
}

// called at the start of each FS system call.
ಶೂನ್ಯ
begin_op(ಶೂನ್ಯ)
{
  acquire(&ಲಘುಗಣಕ.ಬೀಗ);
  ಆಗ (1) {
    ಆದರೆ (ಲಘುಗಣಕ.committing) {
      sleep_prepare(&ಲಘುಗಣಕ);
      release(&ಲಘುಗಣಕ.ಬೀಗ);
      sleep();
      acquire(&ಲಘುಗಣಕ.ಬೀಗ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಘುಗಣಕ.lh.n + (ಲಘುಗಣಕ.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&ಲಘುಗಣಕ);
      release(&ಲಘುಗಣಕ.ಬೀಗ);
      sleep();
      acquire(&ಲಘುಗಣಕ.ಬೀಗ);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಲಘುಗಣಕ.outstanding += 1;
      release(&ಲಘುಗಣಕ.ಬೀಗ);
      ನಿಲ್ಲಿಸು;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
ಶೂನ್ಯ
ಕೊನೆ_op(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ do_commit = 0;

  acquire(&ಲಘುಗಣಕ.ಬೀಗ);
  ಲಘುಗಣಕ.outstanding -= 1;
  ಆದರೆ (ಲಘುಗಣಕ.committing)
    panic("log.committing");
  ಆದರೆ (ಲಘುಗಣಕ.outstanding == 0) {
    do_commit = 1;
    ಲಘುಗಣಕ.committing = 1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&ಲಘುಗಣಕ);
  }
  release(&ಲಘುಗಣಕ.ಬೀಗ);

  ಆದರೆ (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&ಲಘುಗಣಕ.ಬೀಗ);
    ಲಘುಗಣಕ.committing = 0;
    ಲಘುಗಣಕ.ncommit += 1;
    wakeup(&ಲಘುಗಣಕ);
    release(&ಲಘುಗಣಕ.ಬೀಗ);
  }
}

// Copy modified blocks from cache to log.
ಸ್ಥಿರ ಶೂನ್ಯ
write_log(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ ಬಾಲ;

  ಗಾಗಿ (ಬಾಲ = 0; ಬಾಲ < ಲಘುಗಣಕ.lh.n; ಬಾಲ++) {
    ರಚನೆ buf *ಗಮ್ಯ = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.ಆರಂಭ + ಬಾಲ + 1); // log block
    ರಚನೆ buf *from = bread(ಲಘುಗಣಕ.dev, ಲಘುಗಣಕ.lh.ಖಂಡ[ಬಾಲ]); // cache block
    memmove(ಗಮ್ಯ->ದತ್ತಾಂಶ, from->ದತ್ತಾಂಶ, BSIZE);
    bwrite(ಗಮ್ಯ); // write the log
    brelse(from);
    brelse(ಗಮ್ಯ);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ
commit()
{
  ಆದರೆ (ಲಘುಗಣಕ.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_ತಲೆ();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    ಲಘುಗಣಕ.lh.n = 0;
    write_ತಲೆ(); // Erase the transaction from the log
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
log_write(ರಚನೆ buf *b)
{
  ಪೂರ್ಣಾಂಕ i;

  acquire(&ಲಘುಗಣಕ.ಬೀಗ);
  ಆದರೆ (ಲಘುಗಣಕ.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ಆದರೆ (ಲಘುಗಣಕ.outstanding < 1)
    panic("log_write outside of trans");

  ಗಾಗಿ (i = 0; i < ಲಘುಗಣಕ.lh.n; i++) {
    ಆದರೆ (ಲಘುಗಣಕ.lh.ಖಂಡ[i] == b->blockno) // log absorption
      ನಿಲ್ಲಿಸು;
  }
  ಲಘುಗಣಕ.lh.ಖಂಡ[i] = b->blockno;
  ಆದರೆ (i == ಲಘುಗಣಕ.lh.n) { // Add new block to log?
    bpin(b);
    ಲಘುಗಣಕ.lh.n++;
  }
  release(&ಲಘುಗಣಕ.ಬೀಗ);
}

uint64
sys_sync(ಶೂನ್ಯ)
{
  acquire(&ಲಘುಗಣಕ.ಬೀಗ);
  ಆದರೆ (ಲಘುಗಣಕ.committing || ಲಘುಗಣಕ.outstanding > 0) {
    ಪೂರ್ಣಾಂಕ n = ಲಘುಗಣಕ.ncommit + 1;
    ಆಗ (ಲಘುಗಣಕ.ncommit < n) {
      sleep_prepare(&ಲಘುಗಣಕ);
      release(&ಲಘುಗಣಕ.ಬೀಗ);
      sleep();
      acquire(&ಲಘುಗಣಕ.ಬೀಗ);
    }
  }
  release(&ಲಘುಗಣಕ.ಬೀಗ);
  ಹಿಂತಿರುಗಿ 0;
}
