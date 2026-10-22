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

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "buf.h"

સંરચના {
  સંરચના spinlock lock;
  સંરચના buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  સંરચના buf head;
} bcache;

શૂન્ય
binit(શૂન્ય)
{
  સંરચના buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  માટે (b = bcache.buf; b < bcache.buf + NBUF; b++) {
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
સ્થિર સંરચના buf *
bget(uint dev, uint blockno)
{
  સંરચના buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  માટે (b = bcache.head.next; b != &bcache.head; b = b->next) {
    જો (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      પાછા b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  માટે (b = bcache.head.prev; b != &bcache.head; b = b->prev) {
    જો (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      પાછા b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
સંરચના buf *
bread(uint dev, uint blockno)
{
  સંરચના buf *b;

  b = bget(dev, blockno);
  જો (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  પાછા b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
શૂન્ય
bwrite(સંરચના buf *b)
{
  જો (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
શૂન્ય
brelse(સંરચના buf *b)
{
  જો (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  જો (b->refcnt == 0) {
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

શૂન્ય
bpin(સંરચના buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

શૂન્ય
bunpin(સંરચના buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
