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

#समावेश "types.h"
#समावेश "param.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "fs.h"
#समावेश "buf.h"

काष्ठा {
  काष्ठा spinlock lock;
  काष्ठा buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  काष्ठा buf शीर्ष;
} bcache;

व्योम
binit(व्योम)
{
  काष्ठा buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.शीर्ष.पूर्व = &bcache.शीर्ष;
  bcache.शीर्ष.अग्र = &bcache.शीर्ष;
  क्रम (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->अग्र = bcache.शीर्ष.अग्र;
    b->पूर्व = &bcache.शीर्ष;
    initsleeplock(&b->lock, "buffer");
    bcache.शीर्ष.अग्र->पूर्व = b;
    bcache.शीर्ष.अग्र = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
अटल काष्ठा buf *
bget(uint dev, uint blockno)
{
  काष्ठा buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  क्रम (b = bcache.शीर्ष.अग्र; b != &bcache.शीर्ष; b = b->अग्र) {
    अगर (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      वापस b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  क्रम (b = bcache.शीर्ष.पूर्व; b != &bcache.शीर्ष; b = b->पूर्व) {
    अगर (b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      वापस b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
काष्ठा buf *
bread(uint dev, uint blockno)
{
  काष्ठा buf *b;

  b = bget(dev, blockno);
  अगर (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  वापस b;
}

// Write b's contents to disk.  Must be locked.
// Only the log calls bwrite.
व्योम
bwrite(काष्ठा buf *b)
{
  अगर (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
व्योम
brelse(काष्ठा buf *b)
{
  अगर (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  अगर (b->refcnt == 0) {
    // no one is waiting for it.
    b->अग्र->पूर्व = b->पूर्व;
    b->पूर्व->अग्र = b->अग्र;
    b->अग्र = bcache.शीर्ष.अग्र;
    b->पूर्व = &bcache.शीर्ष;
    bcache.शीर्ष.अग्र->पूर्व = b;
    bcache.शीर्ष.अग्र = b;
  }

  release(&bcache.lock);
}

व्योम
bpin(काष्ठा buf *b)
{
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

व्योम
bunpin(काष्ठा buf *b)
{
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}
