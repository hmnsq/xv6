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
  ສຳລັບ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ຖ້າ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ສົ່ງຄືນ f;
    }
  }
  release(&ftable.lock);
  ສົ່ງຄືນ 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  ຖ້າ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ສົ່ງຄືນ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  ຖ້າ (f->ref < 1)
    panic("fileclose");
  ຖ້າ (--f->ref > 0) {
    release(&ftable.lock);
    ສົ່ງຄືນ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ຖ້າ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ຈຳນວນເຕັມ
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  ຖ້າ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ຖ້າ (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ສົ່ງຄືນ -1;
    ສົ່ງຄືນ 0;
  }
  ສົ່ງຄືນ -1;
}

// Read from file f.
// addr is a user virtual address.
ຈຳນວນເຕັມ
fileread(sa_nracanaa file *f, uint64 addr, ຈຳນວນເຕັມ n)
{
  ຈຳນວນເຕັມ r = 0;

  ຖ້າ (f->readable == 0 || n < 0)
    ສົ່ງຄືນ -1;

  ຖ້າ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (f->type == FD_DEVICE) {
    ຖ້າ (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ສົ່ງຄືນ -1;
    r = devsw[f->major].read(1, addr, n);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (f->type == FD_INODE) {
    ilock(f->ip);
    ຖ້າ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ບໍ່ດັ່ງນັ້ນ {
    panic("fileread");
  }

  ສົ່ງຄືນ r;
}

// Write to file f.
// addr is a user virtual address.
ຈຳນວນເຕັມ
filewrite(sa_nracanaa file *f, uint64 addr, ຈຳນວນເຕັມ n)
{
  ຈຳນວນເຕັມ r, ret = 0;

  ຖ້າ (f->writable == 0 || n < 0)
    ສົ່ງຄືນ -1;

  ຖ້າ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (f->type == FD_DEVICE) {
    ຖ້າ (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ສົ່ງຄືນ -1;
    ret = devsw[f->major].write(1, addr, n);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ຈຳນວນເຕັມ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ຈຳນວນເຕັມ i = 0;
    ໃນຂະນະທີ່ (i < n) {
      ຈຳນວນເຕັມ n1 = n - i;
      ຖ້າ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ຖ້າ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ຖ້າ (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ບໍ່ດັ່ງນັ້ນ {
    panic("filewrite");
  }

  ສົ່ງຄືນ ret;
}
