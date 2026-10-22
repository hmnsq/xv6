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

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "buf.h"

ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਬਣਤਰ buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  ਬਣਤਰ buf ਸਿਰ;
} bcache;

ਖਾਲੀ
binit(ਖਾਲੀ)
{
  ਬਣਤਰ buf *b;

  initlock(&bcache.ਤਾਲਾ, "bcache");

  // Create linked list of buffers
  bcache.ਸਿਰ.prev = &bcache.ਸਿਰ;
  bcache.ਸਿਰ.ਅਗਲਾ = &bcache.ਸਿਰ;
  ਲਈ (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->ਅਗਲਾ = bcache.ਸਿਰ.ਅਗਲਾ;
    b->prev = &bcache.ਸਿਰ;
    initsleeplock(&b->ਤਾਲਾ, "buffer");
    bcache.ਸਿਰ.ਅਗਲਾ->prev = b;
    bcache.ਸਿਰ.ਅਗਲਾ = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
ਸਥਿਰ ਬਣਤਰ buf *
bget(uint dev, uint blockno)
{
  ਬਣਤਰ buf *b;

  acquire(&bcache.ਤਾਲਾ);

  // Is the block already cached?
  ਲਈ (b = bcache.ਸਿਰ.ਅਗਲਾ; b != &bcache.ਸਿਰ; b = b->ਅਗਲਾ) {
    ਜੇ (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.ਤਾਲਾ);
      acquiresleep(&b->ਤਾਲਾ);
      ਵਾਪਸ b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  ਲਈ (b = bcache.ਸਿਰ.prev; b != &bcache.ਸਿਰ; b = b->prev) {
    ਜੇ (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.ਤਾਲਾ);
      acquiresleep(&b->ਤਾਲਾ);
      ਵਾਪਸ b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
ਬਣਤਰ buf *
bread(uint dev, uint blockno)
{
  ਬਣਤਰ buf *b;

  b = bget(dev, blockno);
  ਜੇ (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  ਵਾਪਸ b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
ਖਾਲੀ
bwrite(ਬਣਤਰ buf *b)
{
  ਜੇ (!holdingsleep(&b->ਤਾਲਾ))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
ਖਾਲੀ
brelse(ਬਣਤਰ buf *b)
{
  ਜੇ (!holdingsleep(&b->ਤਾਲਾ))
    panic("brelse");

  releasesleep(&b->ਤਾਲਾ);

  acquire(&bcache.ਤਾਲਾ);
  b->refcnt--;
  ਜੇ (b->refcnt == 0) {
    // no one is waiting for it.
    b->ਅਗਲਾ->prev = b->prev;
    b->prev->ਅਗਲਾ = b->ਅਗਲਾ;
    b->ਅਗਲਾ = bcache.ਸਿਰ.ਅਗਲਾ;
    b->prev = &bcache.ਸਿਰ;
    bcache.ਸਿਰ.ਅਗਲਾ->prev = b;
    bcache.ਸਿਰ.ਅਗਲਾ = b;
  }

  release(&bcache.ਤਾਲਾ);
}

ਖਾਲੀ
bpin(ਬਣਤਰ buf *b)
{
  acquire(&bcache.ਤਾਲਾ);
  b->refcnt++;
  release(&bcache.ਤਾਲਾ);
}

ਖਾਲੀ
bunpin(ਬਣਤਰ buf *b)
{
  acquire(&bcache.ਤਾਲਾ);
  b->refcnt--;
  release(&bcache.ਤਾਲਾ);
}
