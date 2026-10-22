//
// Support functions for system calls that involve file descriptors.
//

#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "fs.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "file.h"
#ضمن "stat.h"
#ضمن "proc.h"

بنية devsw devsw[NDEV];
بنية {
  بنية spinlock تالو;
  بنية file file[NFILE];
} ftable;

فراغ
fileinit(فراغ)
{
  initlock(&ftable.تالو, "ftable");
}

// Allocate a file structure.
بنية file *
filealloc(فراغ)
{
  بنية file *f;

  acquire(&ftable.تالو);
  لكل (f = ftable.file; f < ftable.file + NFILE; f++) {
    إذا (f->ref == 0) {
      f->ref = 1;
      release(&ftable.تالو);
      أرجع f;
    }
  }
  release(&ftable.تالو);
  أرجع 0;
}

// Increment ref count for file f.
بنية file *
filedup(بنية file *f)
{
  acquire(&ftable.تالو);
  إذا (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.تالو);
  أرجع f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
فراغ
fileclose(بنية file *f)
{
  بنية file ff;

  acquire(&ftable.تالو);
  إذا (f->ref < 1)
    panic("fileclose");
  إذا (--f->ref > 0) {
    release(&ftable.تالو);
    أرجع;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.تالو);

  إذا (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } وإلا إذا (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    پڇاڙي_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
صحيح
filestat(بنية file *f, uint64 addr)
{
  بنية proc *p = myproc();
  بنية stat st;

  إذا (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    إذا (copyout(p->pagetable, p->sz, addr, (حرف *)&st, حجم(st)) < 0)
      أرجع -1;
    أرجع 0;
  }
  أرجع -1;
}

// Read from file f.
// addr is a user virtual address.
صحيح
fileread(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r = 0;

  إذا (f->readable == 0 || n < 0)
    أرجع -1;

  إذا (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } وإلا إذا (f->type == FD_DEVICE) {
    إذا (f->major < 0 || f->major >= NDEV || !devsw[f->major].اقرأ)
      أرجع -1;
    r = devsw[f->major].اقرأ(1, addr, n);
  } وإلا إذا (f->type == FD_INODE) {
    ilock(f->ip);
    إذا ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } وإلا {
    panic("fileread");
  }

  أرجع r;
}

// Write to file f.
// addr is a user virtual address.
صحيح
filewrite(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r, ret = 0;

  إذا (f->writable == 0 || n < 0)
    أرجع -1;

  إذا (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } وإلا إذا (f->type == FD_DEVICE) {
    إذا (f->major < 0 || f->major >= NDEV || !devsw[f->major].اكتب)
      أرجع -1;
    ret = devsw[f->major].اكتب(1, addr, n);
  } وإلا إذا (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    صحيح max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    صحيح i = 0;
    طالما (i < n) {
      صحيح n1 = n - i;
      إذا (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      إذا ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      پڇاڙي_op();

      إذا (r != n1) {
        // error from writei
        اكسر;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } وإلا {
    panic("filewrite");
  }

  أرجع ret;
}
