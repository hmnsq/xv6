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
  for (f = ftable.file; f < ftable.file + NFILE; f++) {
    ᰀᰧ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ᰀᰧᰌ f;
    }
  }
  release(&ftable.lock);
  ᰀᰧᰌ 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  ᰀᰧ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ᰀᰧᰌ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  ᰀᰧ (f->ref < 1)
    panic("fileclose");
  ᰀᰧ (--f->ref > 0) {
    release(&ftable.lock);
    ᰀᰧᰌ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ᰀᰧ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ᰀᰧᰀ ᰀᰧ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
puur_naa_nka
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  ᰀᰧ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ᰀᰧ (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ᰀᰧᰌ -1;
    ᰀᰧᰌ 0;
  }
  ᰀᰧᰌ -1;
}

// Read from file f.
// addr is a user virtual address.
puur_naa_nka
fileread(sa_nracanaa file *f, uint64 addr, puur_naa_nka n)
{
  puur_naa_nka r = 0;

  ᰀᰧ (f->readable == 0 || n < 0)
    ᰀᰧᰌ -1;

  ᰀᰧ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ᰀᰧᰀ ᰀᰧ (f->type == FD_DEVICE) {
    ᰀᰧ (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ᰀᰧᰌ -1;
    r = devsw[f->major].read(1, addr, n);
  } ᰀᰧᰀ ᰀᰧ (f->type == FD_INODE) {
    ilock(f->ip);
    ᰀᰧ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ᰀᰧᰀ {
    panic("fileread");
  }

  ᰀᰧᰌ r;
}

// Write to file f.
// addr is a user virtual address.
puur_naa_nka
filewrite(sa_nracanaa file *f, uint64 addr, puur_naa_nka n)
{
  puur_naa_nka r, ret = 0;

  ᰀᰧ (f->writable == 0 || n < 0)
    ᰀᰧᰌ -1;

  ᰀᰧ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ᰀᰧᰀ ᰀᰧ (f->type == FD_DEVICE) {
    ᰀᰧ (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ᰀᰧᰌ -1;
    ret = devsw[f->major].write(1, addr, n);
  } ᰀᰧᰀ ᰀᰧ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    puur_naa_nka max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    puur_naa_nka i = 0;
    ᰀᰬ (i < n) {
      puur_naa_nka n1 = n - i;
      ᰀᰧ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ᰀᰧ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ᰀᰧ (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ᰀᰧᰀ {
    panic("filewrite");
  }

  ᰀᰧᰌ ret;
}
