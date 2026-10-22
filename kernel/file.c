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
  cho (f = ftable.file; f < ftable.file + NFILE; f++) {
    nếu (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      trả_về f;
    }
  }
  release(&ftable.lock);
  trả_về 0;
}

// Increment ref count for file f.
struct file *
filedup(struct file *f)
{
  acquire(&ftable.lock);
  nếu (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  trả_về f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  nếu (f->ref < 1)
    panic("fileclose");
  nếu (--f->ref > 0) {
    release(&ftable.lock);
    trả_về;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  nếu (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } không_thì nếu (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
số_nguyên
filestat(struct file *f, uint64 addr)
{
  struct proc *p = myproc();
  struct stat st;

  nếu (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    nếu (copyout(p->pagetable, p->sz, addr, (char *)&st, sizeof(st)) < 0)
      trả_về -1;
    trả_về 0;
  }
  trả_về -1;
}

// Read from file f.
// addr is a user virtual address.
số_nguyên
fileread(struct file *f, uint64 addr, số_nguyên n)
{
  số_nguyên r = 0;

  nếu (f->readable == 0 || n < 0)
    trả_về -1;

  nếu (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } không_thì nếu (f->type == FD_DEVICE) {
    nếu (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      trả_về -1;
    r = devsw[f->major].read(1, addr, n);
  } không_thì nếu (f->type == FD_INODE) {
    ilock(f->ip);
    nếu ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } không_thì {
    panic("fileread");
  }

  trả_về r;
}

// Write to file f.
// addr is a user virtual address.
số_nguyên
filewrite(struct file *f, uint64 addr, số_nguyên n)
{
  số_nguyên r, ret = 0;

  nếu (f->writable == 0 || n < 0)
    trả_về -1;

  nếu (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } không_thì nếu (f->type == FD_DEVICE) {
    nếu (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      trả_về -1;
    ret = devsw[f->major].write(1, addr, n);
  } không_thì nếu (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    số_nguyên max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    số_nguyên i = 0;
    trong_khi (i < n) {
      số_nguyên n1 = n - i;
      nếu (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      nếu ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      nếu (r != n1) {
        // error from writei
        break;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } không_thì {
    panic("filewrite");
  }

  trả_về ret;
}
