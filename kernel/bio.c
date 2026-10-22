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

#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "sleeplock.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "fs.h"
#অন্তর্ভাৱ "buf.h"

সংরচনা {
  সংরচনা spinlock lock;
  সংরচনা buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  সংরচনা buf head;
} bcache;

শূন্য
binit(শূন্য)
{
  সংরচনা buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  বাবে (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
স্থির সংরচনা buf *
bget(uint dev, uint blockno)
{
  সংরচনা buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  বাবে (b = bcache.head.next; b != &bcache.head; b = b->next) {
    যদি (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      ঘূৰাই b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  বাবে (b = bcache.head.prev; b != &bcache.head; b = b->prev) {
    যদি (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      ঘূৰাই b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
সংরচনা buf *
bread(uint dev, uint blockno)
{
  সংরচনা buf *b;

  b = bget(dev, blockno);
  যদি (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  ঘূৰাই b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
শূন্য
bwrite(সংরচনা buf *b)
{
  যদি (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
শূন্য
brelse(সংরচনা buf *b)
{
  যদি (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  যদি (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }

  release(&bcache.lock);
}

শূন্য
bpin(সংরচনা buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

শূন্য
bunpin(সংরচনা buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
