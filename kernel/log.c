#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "buf.h"

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
संरचना logheader {
  पूर्णाङ्क n;
  पूर्णाङ्क block[LOGBLOCKS];
};

संरचना log {
  संरचना spinlock lock;
  पूर्णाङ्क start;
  पूर्णाङ्क outstanding; // how many FS sys calls are executing.
  पूर्णाङ्क committing;  // in commit(), please wait.
  पूर्णाङ्क dev;
  पूर्णाङ्क ncommit;
  संरचना logheader lh;
};
संरचना log log;

स्थिर शून्य recover_from_log(शून्य);
स्थिर शून्य commit();

शून्य
initlog(पूर्णाङ्क dev, संरचना superblock *sb)
{
  यदि (परिमाण(संरचना logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&log.lock, "log");
  log.start = sb->logstart;
  log.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
स्थिर शून्य
install_trans(पूर्णाङ्क recovering)
{
  पूर्णाङ्क tail;

  प्रति (tail = 0; tail < log.lh.n; tail++) {
    यदि (recovering) {
      printk("recovering tail %d dst %d\n", tail, log.lh.block[tail]);
    }
    संरचना buf *lbuf = bread(log.dev, log.start + tail + 1); // read log block
    संरचना buf *dbuf = bread(log.dev, log.lh.block[tail]);   // read dst
    स_हटाओ(dbuf->data, lbuf->data, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    यदि (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
स्थिर शून्य
read_head(शून्य)
{
  संरचना buf *buf = bread(log.dev, log.start);
  संरचना logheader *lh = (संरचना logheader *)(buf->data);
  पूर्णाङ्क i;
  log.lh.n = lh->n;
  प्रति (i = 0; i < log.lh.n; i++) {
    log.lh.block[i] = lh->block[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
स्थिर शून्य
write_head(शून्य)
{
  संरचना buf *buf = bread(log.dev, log.start);
  संरचना logheader *hb = (संरचना logheader *)(buf->data);
  पूर्णाङ्क i;
  hb->n = log.lh.n;
  प्रति (i = 0; i < log.lh.n; i++) {
    hb->block[i] = log.lh.block[i];
  }
  bwrite(buf);
  brelse(buf);
}

स्थिर शून्य
recover_from_log(शून्य)
{
  read_head();
  install_trans(1); // if committed, copy from log to disk
  log.lh.n = 0;
  write_head(); // clear the log
}

// called at the start of each FS system call.
शून्य
begin_op(शून्य)
{
  acquire(&log.lock);
  यावत् (1) {
    यदि (log.committing) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } अन्यथा यदि (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    } अन्यथा {
      log.outstanding += 1;
      release(&log.lock);
      भङ्ग;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
शून्य
end_op(शून्य)
{
  पूर्णाङ्क do_commit = 0;

  acquire(&log.lock);
  log.outstanding -= 1;
  यदि (log.committing)
    panic("log.committing");
  यदि (log.outstanding == 0) {
    do_commit = 1;
    log.committing = 1;
  } अन्यथा {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&log);
  }
  release(&log.lock);

  यदि (do_commit) {
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
स्थिर शून्य
write_log(शून्य)
{
  पूर्णाङ्क tail;

  प्रति (tail = 0; tail < log.lh.n; tail++) {
    संरचना buf *to = bread(log.dev, log.start + tail + 1); // log block
    संरचना buf *from = bread(log.dev, log.lh.block[tail]); // cache block
    स_हटाओ(to->data, from->data, BSIZE);
    bwrite(to); // write the log
    brelse(from);
    brelse(to);
  }
}

स्थिर शून्य
commit()
{
  यदि (log.lh.n > 0) {
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
शून्य
log_write(संरचना buf *b)
{
  पूर्णाङ्क i;

  acquire(&log.lock);
  यदि (log.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  यदि (log.outstanding < 1)
    panic("log_write outside of trans");

  प्रति (i = 0; i < log.lh.n; i++) {
    यदि (log.lh.block[i] == b->blockno) // log absorption
      भङ्ग;
  }
  log.lh.block[i] = b->blockno;
  यदि (i == log.lh.n) { // Add new block to log?
    bpin(b);
    log.lh.n++;
  }
  release(&log.lock);
}

uint64
sys_sync(शून्य)
{
  acquire(&log.lock);
  यदि (log.committing || log.outstanding > 0) {
    पूर्णाङ्क n = log.ncommit + 1;
    यावत् (log.ncommit < n) {
      sleep_prepare(&log);
      release(&log.lock);
      sleep();
      acquire(&log.lock);
    }
  }
  release(&log.lock);
  प्रत्यागम 0;
}
