#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "buf.h"

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
నిర్మాణం logheader {
  పూర్ణాంకం n;
  పూర్ణాంకం ఖండం[LOGBLOCKS];
};

నిర్మాణం సంవర్గమానం {
  నిర్మాణం spinlock తాళం;
  పూర్ణాంకం ప్రారంభం;
  పూర్ణాంకం outstanding; // how many FS sys calls are executing.
  పూర్ణాంకం committing;  // in commit(), please wait.
  పూర్ణాంకం dev;
  పూర్ణాంకం ncommit;
  నిర్మాణం logheader lh;
};
నిర్మాణం సంవర్గమానం సంవర్గమానం;

స్థిర శూన్య recover_from_log(శూన్య);
స్థిర శూన్య commit();

శూన్య
initlog(పూర్ణాంకం dev, నిర్మాణం superblock *sb)
{
  ఒకవేళ (పరిమాణం(నిర్మాణం logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&సంవర్గమానం.తాళం, "log");
  సంవర్గమానం.ప్రారంభం = sb->logstart;
  సంవర్గమానం.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
స్థిర శూన్య
install_trans(పూర్ణాంకం recovering)
{
  పూర్ణాంకం తోక;

  కొరకు (తోక = 0; తోక < సంవర్గమానం.lh.n; తోక++) {
    ఒకవేళ (recovering) {
      printk("recovering tail %d dst %d\n", తోక, సంవర్గమానం.lh.ఖండం[తోక]);
    }
    నిర్మాణం buf *lbuf = bread(సంవర్గమానం.dev, సంవర్గమానం.ప్రారంభం + తోక + 1); // read log block
    నిర్మాణం buf *dbuf = bread(సంవర్గమానం.dev, సంవర్గమానం.lh.ఖండం[తోక]);   // read dst
    memmove(dbuf->దత్తాంశం, lbuf->దత్తాంశం, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    ఒకవేళ (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
స్థిర శూన్య
read_తల(శూన్య)
{
  నిర్మాణం buf *buf = bread(సంవర్గమానం.dev, సంవర్గమానం.ప్రారంభం);
  నిర్మాణం logheader *lh = (నిర్మాణం logheader *)(buf->దత్తాంశం);
  పూర్ణాంకం i;
  సంవర్గమానం.lh.n = lh->n;
  కొరకు (i = 0; i < సంవర్గమానం.lh.n; i++) {
    సంవర్గమానం.lh.ఖండం[i] = lh->ఖండం[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
స్థిర శూన్య
write_తల(శూన్య)
{
  నిర్మాణం buf *buf = bread(సంవర్గమానం.dev, సంవర్గమానం.ప్రారంభం);
  నిర్మాణం logheader *hb = (నిర్మాణం logheader *)(buf->దత్తాంశం);
  పూర్ణాంకం i;
  hb->n = సంవర్గమానం.lh.n;
  కొరకు (i = 0; i < సంవర్గమానం.lh.n; i++) {
    hb->ఖండం[i] = సంవర్గమానం.lh.ఖండం[i];
  }
  bwrite(buf);
  brelse(buf);
}

స్థిర శూన్య
recover_from_log(శూన్య)
{
  read_తల();
  install_trans(1); // if committed, copy from log to disk
  సంవర్గమానం.lh.n = 0;
  write_తల(); // clear the log
}

// called at the start of each FS system call.
శూన్య
begin_op(శూన్య)
{
  acquire(&సంవర్గమానం.తాళం);
  అయితే (1) {
    ఒకవేళ (సంవర్గమానం.committing) {
      sleep_prepare(&సంవర్గమానం);
      release(&సంవర్గమానం.తాళం);
      sleep();
      acquire(&సంవర్గమానం.తాళం);
    } లేకపోతే ఒకవేళ (సంవర్గమానం.lh.n + (సంవర్గమానం.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&సంవర్గమానం);
      release(&సంవర్గమానం.తాళం);
      sleep();
      acquire(&సంవర్గమానం.తాళం);
    } లేకపోతే {
      సంవర్గమానం.outstanding += 1;
      release(&సంవర్గమానం.తాళం);
      ఆపు;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
శూన్య
చివర_op(శూన్య)
{
  పూర్ణాంకం do_commit = 0;

  acquire(&సంవర్గమానం.తాళం);
  సంవర్గమానం.outstanding -= 1;
  ఒకవేళ (సంవర్గమానం.committing)
    panic("log.committing");
  ఒకవేళ (సంవర్గమానం.outstanding == 0) {
    do_commit = 1;
    సంవర్గమానం.committing = 1;
  } లేకపోతే {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&సంవర్గమానం);
  }
  release(&సంవర్గమానం.తాళం);

  ఒకవేళ (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&సంవర్గమానం.తాళం);
    సంవర్గమానం.committing = 0;
    సంవర్గమానం.ncommit += 1;
    wakeup(&సంవర్గమానం);
    release(&సంవర్గమానం.తాళం);
  }
}

// Copy modified blocks from cache to log.
స్థిర శూన్య
write_log(శూన్య)
{
  పూర్ణాంకం తోక;

  కొరకు (తోక = 0; తోక < సంవర్గమానం.lh.n; తోక++) {
    నిర్మాణం buf *గమ్యం = bread(సంవర్గమానం.dev, సంవర్గమానం.ప్రారంభం + తోక + 1); // log block
    నిర్మాణం buf *from = bread(సంవర్గమానం.dev, సంవర్గమానం.lh.ఖండం[తోక]); // cache block
    memmove(గమ్యం->దత్తాంశం, from->దత్తాంశం, BSIZE);
    bwrite(గమ్యం); // write the log
    brelse(from);
    brelse(గమ్యం);
  }
}

స్థిర శూన్య
commit()
{
  ఒకవేళ (సంవర్గమానం.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_తల();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    సంవర్గమానం.lh.n = 0;
    write_తల(); // Erase the transaction from the log
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
శూన్య
log_write(నిర్మాణం buf *b)
{
  పూర్ణాంకం i;

  acquire(&సంవర్గమానం.తాళం);
  ఒకవేళ (సంవర్గమానం.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  ఒకవేళ (సంవర్గమానం.outstanding < 1)
    panic("log_write outside of trans");

  కొరకు (i = 0; i < సంవర్గమానం.lh.n; i++) {
    ఒకవేళ (సంవర్గమానం.lh.ఖండం[i] == b->blockno) // log absorption
      ఆపు;
  }
  సంవర్గమానం.lh.ఖండం[i] = b->blockno;
  ఒకవేళ (i == సంవర్గమానం.lh.n) { // Add new block to log?
    bpin(b);
    సంవర్గమానం.lh.n++;
  }
  release(&సంవర్గమానం.తాళం);
}

uint64
sys_sync(శూన్య)
{
  acquire(&సంవర్గమానం.తాళం);
  ఒకవేళ (సంవర్గమానం.committing || సంవర్గమానం.outstanding > 0) {
    పూర్ణాంకం n = సంవర్గమానం.ncommit + 1;
    అయితే (సంవర్గమానం.ncommit < n) {
      sleep_prepare(&సంవర్గమానం);
      release(&సంవర్గమానం.తాళం);
      sleep();
      acquire(&సంవర్గమానం.తాళం);
    }
  }
  release(&సంవర్గమానం.తాళం);
  తిరిగి 0;
}
