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
  위해 (f = ftable.file; f < ftable.file + NFILE; f++) {
    만약 (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      반환 f;
    }
  }
  release(&ftable.lock);
  반환 0;
}

// Increment ref count for file f.
struct file *
filedup(struct file *f)
{
  acquire(&ftable.lock);
  만약 (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  반환 f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  만약 (f->ref < 1)
    panic("fileclose");
  만약 (--f->ref > 0) {
    release(&ftable.lock);
    반환;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  만약 (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } 아니면 만약 (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
정수
filestat(struct file *f, uint64 addr)
{
  struct proc *p = myproc();
  struct stat st;

  만약 (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    만약 (copyout(p->pagetable, p->sz, addr, (char *)&st, sizeof(st)) < 0)
      반환 -1;
    반환 0;
  }
  반환 -1;
}

// Read from file f.
// addr is a user virtual address.
정수
fileread(struct file *f, uint64 addr, 정수 n)
{
  정수 r = 0;

  만약 (f->readable == 0 || n < 0)
    반환 -1;

  만약 (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } 아니면 만약 (f->type == FD_DEVICE) {
    만약 (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      반환 -1;
    r = devsw[f->major].read(1, addr, n);
  } 아니면 만약 (f->type == FD_INODE) {
    ilock(f->ip);
    만약 ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } 아니면 {
    panic("fileread");
  }

  반환 r;
}

// Write to file f.
// addr is a user virtual address.
정수
filewrite(struct file *f, uint64 addr, 정수 n)
{
  정수 r, ret = 0;

  만약 (f->writable == 0 || n < 0)
    반환 -1;

  만약 (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } 아니면 만약 (f->type == FD_DEVICE) {
    만약 (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      반환 -1;
    ret = devsw[f->major].write(1, addr, n);
  } 아니면 만약 (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    정수 max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    정수 i = 0;
    동안 (i < n) {
      정수 n1 = n - i;
      만약 (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      만약 ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      만약 (r != n1) {
        // error from writei
        break;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } 아니면 {
    panic("filewrite");
  }

  반환 ret;
}
