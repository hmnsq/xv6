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

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "buf.h"

संरचना {
  संरचना spinlock lock;
  संरचना buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  संरचना buf head;
} bcache;

शून्य
binit(शून्य)
{
  संरचना buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  प्रति (b = bcache.buf; b < bcache.buf + NBUF; b++) {
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
स्थिर संरचना buf *
bget(uint dev, uint blockno)
{
  संरचना buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  प्रति (b = bcache.head.next; b != &bcache.head; b = b->next) {
    जइ (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      प्रत्यागम b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  प्रति (b = bcache.head.prev; b != &bcache.head; b = b->prev) {
    जइ (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      प्रत्यागम b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
संरचना buf *
bread(uint dev, uint blockno)
{
  संरचना buf *b;

  b = bget(dev, blockno);
  जइ (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  प्रत्यागम b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
शून्य
bwrite(संरचना buf *b)
{
  जइ (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
शून्य
brelse(संरचना buf *b)
{
  जइ (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  जइ (b->refcnt == 0) {
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

शून्य
bpin(संरचना buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

शून्य
bunpin(संरचना buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
