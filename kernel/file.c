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
  עבור (f = ftable.file; f < ftable.file + NFILE; f++) {
    אם (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      החזר f;
    }
  }
  release(&ftable.lock);
  החזר 0;
}

// Increment ref count for file f.
بنية file *
filedup(بنية file *f)
{
  acquire(&ftable.lock);
  אם (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  החזר f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
فراغ
fileclose(بنية file *f)
{
  بنية file ff;

  acquire(&ftable.lock);
  אם (f->ref < 1)
    panic("fileclose");
  אם (--f->ref > 0) {
    release(&ftable.lock);
    החזר;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  אם (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } אחרת אם (ff.type == FD_INODE || ff.type == FD_DEVICE) {
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

  אם (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    אם (copyout(p->pagetable, p->sz, addr, (حرف *)&st, حجم(st)) < 0)
      החזר -1;
    החזר 0;
  }
  החזר -1;
}

// Read from file f.
// addr is a user virtual address.
صحيح
fileread(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r = 0;

  אם (f->readable == 0 || n < 0)
    החזר -1;

  אם (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } אחרת אם (f->type == FD_DEVICE) {
    אם (f->major < 0 || f->major >= NDEV || !devsw[f->major].اقرأ)
      החזר -1;
    r = devsw[f->major].اقرأ(1, addr, n);
  } אחרת אם (f->type == FD_INODE) {
    ilock(f->ip);
    אם ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } אחרת {
    panic("fileread");
  }

  החזר r;
}

// Write to file f.
// addr is a user virtual address.
صحيح
filewrite(بنية file *f, uint64 addr, صحيح n)
{
  صحيح r, ret = 0;

  אם (f->writable == 0 || n < 0)
    החזר -1;

  אם (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } אחרת אם (f->type == FD_DEVICE) {
    אם (f->major < 0 || f->major >= NDEV || !devsw[f->major].اكتب)
      החזר -1;
    ret = devsw[f->major].اكتب(1, addr, n);
  } אחרת אם (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    صحيح max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    صحيح i = 0;
    כלעוד (i < n) {
      صحيح n1 = n - i;
      אם (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      אם ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      אם (r != n1) {
        // error from writei
        اكسر;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } אחרת {
    panic("filewrite");
  }

  החזר ret;
}
