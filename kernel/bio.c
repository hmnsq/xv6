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
  بنية spinlock قلف;
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

  initlock(&bcache.قلف, "bcache");

  // Create linked list of buffers
  bcache.سر.prev = &bcache.سر;
  bcache.سر.بل = &bcache.سر;
  لكل (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->بل = bcache.سر.بل;
    b->prev = &bcache.سر;
    initsleeplock(&b->قلف, "buffer");
    bcache.سر.بل->prev = b;
    bcache.سر.بل = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
ساكن بنية buf *
bget(uint dev, uint blockno)
{
  بنية buf *b;

  acquire(&bcache.قلف);

  // Is the block already cached?
  لكل (b = bcache.سر.بل; b != &bcache.سر; b = b->بل) {
    إذا (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.قلف);
      acquiresleep(&b->قلف);
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
      release(&bcache.قلف);
      acquiresleep(&b->قلف);
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
  إذا (!holdingsleep(&b->قلف))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
فراغ
brelse(بنية buf *b)
{
  إذا (!holdingsleep(&b->قلف))
    panic("brelse");

  releasesleep(&b->قلف);

  acquire(&bcache.قلف);
  b->refcnt--;
  إذا (b->refcnt == 0) {
    // no one is waiting for it.
    b->بل->prev = b->prev;
    b->prev->بل = b->بل;
    b->بل = bcache.سر.بل;
    b->prev = &bcache.سر;
    bcache.سر.بل->prev = b;
    bcache.سر.بل = b;
  }

  release(&bcache.قلف);
}

فراغ
bpin(بنية buf *b)
{
  acquire(&bcache.قلف);
  b->refcnt++;
  release(&bcache.قلف);
}

فراغ
bunpin(بنية buf *b)
{
  acquire(&bcache.قلف);
  b->refcnt--;
  release(&bcache.قلف);
}
