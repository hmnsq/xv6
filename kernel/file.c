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

sa_nracanaa devsw devsw[NDEV];
sa_nracanaa {
  sa_nracanaa spinlock lock;
  sa_nracanaa file file[NFILE];
} ftable;

void
fileinit(void)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
sa_nracanaa file *
filealloc(void)
{
  sa_nracanaa file *f;

  acquire(&ftable.lock);
  សម្រាប់ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ប្រសិនបើ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ត្រឡប់ f;
    }
  }
  release(&ftable.lock);
  ត្រឡប់ 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  ប្រសិនបើ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ត្រឡប់ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  ប្រសិនបើ (f->ref < 1)
    panic("fileclose");
  ប្រសិនបើ (--f->ref > 0) {
    release(&ftable.lock);
    ត្រឡប់;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ប្រសិនបើ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } បើមិនដូច្នេះ ប្រសិនបើ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ចំនួនគត់
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  ប្រសិនបើ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ប្រសិនបើ (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ត្រឡប់ -1;
    ត្រឡប់ 0;
  }
  ត្រឡប់ -1;
}

// Read from file f.
// addr is a user virtual address.
ចំនួនគត់
fileread(sa_nracanaa file *f, uint64 addr, ចំនួនគត់ n)
{
  ចំនួនគត់ r = 0;

  ប្រសិនបើ (f->readable == 0 || n < 0)
    ត្រឡប់ -1;

  ប្រសិនបើ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } បើមិនដូច្នេះ ប្រសិនបើ (f->type == FD_DEVICE) {
    ប្រសិនបើ (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ត្រឡប់ -1;
    r = devsw[f->major].read(1, addr, n);
  } បើមិនដូច្នេះ ប្រសិនបើ (f->type == FD_INODE) {
    ilock(f->ip);
    ប្រសិនបើ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } បើមិនដូច្នេះ {
    panic("fileread");
  }

  ត្រឡប់ r;
}

// Write to file f.
// addr is a user virtual address.
ចំនួនគត់
filewrite(sa_nracanaa file *f, uint64 addr, ចំនួនគត់ n)
{
  ចំនួនគត់ r, ret = 0;

  ប្រសិនបើ (f->writable == 0 || n < 0)
    ត្រឡប់ -1;

  ប្រសិនបើ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } បើមិនដូច្នេះ ប្រសិនបើ (f->type == FD_DEVICE) {
    ប្រសិនបើ (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ត្រឡប់ -1;
    ret = devsw[f->major].write(1, addr, n);
  } បើមិនដូច្នេះ ប្រសិនបើ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ចំនួនគត់ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ចំនួនគត់ i = 0;
    ខណៈពេល (i < n) {
      ចំនួនគត់ n1 = n - i;
      ប្រសិនបើ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ប្រសិនបើ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ប្រសិនបើ (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } បើមិនដូច្នេះ {
    panic("filewrite");
  }

  ត្រឡប់ ret;
}
