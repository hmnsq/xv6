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

#ضمن "types.h"
#ضمن "param.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "fs.h"
#ضمن "buf.h"

بنية {
  بنية spinlock جندرا;
  بنية buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  بنية buf سر;
} bcache;

فراغ
binit(فراغ)
{
  بنية buf *b;

  initlock(&bcache.جندرا, "bcache");

  // Create linked list of buffers
  bcache.سر.prev = &bcache.سر;
  bcache.سر.اگلا = &bcache.سر;
  لكل (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->اگلا = bcache.سر.اگلا;
    b->prev = &bcache.سر;
    initsleeplock(&b->جندرا, "buffer");
    bcache.سر.اگلا->prev = b;
    bcache.سر.اگلا = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
ساكن بنية buf *
bget(uint dev, uint blockno)
{
  بنية buf *b;

  acquire(&bcache.جندرا);

  // Is the block already cached?
  لكل (b = bcache.سر.اگلا; b != &bcache.سر; b = b->اگلا) {
    إذا (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.جندرا);
      acquiresleep(&b->جندرا);
      أرجع b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  لكل (b = bcache.سر.prev; b != &bcache.سر; b = b->prev) {
    إذا (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.جندرا);
      acquiresleep(&b->جندرا);
      أرجع b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
بنية buf *
bread(uint dev, uint blockno)
{
  بنية buf *b;

  b = bget(dev, blockno);
  إذا (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  أرجع b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
فراغ
bwrite(بنية buf *b)
{
  إذا (!holdingsleep(&b->جندرا))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
فراغ
brelse(بنية buf *b)
{
  إذا (!holdingsleep(&b->جندرا))
    panic("brelse");

  releasesleep(&b->جندرا);

  acquire(&bcache.جندرا);
  b->refcnt--;
  إذا (b->refcnt == 0) {
    // no one is waiting for it.
    b->اگلا->prev = b->prev;
    b->prev->اگلا = b->اگلا;
    b->اگلا = bcache.سر.اگلا;
    b->prev = &bcache.سر;
    bcache.سر.اگلا->prev = b;
    bcache.سر.اگلا = b;
  }

  release(&bcache.جندرا);
}

فراغ
bpin(بنية buf *b)
{
  acquire(&bcache.جندرا);
  b->refcnt++;
  release(&bcache.جندرا);
}

فراغ
bunpin(بنية buf *b)
{
  acquire(&bcache.جندرا);
  b->refcnt--;
  release(&bcache.جندرا);
}
