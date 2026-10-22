#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "buf.h"

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
ساخت logheader {
  صحیح n;
  صحیح بلاک[LOGBLOCKS];
};

ساخت لوگارتھم {
  ساخت spinlock تالا;
  صحیح آغاز;
  صحیح outstanding; // how many FS sys calls are executing.
  صحیح committing;  // in commit(), please wait.
  صحیح dev;
  صحیح ncommit;
  ساخت logheader lh;
};
ساخت لوگارتھم لوگارتھم;

ساکن خالی recover_from_log(خالی);
ساکن خالی commit();

خالی
initlog(صحیح dev, ساخت superblock *sb)
{
  اگر (حجم(ساخت logheader) >= BSIZE)
    panic("initlog: too big logheader");

  initlock(&لوگارتھم.تالا, "log");
  لوگارتھم.آغاز = sb->logstart;
  لوگارتھم.dev = dev;
  recover_from_log();
}

// Copy committed blocks from log to their home location
ساکن خالی
install_trans(صحیح recovering)
{
  صحیح دم;

  برائے (دم = 0; دم < لوگارتھم.lh.n; دم++) {
    اگر (recovering) {
      printk("recovering tail %d dst %d\n", دم, لوگارتھم.lh.بلاک[دم]);
    }
    ساخت buf *lbuf = bread(لوگارتھم.dev, لوگارتھم.آغاز + دم + 1); // read log block
    ساخت buf *dbuf = bread(لوگارتھم.dev, لوگارتھم.lh.بلاک[دم]);   // read dst
    memmove(dbuf->ڈیٹا, lbuf->ڈیٹا, BSIZE); // copy block to dst
    bwrite(dbuf);                           // write dst to disk
    اگر (recovering == 0)
      bunpin(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
ساکن خالی
read_سر(خالی)
{
  ساخت buf *buf = bread(لوگارتھم.dev, لوگارتھم.آغاز);
  ساخت logheader *lh = (ساخت logheader *)(buf->ڈیٹا);
  صحیح i;
  لوگارتھم.lh.n = lh->n;
  برائے (i = 0; i < لوگارتھم.lh.n; i++) {
    لوگارتھم.lh.بلاک[i] = lh->بلاک[i];
  }
  brelse(buf);
}

// Write in-memory log header to disk.
// This is the true point at which the
// current transaction commits.
ساکن خالی
write_سر(خالی)
{
  ساخت buf *buf = bread(لوگارتھم.dev, لوگارتھم.آغاز);
  ساخت logheader *hb = (ساخت logheader *)(buf->ڈیٹا);
  صحیح i;
  hb->n = لوگارتھم.lh.n;
  برائے (i = 0; i < لوگارتھم.lh.n; i++) {
    hb->بلاک[i] = لوگارتھم.lh.بلاک[i];
  }
  bwrite(buf);
  brelse(buf);
}

ساکن خالی
recover_from_log(خالی)
{
  read_سر();
  install_trans(1); // if committed, copy from log to disk
  لوگارتھم.lh.n = 0;
  write_سر(); // clear the log
}

// called at the start of each FS system call.
خالی
begin_op(خالی)
{
  acquire(&لوگارتھم.تالا);
  جب (1) {
    اگر (لوگارتھم.committing) {
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.تالا);
      sleep();
      acquire(&لوگارتھم.تالا);
    } ورنہ اگر (لوگارتھم.lh.n + (لوگارتھم.outstanding + 1) * MAXOPBLOCKS > LOGBLOCKS) {
      // this op might exhaust log space; wait for commit.
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.تالا);
      sleep();
      acquire(&لوگارتھم.تالا);
    } ورنہ {
      لوگارتھم.outstanding += 1;
      release(&لوگارتھم.تالا);
      توڑو;
    }
  }
}

// called at the end of each FS system call.
// commits if this was the last outstanding operation.
خالی
انتہا_op(خالی)
{
  صحیح do_commit = 0;

  acquire(&لوگارتھم.تالا);
  لوگارتھم.outstanding -= 1;
  اگر (لوگارتھم.committing)
    panic("log.committing");
  اگر (لوگارتھم.outstanding == 0) {
    do_commit = 1;
    لوگارتھم.committing = 1;
  } ورنہ {
    // begin_op() may be waiting for log space,
    // and decrementing log.outstanding has decreased
    // the amount of reserved space.
    wakeup(&لوگارتھم);
  }
  release(&لوگارتھم.تالا);

  اگر (do_commit) {
    // call commit w/o holding locks, since not allowed
    // to sleep with locks.
    commit();
    acquire(&لوگارتھم.تالا);
    لوگارتھم.committing = 0;
    لوگارتھم.ncommit += 1;
    wakeup(&لوگارتھم);
    release(&لوگارتھم.تالا);
  }
}

// Copy modified blocks from cache to log.
ساکن خالی
write_log(خالی)
{
  صحیح دم;

  برائے (دم = 0; دم < لوگارتھم.lh.n; دم++) {
    ساخت buf *مقصد = bread(لوگارتھم.dev, لوگارتھم.آغاز + دم + 1); // log block
    ساخت buf *from = bread(لوگارتھم.dev, لوگارتھم.lh.بلاک[دم]); // cache block
    memmove(مقصد->ڈیٹا, from->ڈیٹا, BSIZE);
    bwrite(مقصد); // write the log
    brelse(from);
    brelse(مقصد);
  }
}

ساکن خالی
commit()
{
  اگر (لوگارتھم.lh.n > 0) {
    write_log();      // Write modified blocks from cache to log
    write_سر();     // Write header to disk -- the real commit
    install_trans(0); // Now install writes to home locations
    لوگارتھم.lh.n = 0;
    write_سر(); // Erase the transaction from the log
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
خالی
log_write(ساخت buf *b)
{
  صحیح i;

  acquire(&لوگارتھم.تالا);
  اگر (لوگارتھم.lh.n >= LOGBLOCKS)
    panic("too big a transaction");
  اگر (لوگارتھم.outstanding < 1)
    panic("log_write outside of trans");

  برائے (i = 0; i < لوگارتھم.lh.n; i++) {
    اگر (لوگارتھم.lh.بلاک[i] == b->blockno) // log absorption
      توڑو;
  }
  لوگارتھم.lh.بلاک[i] = b->blockno;
  اگر (i == لوگارتھم.lh.n) { // Add new block to log?
    bpin(b);
    لوگارتھم.lh.n++;
  }
  release(&لوگارتھم.تالا);
}

uint64
sys_sync(خالی)
{
  acquire(&لوگارتھم.تالا);
  اگر (لوگارتھم.committing || لوگارتھم.outstanding > 0) {
    صحیح n = لوگارتھم.ncommit + 1;
    جب (لوگارتھم.ncommit < n) {
      sleep_prepare(&لوگارتھم);
      release(&لوگارتھم.تالا);
      sleep();
      acquire(&لوگارتھم.تالا);
    }
  }
  release(&لوگارتھم.تالا);
  واپس 0;
}
