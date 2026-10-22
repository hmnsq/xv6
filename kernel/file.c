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
  بنية spinlock lock;
  بنية file file[NFILE];
} ftable;

فراغ
fileinit(فراغ)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
بنية file *
filealloc(فراغ)
{
  بنية file *f;

  acquire(&ftable.lock);
  لكل (f = ftable.file; f < ftable.file + NFILE; f++) {
    ܐܢ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ܦܢܐ f;
    }
  }
  release(&ftable.lock);
  ܦܢܐ 0;
}

// Increment ref count for file f.
بنية file *
filedup(بنية file *f)
{
  acquire(&ftable.lock);
  ܐܢ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ܦܢܐ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
فراغ
fileclose(بنية file *f)
{
  بنية file ff;

  acquire(&ftable.lock);
  ܐܢ (f->ref < 1)
    panic("fileclose");
  ܐܢ (--f->ref > 0) {
    release(&ftable.lock);
    ܦܢܐ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ܐܢ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ܐܚܪܢܐ ܐܢ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
صحيح
filestat(بنية file *f, uint64 addr)
{
  بنية proc *p = myproc();
  بنية stat st;

  ܐܢ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ܐܢ (copyout(p->pagetable, p->sz, addr, (حرف *)&st, حجم(st)) < 0)
      ܦܢܐ -1;
    ܦܢܐ 0;
  }
  ܦܢܐ -1;
}

// Read from file f.
// addr is a user virtual address.
صحيح
fileread(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r = 0;

  ܐܢ (f->readable == 0 || n < 0)
    ܦܢܐ -1;

  ܐܢ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ܐܚܪܢܐ ܐܢ (f->type == FD_DEVICE) {
    ܐܢ (f->major < 0 || f->major >= NDEV || !devsw[f->major].اقرأ)
      ܦܢܐ -1;
    r = devsw[f->major].اقرأ(1, addr, n);
  } ܐܚܪܢܐ ܐܢ (f->type == FD_INODE) {
    ilock(f->ip);
    ܐܢ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ܐܚܪܢܐ {
    panic("fileread");
  }

  ܦܢܐ r;
}

// Write to file f.
// addr is a user virtual address.
صحيح
filewrite(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r, ret = 0;

  ܐܢ (f->writable == 0 || n < 0)
    ܦܢܐ -1;

  ܐܢ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ܐܚܪܢܐ ܐܢ (f->type == FD_DEVICE) {
    ܐܢ (f->major < 0 || f->major >= NDEV || !devsw[f->major].اكتب)
      ܦܢܐ -1;
    ret = devsw[f->major].اكتب(1, addr, n);
  } ܐܚܪܢܐ ܐܢ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    صحيح max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    صحيح i = 0;
    ܟܡܐ (i < n) {
      صحيح n1 = n - i;
      ܐܢ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ܐܢ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ܐܢ (r != n1) {
        // error from writei
        اكسر;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ܐܚܪܢܐ {
    panic("filewrite");
  }

  ܦܢܐ ret;
}
