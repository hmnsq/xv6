// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.

#شامل "types.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "fs.h"
#شامل "buf.h"

ساخت {
  ساخت spinlock تالا;
  ساخت buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  ساخت buf سر;
} bcache;

خالی
binit(خالی)
{
  ساخت buf *b;

  initlock(&bcache.تالا, "bcache");

  // Create linked list of buffers
  bcache.سر.پچھلا = &bcache.سر;
  bcache.سر.اگلا = &bcache.سر;
  برائے (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->اگلا = bcache.سر.اگلا;
    b->پچھلا = &bcache.سر;
    initsleeplock(&b->تالا, "buffer");
    bcache.سر.اگلا->پچھلا = b;
    bcache.سر.اگلا = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
ساکن ساخت buf *
bget(uint dev, uint blockno)
{
  ساخت buf *b;

  acquire(&bcache.تالا);

  // Is the block already cached?
  برائے (b = bcache.سر.اگلا; b != &bcache.سر; b = b->اگلا) {
    اگر (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.تالا);
      acquiresleep(&b->تالا);
      واپس b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  برائے (b = bcache.سر.پچھلا; b != &bcache.سر; b = b->پچھلا) {
    اگر (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.تالا);
      acquiresleep(&b->تالا);
      واپس b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
ساخت buf *
bread(uint dev, uint blockno)
{
  ساخت buf *b;

  b = bget(dev, blockno);
  اگر (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  واپس b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
خالی
bwrite(ساخت buf *b)
{
  اگر (!holdingsleep(&b->تالا))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
خالی
brelse(ساخت buf *b)
{
  اگر (!holdingsleep(&b->تالا))
    panic("brelse");

  releasesleep(&b->تالا);

  acquire(&bcache.تالا);
  b->refcnt--;
  اگر (b->refcnt == 0) {
    // no one is waiting for it.
    b->اگلا->پچھلا = b->پچھلا;
    b->پچھلا->اگلا = b->اگلا;
    b->اگلا = bcache.سر.اگلا;
    b->پچھلا = &bcache.سر;
    bcache.سر.اگلا->پچھلا = b;
    bcache.سر.اگلا = b;
  }

  release(&bcache.تالا);
}

خالی
bpin(ساخت buf *b)
{
  acquire(&bcache.تالا);
  b->refcnt++;
  release(&bcache.تالا);
}

خالی
bunpin(ساخت buf *b)
{
  acquire(&bcache.تالا);
  b->refcnt--;
  release(&bcache.تالا);
}
