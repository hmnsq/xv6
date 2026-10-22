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

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "buf.h"

ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;
  ರಚನೆ buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  ರಚನೆ buf ತಲೆ;
} bcache;

ಶೂನ್ಯ
binit(ಶೂನ್ಯ)
{
  ರಚನೆ buf *b;

  initlock(&bcache.ಬೀಗ, "bcache");

  // Create linked list of buffers
  bcache.ತಲೆ.prev = &bcache.ತಲೆ;
  bcache.ತಲೆ.ಮುಂದಿನ = &bcache.ತಲೆ;
  ಗಾಗಿ (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->ಮುಂದಿನ = bcache.ತಲೆ.ಮುಂದಿನ;
    b->prev = &bcache.ತಲೆ;
    initsleeplock(&b->ಬೀಗ, "buffer");
    bcache.ತಲೆ.ಮುಂದಿನ->prev = b;
    bcache.ತಲೆ.ಮುಂದಿನ = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
ಸ್ಥಿರ ರಚನೆ buf *
bget(uint dev, uint blockno)
{
  ರಚನೆ buf *b;

  acquire(&bcache.ಬೀಗ);

  // Is the block already cached?
  ಗಾಗಿ (b = bcache.ತಲೆ.ಮುಂದಿನ; b != &bcache.ತಲೆ; b = b->ಮುಂದಿನ) {
    ಆದರೆ (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.ಬೀಗ);
      acquiresleep(&b->ಬೀಗ);
      ಹಿಂತಿರುಗಿ b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  ಗಾಗಿ (b = bcache.ತಲೆ.prev; b != &bcache.ತಲೆ; b = b->prev) {
    ಆದರೆ (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.ಬೀಗ);
      acquiresleep(&b->ಬೀಗ);
      ಹಿಂತಿರುಗಿ b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
ರಚನೆ buf *
bread(uint dev, uint blockno)
{
  ರಚನೆ buf *b;

  b = bget(dev, blockno);
  ಆದರೆ (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  ಹಿಂತಿರುಗಿ b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
ಶೂನ್ಯ
bwrite(ರಚನೆ buf *b)
{
  ಆದರೆ (!holdingsleep(&b->ಬೀಗ))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
ಶೂನ್ಯ
brelse(ರಚನೆ buf *b)
{
  ಆದರೆ (!holdingsleep(&b->ಬೀಗ))
    panic("brelse");

  releasesleep(&b->ಬೀಗ);

  acquire(&bcache.ಬೀಗ);
  b->refcnt--;
  ಆದರೆ (b->refcnt == 0) {
    // no one is waiting for it.
    b->ಮುಂದಿನ->prev = b->prev;
    b->prev->ಮುಂದಿನ = b->ಮುಂದಿನ;
    b->ಮುಂದಿನ = bcache.ತಲೆ.ಮುಂದಿನ;
    b->prev = &bcache.ತಲೆ;
    bcache.ತಲೆ.ಮುಂದಿನ->prev = b;
    bcache.ತಲೆ.ಮುಂದಿನ = b;
  }

  release(&bcache.ಬೀಗ);
}

ಶೂನ್ಯ
bpin(ರಚನೆ buf *b)
{
  acquire(&bcache.ಬೀಗ);
  b->refcnt++;
  release(&bcache.ಬೀಗ);
}

ಶೂನ್ಯ
bunpin(ರಚನೆ buf *b)
{
  acquire(&bcache.ಬೀಗ);
  b->refcnt--;
  release(&bcache.ಬೀಗ);
}
