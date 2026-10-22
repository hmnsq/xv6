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
  за (f = ftable.file; f < ftable.file + NFILE; f++) {
    ако (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      врати f;
    }
  }
  release(&ftable.lock);
  врати 0;
}

// Increment ref count for file f.
struct file *
filedup(struct file *f)
{
  acquire(&ftable.lock);
  ако (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  врати f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  ако (f->ref < 1)
    panic("fileclose");
  ако (--f->ref > 0) {
    release(&ftable.lock);
    врати;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ако (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } инако ако (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
цео
filestat(struct file *f, uint64 addr)
{
  struct proc *p = myproc();
  struct stat st;

  ако (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ако (copyout(p->pagetable, p->sz, addr, (char *)&st, sizeof(st)) < 0)
      врати -1;
    врати 0;
  }
  врати -1;
}

// Read from file f.
// addr is a user virtual address.
цео
fileread(struct file *f, uint64 addr, цео n)
{
  цео r = 0;

  ако (f->readable == 0 || n < 0)
    врати -1;

  ако (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } инако ако (f->type == FD_DEVICE) {
    ако (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      врати -1;
    r = devsw[f->major].read(1, addr, n);
  } инако ако (f->type == FD_INODE) {
    ilock(f->ip);
    ако ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } инако {
    panic("fileread");
  }

  врати r;
}

// Write to file f.
// addr is a user virtual address.
цео
filewrite(struct file *f, uint64 addr, цео n)
{
  цео r, ret = 0;

  ако (f->writable == 0 || n < 0)
    врати -1;

  ако (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } инако ако (f->type == FD_DEVICE) {
    ако (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      врати -1;
    ret = devsw[f->major].write(1, addr, n);
  } инако ако (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    цео max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    цео i = 0;
    док (i < n) {
      цео n1 = n - i;
      ако (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ако ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ако (r != n1) {
        // error from writei
        break;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } инако {
    panic("filewrite");
  }

  врати ret;
}
