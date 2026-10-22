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

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "buf.h"

ഘടന {
  ഘടന spinlock lock;
  ഘടന buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  ഘടന buf head;
} bcache;

ശൂന്യം
binit(ശൂന്യം)
{
  ഘടന buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  വേണ്ടി (b = bcache.buf; b < bcache.buf + NBUF; b++) {
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
സ്ഥിതം ഘടന buf *
bget(uint dev, uint blockno)
{
  ഘടന buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  വേണ്ടി (b = bcache.head.next; b != &bcache.head; b = b->next) {
    എങ്കിൽ (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      തിരികെ b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  വേണ്ടി (b = bcache.head.prev; b != &bcache.head; b = b->prev) {
    എങ്കിൽ (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      തിരികെ b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
ഘടന buf *
bread(uint dev, uint blockno)
{
  ഘടന buf *b;

  b = bget(dev, blockno);
  എങ്കിൽ (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  തിരികെ b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
ശൂന്യം
bwrite(ഘടന buf *b)
{
  എങ്കിൽ (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
ശൂന്യം
brelse(ഘടന buf *b)
{
  എങ്കിൽ (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  എങ്കിൽ (b->refcnt == 0) {
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

ശൂന്യം
bpin(ഘടന buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

ശൂന്യം
bunpin(ഘടന buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
