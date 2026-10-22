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

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "buf.h"

কাঠামো {
  কাঠামো spinlock lock;
  কাঠামো buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  কাঠামো buf শীর্ষ;
} bcache;

শূন্য
binit(শূন্য)
{
  কাঠামো buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.শীর্ষ.পূর্ব = &bcache.শীর্ষ;
  bcache.শীর্ষ.পরবর্তী = &bcache.শীর্ষ;
  জন্য (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->পরবর্তী = bcache.শীর্ষ.পরবর্তী;
    b->পূর্ব = &bcache.শীর্ষ;
    initsleeplock(&b->lock, "buffer");
    bcache.শীর্ষ.পরবর্তী->পূর্ব = b;
    bcache.শীর্ষ.পরবর্তী = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
স্থির কাঠামো buf *
bget(uint dev, uint blockno)
{
  কাঠামো buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  জন্য (b = bcache.শীর্ষ.পরবর্তী; b != &bcache.শীর্ষ; b = b->পরবর্তী) {
    যদি (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      ফেরত b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  জন্য (b = bcache.শীর্ষ.পূর্ব; b != &bcache.শীর্ষ; b = b->পূর্ব) {
    যদি (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      ফেরত b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
কাঠামো buf *
bread(uint dev, uint blockno)
{
  কাঠামো buf *b;

  b = bget(dev, blockno);
  যদি (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  ফেরত b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
শূন্য
bwrite(কাঠামো buf *b)
{
  যদি (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
শূন্য
brelse(কাঠামো buf *b)
{
  যদি (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  যদি (b->refcnt == 0) {
    // no one is waiting for it.
    b->পরবর্তী->পূর্ব = b->পূর্ব;
    b->পূর্ব->পরবর্তী = b->পরবর্তী;
    b->পরবর্তী = bcache.শীর্ষ.পরবর্তী;
    b->পূর্ব = &bcache.শীর্ষ;
    bcache.শীর্ষ.পরবর্তী->পূর্ব = b;
    bcache.শীর্ষ.পরবর্তী = b;
  }

  release(&bcache.lock);
}

শূন্য
bpin(কাঠামো buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

শূন্য
bunpin(কাঠামো buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
