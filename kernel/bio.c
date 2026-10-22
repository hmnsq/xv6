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

#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "buf.h"

నిర్మాణం {
  నిర్మాణం spinlock తాళం;
  నిర్మాణం buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  నిర్మాణం buf తల;
} bcache;

శూన్య
binit(శూన్య)
{
  నిర్మాణం buf *b;

  initlock(&bcache.తాళం, "bcache");

  // Create linked list of buffers
  bcache.తల.prev = &bcache.తల;
  bcache.తల.తదుపరి = &bcache.తల;
  కొరకు (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->తదుపరి = bcache.తల.తదుపరి;
    b->prev = &bcache.తల;
    initsleeplock(&b->తాళం, "buffer");
    bcache.తల.తదుపరి->prev = b;
    bcache.తల.తదుపరి = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
స్థిర నిర్మాణం buf *
bget(uint dev, uint blockno)
{
  నిర్మాణం buf *b;

  acquire(&bcache.తాళం);

  // Is the block already cached?
  కొరకు (b = bcache.తల.తదుపరి; b != &bcache.తల; b = b->తదుపరి) {
    ఒకవేళ (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.తాళం);
      acquiresleep(&b->తాళం);
      తిరిగి b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  కొరకు (b = bcache.తల.prev; b != &bcache.తల; b = b->prev) {
    ఒకవేళ (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.తాళం);
      acquiresleep(&b->తాళం);
      తిరిగి b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
నిర్మాణం buf *
bread(uint dev, uint blockno)
{
  నిర్మాణం buf *b;

  b = bget(dev, blockno);
  ఒకవేళ (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  తిరిగి b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
శూన్య
bwrite(నిర్మాణం buf *b)
{
  ఒకవేళ (!holdingsleep(&b->తాళం))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
శూన్య
brelse(నిర్మాణం buf *b)
{
  ఒకవేళ (!holdingsleep(&b->తాళం))
    panic("brelse");

  releasesleep(&b->తాళం);

  acquire(&bcache.తాళం);
  b->refcnt--;
  ఒకవేళ (b->refcnt == 0) {
    // no one is waiting for it.
    b->తదుపరి->prev = b->prev;
    b->prev->తదుపరి = b->తదుపరి;
    b->తదుపరి = bcache.తల.తదుపరి;
    b->prev = &bcache.తల;
    bcache.తల.తదుపరి->prev = b;
    bcache.తల.తదుపరి = b;
  }

  release(&bcache.తాళం);
}

శూన్య
bpin(నిర్మాణం buf *b)
{
  acquire(&bcache.తాళం);
  b->refcnt++;
  release(&bcache.తాళం);
}

శూన్య
bunpin(నిర్మాణం buf *b)
{
  acquire(&bcache.తాళం);
  b->refcnt--;
  release(&bcache.తాళం);
}
