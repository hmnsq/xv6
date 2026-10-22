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
  पूर्णाङ्क खण्डम्[LOGBLOCKS];
};

संरचना लघुगणकः {
  संरचना spinlock तालकम्;
  पूर्णाङ्क start;
  पूर्णाङ्क outstanding; // how many FS sys calls are executing.
  पूर्णाङ्क committing;  // in commit(), please wait.
  पूर्णाङ्क dev;
  पूर्णाङ्क ncommit;
  संरचना logheader lh;
};
संरचना लघुगणकः लघुगणकः;

स्थिर शून्य recover_from_log(शून्य);
स्थिर शून्य commit();

शून्य
initlog(पूर्णाङ्क dev, संरचना superblock *sb)
{
  यदि (परिमाण(संरचना logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&लघुगणकः.तालकम्, "log");
  लघुगणकः.start = sb->logstart;
  लघुगणकः.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
स्थिर शून्य
install_trans(पूर्णाङ्क recovering)
{
  पूर्णाङ्क पुच्छ;

  प्रति (पुच्छ = 0; पुच्छ < लघुगणकः.lh.n; पुच्छ++) {
    यदि (recovering) {
      printk("recovering tail %d dst %d\n", पुच्छ, लघुगणकः.lh.खण्डम्[पुच्छ]);
    }
    संरचना buf *lbuf = bread(लघुगणकः.dev, लघुगणकः.start + पुच्छ + 1); // read log block
    संरचना buf *dbuf = bread(लघुगणकः.dev, लघुगणकः.lh.खण्डम्[पुच्छ]);   // read dst
    memmove(dbuf->दत्त, lbuf->दत्त, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    यदि (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
स्थिर शून्य
read_शीर्ष(शून्य)
{
  संरचना buf *buf = bread(लघुगणकः.dev, लघुगणकः.start);
  संरचना logheader *lh = (संरचना logheader *)(buf->दत्त);
  पूर्णाङ्क i;
  लघुगणकः.lh.n = lh->n;
  प्रति (i = 0; i < लघुगणकः.lh.n; i++) {
    लघुगणकः.lh.खण्डम्[i] = lh->खण्डम्[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
स्थिर शून्य
write_शीर्ष(शून्य)
{
  संरचना buf *buf = bread(लघुगणकः.dev, लघुगणकः.start);
  संरचना logheader *hb = (संरचना logheader *)(buf->दत्त);
  पूर्णाङ्क i;
  hb->n = लघुगणकः.lh.n;
  प्रति (i = 0; i < लघुगणकः.lh.n; i++) {
    hb->खण्डम्[i] = लघुगणकः.lh.खण्डम्[i];
  }
  bwrite(buf);
  brelse(buf);
}

स्थिर शून्य
recover_from_log(शून्य)
{
  read_शीर्ष();
  install_trans(1); // if committed, copy from log to disk
  लघुगणकः.lh.n = 0;
  write_शीर्ष(); // clear the log
}

// called at the start of each FS system call.
शून्य
begin_op(शून्य)
{
  acquire(&लघुगणकः.तालकम्);
  यावत् (1) {
    यदि (लघुगणकः.committing) {
      sleep_prepare(&लघुगणकः);
      release(&लघुगणकः.तालकम्);
      sleep();
      acquire(&लघुगणकः.तालकम्);
    } अन्यथा यदि (लघुगणकः.lh.n + (लघुगणकः.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&लघुगणकः);
      release(&लघुगणकः.तालकम्);
      sleep();
      acquire(&लघुगणकः.तालकम्);
    } अन्यथा {
      लघुगणकः.outstanding += 1;
      release(&लघुगणकः.तालकम्);
      भङ्ग;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
शून्य
अन्तः_op(शून्य)
{
  पूर्णाङ्क do_commit = 0;

  acquire(&लघुगणकः.तालकम्);
  लघुगणकः.outstanding -= 1;
  यदि (लघुगणकः.committing)
    panic("log.committing");
  यदि (लघुगणकः.outstanding == 0) {
    do_commit = 1;
    लघुगणकः.committing = 1;
  } अन्यथा {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&लघुगणकः);
  }
  release(&लघुगणकः.तालकम्);

  यदि (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&लघुगणकः.तालकम्);
    लघुगणकः.committing = 0;
    लघुगणकः.ncommit += 1;
    wakeup(&लघुगणकः);
    release(&लघुगणकः.तालकम्);
  }
}

// Copy modified blocks from cache to log.
स्थिर शून्य
write_log(शून्य)
{
  पूर्णाङ्क पुच्छ;

  प्रति (पुच्छ = 0; पुच्छ < लघुगणकः.lh.n; पुच्छ++) {
    संरचना buf *गन्तव्यम् = bread(लघुगणकः.dev, लघुगणकः.start + पुच्छ + 1); // log block
    संरचना buf *from = bread(लघुगणकः.dev, लघुगणकः.lh.खण्डम्[पुच्छ]); // cache block
    memmove(गन्तव्यम्->दत्त, from->दत्त, BSIZE);
    bwrite(गन्तव्यम्); // write the log
    brelse(from);
    brelse(गन्तव्यम्);
  }
}

स्थिर शून्य
commit()
{
  यदि (लघुगणकः.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_शीर्ष();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    लघुगणकः.lh.n = 0;
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
शून्य
log_write(संरचना buf *b)
{
  पूर्णाङ्क i;

  acquire(&लघुगणकः.तालकम्);
  यदि (लघुगणकः.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  यदि (लघुगणकः.outstanding < 1)
    panic("log_write outside of trans");

  प्रति (i = 0; i < लघुगणकः.lh.n; i++) {
    यदि (लघुगणकः.lh.खण्डम्[i] == b->blockno) // log absorption
      भङ्ग;
  }
  लघुगणकः.lh.खण्डम्[i] = b->blockno;
  यदि (i == लघुगणकः.lh.n) { // Add new block to log?
    bpin(b);
    लघुगणकः.lh.n++;
  }
  release(&लघुगणकः.तालकम्);
}

uint64
sys_sync(शून्य)
{
  acquire(&लघुगणकः.तालकम्);
  यदि (लघुगणकः.committing || लघुगणकः.outstanding > 0) {
    पूर्णाङ्क n = लघुगणकः.ncommit + 1;
    यावत् (लघुगणकः.ncommit < n) {
      sleep_prepare(&लघुगणकः);
      release(&लघुगणकः.तालकम्);
      sleep();
      acquire(&लघुगणकः.तालकम्);
    }
  }
  release(&लघुगणकः.तालकम्);
  प्रत्यागम 0;
}
