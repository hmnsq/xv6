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
  အတွက် (f = ftable.file; f < ftable.file + NFILE; f++) {
    အကယ်၍ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ပြန်ပေး f;
    }
  }
  release(&ftable.lock);
  ပြန်ပေး 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  အကယ်၍ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ပြန်ပေး f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  အကယ်၍ (f->ref < 1)
    panic("fileclose");
  အကယ်၍ (--f->ref > 0) {
    release(&ftable.lock);
    ပြန်ပေး;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  အကယ်၍ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } မဟုတ်ပါက အကယ်၍ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ကိန်းပြည့်
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  အကယ်၍ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    အကယ်၍ (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ပြန်ပေး -1;
    ပြန်ပေး 0;
  }
  ပြန်ပေး -1;
}

// Read from file f.
// addr is a user virtual address.
ကိန်းပြည့်
fileread(sa_nracanaa file *f, uint64 addr, ကိန်းပြည့် n)
{
  ကိန်းပြည့် r = 0;

  အကယ်၍ (f->readable == 0 || n < 0)
    ပြန်ပေး -1;

  အကယ်၍ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } မဟုတ်ပါက အကယ်၍ (f->type == FD_DEVICE) {
    အကယ်၍ (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ပြန်ပေး -1;
    r = devsw[f->major].read(1, addr, n);
  } မဟုတ်ပါက အကယ်၍ (f->type == FD_INODE) {
    ilock(f->ip);
    အကယ်၍ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } မဟုတ်ပါက {
    panic("fileread");
  }

  ပြန်ပေး r;
}

// Write to file f.
// addr is a user virtual address.
ကိန်းပြည့်
filewrite(sa_nracanaa file *f, uint64 addr, ကိန်းပြည့် n)
{
  ကိန်းပြည့် r, ret = 0;

  အကယ်၍ (f->writable == 0 || n < 0)
    ပြန်ပေး -1;

  အကယ်၍ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } မဟုတ်ပါက အကယ်၍ (f->type == FD_DEVICE) {
    အကယ်၍ (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ပြန်ပေး -1;
    ret = devsw[f->major].write(1, addr, n);
  } မဟုတ်ပါက အကယ်၍ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ကိန်းပြည့် max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ကိန်းပြည့် i = 0;
    နေစဉ် (i < n) {
      ကိန်းပြည့် n1 = n - i;
      အကယ်၍ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      အကယ်၍ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      အကယ်၍ (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } မဟုတ်ပါက {
    panic("filewrite");
  }

  ပြန်ပေး ret;
}
