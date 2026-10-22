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
  ために (f = ftable.file; f < ftable.file + NFILE; f++) {
    もし (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      戻る f;
    }
  }
  release(&ftable.lock);
  戻る 0;
}

// Increment ref count for file f.
struct file *
filedup(struct file *f)
{
  acquire(&ftable.lock);
  もし (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  戻る f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  もし (f->ref < 1)
    panic("fileclose");
  もし (--f->ref > 0) {
    release(&ftable.lock);
    戻る;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  もし (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } 否则 もし (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
整数
filestat(struct file *f, uint64 addr)
{
  struct proc *p = myproc();
  struct stat st;

  もし (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    もし (copyout(p->pagetable, p->sz, addr, (char *)&st, sizeof(st)) < 0)
      戻る -1;
    戻る 0;
  }
  戻る -1;
}

// Read from file f.
// addr is a user virtual address.
整数
fileread(struct file *f, uint64 addr, 整数 n)
{
  整数 r = 0;

  もし (f->readable == 0 || n < 0)
    戻る -1;

  もし (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } 否则 もし (f->type == FD_DEVICE) {
    もし (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      戻る -1;
    r = devsw[f->major].read(1, addr, n);
  } 否则 もし (f->type == FD_INODE) {
    ilock(f->ip);
    もし ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } 否则 {
    panic("fileread");
  }

  戻る r;
}

// Write to file f.
// addr is a user virtual address.
整数
filewrite(struct file *f, uint64 addr, 整数 n)
{
  整数 r, ret = 0;

  もし (f->writable == 0 || n < 0)
    戻る -1;

  もし (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } 否则 もし (f->type == FD_DEVICE) {
    もし (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      戻る -1;
    ret = devsw[f->major].write(1, addr, n);
  } 否则 もし (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    整数 max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    整数 i = 0;
    ながら (i < n) {
      整数 n1 = n - i;
      もし (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      もし ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      もし (r != n1) {
        // error from writei
        break;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } 否则 {
    panic("filewrite");
  }

  戻る ret;
}
