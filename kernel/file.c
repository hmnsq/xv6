//
// Support functions for system calls that involve file descriptors.
//

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "fs.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "file.h"
#شامل "stat.h"
#شامل "proc.h"

ساختار devsw devsw[NDEV];
ساختار {
  ساختار spinlock lock;
  ساختار file file[NFILE];
} ftable;

تهی
fileinit(تهی)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
ساختار file *
filealloc(تهی)
{
  ساختار file *f;

  acquire(&ftable.lock);
  برای (f = ftable.file; f < ftable.file + NFILE; f++) {
    اگر (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      بازگشت f;
    }
  }
  release(&ftable.lock);
  بازگشت 0;
}

// Increment ref count for file f.
ساختار file *
filedup(ساختار file *f)
{
  acquire(&ftable.lock);
  اگر (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  بازگشت f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
تهی
fileclose(ساختار file *f)
{
  ساختار file ff;

  acquire(&ftable.lock);
  اگر (f->ref < 1)
    panic("fileclose");
  اگر (--f->ref > 0) {
    release(&ftable.lock);
    بازگشت;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  اگر (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } وگرنه اگر (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
صحیح
filestat(ساختار file *f, uint64 addr)
{
  ساختار proc *p = myproc();
  ساختار stat st;

  اگر (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    اگر (copyout(p->pagetable, p->sz, addr, (نویسه *)&st, اندازه(st)) < 0)
      بازگشت -1;
    بازگشت 0;
  }
  بازگشت -1;
}

// Read from file f.
// addr is a user virtual address.
صحیح
fileread(ساختار file *f, uint64 addr, صحیح n)
{
  صحیح r = 0;

  اگر (f->readable == 0 || n < 0)
    بازگشت -1;

  اگر (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } وگرنه اگر (f->type == FD_DEVICE) {
    اگر (f->major < 0 || f->major >= NDEV || !devsw[f->major].اقرأ)
      بازگشت -1;
    r = devsw[f->major].اقرأ(1, addr, n);
  } وگرنه اگر (f->type == FD_INODE) {
    ilock(f->ip);
    اگر ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } وگرنه {
    panic("fileread");
  }

  بازگشت r;
}

// Write to file f.
// addr is a user virtual address.
صحیح
filewrite(ساختار file *f, uint64 addr, صحیح n)
{
  صحیح r, ret = 0;

  اگر (f->writable == 0 || n < 0)
    بازگشت -1;

  اگر (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } وگرنه اگر (f->type == FD_DEVICE) {
    اگر (f->major < 0 || f->major >= NDEV || !devsw[f->major].اكتب)
      بازگشت -1;
    ret = devsw[f->major].اكتب(1, addr, n);
  } وگرنه اگر (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    صحیح max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    صحیح i = 0;
    وقتی (i < n) {
      صحیح n1 = n - i;
      اگر (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      اگر ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      اگر (r != n1) {
        // error from writei
        بشکن;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } وگرنه {
    panic("filewrite");
  }

  بازگشت ret;
}
