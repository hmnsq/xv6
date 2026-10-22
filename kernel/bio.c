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

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "buf.h"

கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;
  கட்டமைப்பு buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  கட்டமைப்பு buf head;
} bcache;

வெறுமை
binit(வெறுமை)
{
  கட்டமைப்பு buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  க்கு (b = bcache.buf; b < bcache.buf + NBUF; b++) {
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
நிலையான கட்டமைப்பு buf *
bget(uint dev, uint blockno)
{
  கட்டமைப்பு buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  க்கு (b = bcache.head.next; b != &bcache.head; b = b->next) {
    எனில் (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      திரும்பு b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  க்கு (b = bcache.head.prev; b != &bcache.head; b = b->prev) {
    எனில் (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      திரும்பு b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
கட்டமைப்பு buf *
bread(uint dev, uint blockno)
{
  கட்டமைப்பு buf *b;

  b = bget(dev, blockno);
  எனில் (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  திரும்பு b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
வெறுமை
bwrite(கட்டமைப்பு buf *b)
{
  எனில் (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
வெறுமை
brelse(கட்டமைப்பு buf *b)
{
  எனில் (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  எனில் (b->refcnt == 0) {
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

வெறுமை
bpin(கட்டமைப்பு buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

வெறுமை
bunpin(கட்டமைப்பு buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
