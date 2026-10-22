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
  สำหรับ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ถ้า (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      คืน f;
    }
  }
  release(&ftable.lock);
  คืน 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  ถ้า (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  คืน f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  ถ้า (f->ref < 1)
    panic("fileclose");
  ถ้า (--f->ref > 0) {
    release(&ftable.lock);
    คืน;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ถ้า (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } มิฉะนั้น ถ้า (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
จำนวนเต็ม
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  ถ้า (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ถ้า (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      คืน -1;
    คืน 0;
  }
  คืน -1;
}

// Read from file f.
// addr is a user virtual address.
จำนวนเต็ม
fileread(sa_nracanaa file *f, uint64 addr, จำนวนเต็ม n)
{
  จำนวนเต็ม r = 0;

  ถ้า (f->readable == 0 || n < 0)
    คืน -1;

  ถ้า (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } มิฉะนั้น ถ้า (f->type == FD_DEVICE) {
    ถ้า (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      คืน -1;
    r = devsw[f->major].read(1, addr, n);
  } มิฉะนั้น ถ้า (f->type == FD_INODE) {
    ilock(f->ip);
    ถ้า ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } มิฉะนั้น {
    panic("fileread");
  }

  คืน r;
}

// Write to file f.
// addr is a user virtual address.
จำนวนเต็ม
filewrite(sa_nracanaa file *f, uint64 addr, จำนวนเต็ม n)
{
  จำนวนเต็ม r, ret = 0;

  ถ้า (f->writable == 0 || n < 0)
    คืน -1;

  ถ้า (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } มิฉะนั้น ถ้า (f->type == FD_DEVICE) {
    ถ้า (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      คืน -1;
    ret = devsw[f->major].write(1, addr, n);
  } มิฉะนั้น ถ้า (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    จำนวนเต็ม max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    จำนวนเต็ม i = 0;
    ขณะ (i < n) {
      จำนวนเต็ม n1 = n - i;
      ถ้า (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ถ้า ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ถ้า (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } มิฉะนั้น {
    panic("filewrite");
  }

  คืน ret;
}
