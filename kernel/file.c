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
  සඳහා (f = ftable.file; f < ftable.file + NFILE; f++) {
    නම් (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ආපසු f;
    }
  }
  release(&ftable.lock);
  ආපසු 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  නම් (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ආපසු f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  නම් (f->ref < 1)
    panic("fileclose");
  නම් (--f->ref > 0) {
    release(&ftable.lock);
    ආපසු;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  නම් (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } නැතහොත් නම් (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
පූර්ණ_සංඛ්‍යා
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  නම් (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    නම් (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ආපසු -1;
    ආපසු 0;
  }
  ආපසු -1;
}

// Read from file f.
// addr is a user virtual address.
පූර්ණ_සංඛ්‍යා
fileread(sa_nracanaa file *f, uint64 addr, පූර්ණ_සංඛ්‍යා n)
{
  පූර්ණ_සංඛ්‍යා r = 0;

  නම් (f->readable == 0 || n < 0)
    ආපසු -1;

  නම් (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } නැතහොත් නම් (f->type == FD_DEVICE) {
    නම් (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ආපසු -1;
    r = devsw[f->major].read(1, addr, n);
  } නැතහොත් නම් (f->type == FD_INODE) {
    ilock(f->ip);
    නම් ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } නැතහොත් {
    panic("fileread");
  }

  ආපසු r;
}

// Write to file f.
// addr is a user virtual address.
පූර්ණ_සංඛ්‍යා
filewrite(sa_nracanaa file *f, uint64 addr, පූර්ණ_සංඛ්‍යා n)
{
  පූර්ණ_සංඛ්‍යා r, ret = 0;

  නම් (f->writable == 0 || n < 0)
    ආපසු -1;

  නම් (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } නැතහොත් නම් (f->type == FD_DEVICE) {
    නම් (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ආපසු -1;
    ret = devsw[f->major].write(1, addr, n);
  } නැතහොත් නම් (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    පූර්ණ_සංඛ්‍යා max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    පූර්ණ_සංඛ්‍යා i = 0;
    අතරතුර (i < n) {
      පූර්ණ_සංඛ්‍යා n1 = n - i;
      නම් (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      නම් ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      නම් (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } නැතහොත් {
    panic("filewrite");
  }

  ආපසු ret;
}
