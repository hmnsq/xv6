//
// Support functions for system calls that involve file descriptors.
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"

struct devsw devsw[NDEV];
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;

void
fileinit(void)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
struct file *
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  для (f = ftable.file; f < ftable.file + NFILE; f++) {
    якщо (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      повернути f;
    }
  }
  release(&ftable.lock);
  повернути 0;
}

// Increment ref count for file f.
struct file *
filedup(struct file *f)
{
  acquire(&ftable.lock);
  якщо (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  повернути f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  якщо (f->ref < 1)
    panic("fileclose");
  якщо (--f->ref > 0) {
    release(&ftable.lock);
    повернути;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  якщо (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } інакше якщо (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ціле
filestat(struct file *f, uint64 addr)
{
  struct proc *p = myproc();
  struct stat st;

  якщо (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    якщо (copyout(p->pagetable, p->sz, addr, (char *)&st, sizeof(st)) < 0)
      повернути -1;
    повернути 0;
  }
  повернути -1;
}

// Read from file f.
// addr is a user virtual address.
ціле
fileread(struct file *f, uint64 addr, ціле n)
{
  ціле r = 0;

  якщо (f->readable == 0 || n < 0)
    повернути -1;

  якщо (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } інакше якщо (f->type == FD_DEVICE) {
    якщо (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      повернути -1;
    r = devsw[f->major].read(1, addr, n);
  } інакше якщо (f->type == FD_INODE) {
    ilock(f->ip);
    якщо ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } інакше {
    panic("fileread");
  }

  повернути r;
}

// Write to file f.
// addr is a user virtual address.
ціле
filewrite(struct file *f, uint64 addr, ціле n)
{
  ціле r, ret = 0;

  якщо (f->writable == 0 || n < 0)
    повернути -1;

  якщо (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } інакше якщо (f->type == FD_DEVICE) {
    якщо (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      повернути -1;
    ret = devsw[f->major].write(1, addr, n);
  } інакше якщо (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ціле max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ціле i = 0;
    поки (i < n) {
      ціле n1 = n - i;
      якщо (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      якщо ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      якщо (r != n1) {
        // error from writei
        break;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } інакше {
    panic("filewrite");
  }

  повернути ret;
}
