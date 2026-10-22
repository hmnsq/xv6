//
// Support functions for system calls that involve file descriptors.
//

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"

ਬਣਤਰ devsw devsw[NDEV];
ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਬਣਤਰ file file[NFILE];
} ftable;

ਖਾਲੀ
fileinit(ਖਾਲੀ)
{
  initlock(&ftable.ਤਾਲਾ, "ftable");
}

// Allocate a file structure.
ਬਣਤਰ file *
filealloc(ਖਾਲੀ)
{
  ਬਣਤਰ file *f;

  acquire(&ftable.ਤਾਲਾ);
  ਲਈ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ਜੇ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.ਤਾਲਾ);
      ਵਾਪਸ f;
    }
  }
  release(&ftable.ਤਾਲਾ);
  ਵਾਪਸ 0;
}

// Increment ref count for file f.
ਬਣਤਰ file *
filedup(ਬਣਤਰ file *f)
{
  acquire(&ftable.ਤਾਲਾ);
  ਜੇ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.ਤਾਲਾ);
  ਵਾਪਸ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
ਖਾਲੀ
fileclose(ਬਣਤਰ file *f)
{
  ਬਣਤਰ file ff;

  acquire(&ftable.ਤਾਲਾ);
  ਜੇ (f->ref < 1)
    panic("fileclose");
  ਜੇ (--f->ref > 0) {
    release(&ftable.ਤਾਲਾ);
    ਵਾਪਸ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.ਤਾਲਾ);

  ਜੇ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ਨਹੀਂਤਾਂ ਜੇ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    ਸਿਰਾ_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ਪੂਰਨ
filestat(ਬਣਤਰ file *f, uint64 addr)
{
  ਬਣਤਰ proc *p = myproc();
  ਬਣਤਰ stat st;

  ਜੇ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ਜੇ (copyout(p->pagetable, p->sz, addr, (ਅੱਖਰ *)&st, ਆਕਾਰ(st)) < 0)
      ਵਾਪਸ -1;
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ -1;
}

// Read from file f.
// addr is a user virtual address.
ਪੂਰਨ
fileread(ਬਣਤਰ file *f, uint64 addr, ਪੂਰਨ n)
{
  ਪੂਰਨ r = 0;

  ਜੇ (f->readable == 0 || n < 0)
    ਵਾਪਸ -1;

  ਜੇ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ਨਹੀਂਤਾਂ ਜੇ (f->type == FD_DEVICE) {
    ਜੇ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ਪਠ)
      ਵਾਪਸ -1;
    r = devsw[f->major].ਪਠ(1, addr, n);
  } ਨਹੀਂਤਾਂ ਜੇ (f->type == FD_INODE) {
    ilock(f->ip);
    ਜੇ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ਨਹੀਂਤਾਂ {
    panic("fileread");
  }

  ਵਾਪਸ r;
}

// Write to file f.
// addr is a user virtual address.
ਪੂਰਨ
filewrite(ਬਣਤਰ file *f, uint64 addr, ਪੂਰਨ n)
{
  ਪੂਰਨ r, ret = 0;

  ਜੇ (f->writable == 0 || n < 0)
    ਵਾਪਸ -1;

  ਜੇ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ਨਹੀਂਤਾਂ ਜੇ (f->type == FD_DEVICE) {
    ਜੇ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ਲਿਖ)
      ਵਾਪਸ -1;
    ret = devsw[f->major].ਲਿਖ(1, addr, n);
  } ਨਹੀਂਤਾਂ ਜੇ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ਪੂਰਨ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ਪੂਰਨ i = 0;
    ਜਦੋਂ (i < n) {
      ਪੂਰਨ n1 = n - i;
      ਜੇ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ਜੇ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      ਸਿਰਾ_op();

      ਜੇ (r != n1) {
        // error from writei
        ਰੋਕੋ;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ਨਹੀਂਤਾਂ {
    panic("filewrite");
  }

  ਵਾਪਸ ret;
}
