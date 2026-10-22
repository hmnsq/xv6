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
  དོན་ལུ (f = ftable.file; f < ftable.file + NFILE; f++) {
    གལ་སྲིད (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ལོག f;
    }
  }
  release(&ftable.lock);
  ལོག 0;
}

// Increment ref count for file f.
sa_nracanaa file *
filedup(sa_nracanaa file *f)
{
  acquire(&ftable.lock);
  གལ་སྲིད (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ལོག f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(sa_nracanaa file *f)
{
  sa_nracanaa file ff;

  acquire(&ftable.lock);
  གལ་སྲིད (f->ref < 1)
    panic("fileclose");
  གལ་སྲིད (--f->ref > 0) {
    release(&ftable.lock);
    ལོག;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  གལ་སྲིད (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } དེ་མིན གལ་སྲིད (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ཧྲིལ
filestat(sa_nracanaa file *f, uint64 addr)
{
  sa_nracanaa proc *p = myproc();
  sa_nracanaa stat st;

  གལ་སྲིད (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    གལ་སྲིད (copyout(p->pagetable, p->sz, addr, (var_na *)&st, parimaa_na(st)) < 0)
      ལོག -1;
    ལོག 0;
  }
  ལོག -1;
}

// Read from file f.
// addr is a user virtual address.
ཧྲིལ
fileread(sa_nracanaa file *f, uint64 addr, ཧྲིལ n)
{
  ཧྲིལ r = 0;

  གལ་སྲིད (f->readable == 0 || n < 0)
    ལོག -1;

  གལ་སྲིད (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } དེ་མིན གལ་སྲིད (f->type == FD_DEVICE) {
    གལ་སྲིད (f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      ལོག -1;
    r = devsw[f->major].read(1, addr, n);
  } དེ་མིན གལ་སྲིད (f->type == FD_INODE) {
    ilock(f->ip);
    གལ་སྲིད ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } དེ་མིན {
    panic("fileread");
  }

  ལོག r;
}

// Write to file f.
// addr is a user virtual address.
ཧྲིལ
filewrite(sa_nracanaa file *f, uint64 addr, ཧྲིལ n)
{
  ཧྲིལ r, ret = 0;

  གལ་སྲིད (f->writable == 0 || n < 0)
    ལོག -1;

  གལ་སྲིད (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } དེ་མིན གལ་སྲིད (f->type == FD_DEVICE) {
    གལ་སྲིད (f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      ལོག -1;
    ret = devsw[f->major].write(1, addr, n);
  } དེ་མིན གལ་སྲིད (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ཧྲིལ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ཧྲིལ i = 0;
    སྐབས (i < n) {
      ཧྲིལ n1 = n - i;
      གལ་སྲིད (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      གལ་སྲིད ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      གལ་སྲིད (r != n1) {
        // error from writei
        bha_nga;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } དེ་མིན {
    panic("filewrite");
  }

  ལོག ret;
}
