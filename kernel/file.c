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

ساخت devsw devsw[NDEV];
ساخت {
  ساخت spinlock تالا;
  ساخت file file[NFILE];
} ftable;

خالی
fileinit(خالی)
{
  initlock(&ftable.تالا, "ftable");
}

// Allocate a file structure.
ساخت file *
filealloc(خالی)
{
  ساخت file *f;

  acquire(&ftable.تالا);
  برائے (f = ftable.file; f < ftable.file + NFILE; f++) {
    اگر (f->ref == 0) {
      f->ref = 1;
      release(&ftable.تالا);
      واپس f;
    }
  }
  release(&ftable.تالا);
  واپس 0;
}

// Increment ref count for file f.
ساخت file *
filedup(ساخت file *f)
{
  acquire(&ftable.تالا);
  اگر (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.تالا);
  واپس f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
خالی
fileclose(ساخت file *f)
{
  ساخت file ff;

  acquire(&ftable.تالا);
  اگر (f->ref < 1)
    panic("fileclose");
  اگر (--f->ref > 0) {
    release(&ftable.تالا);
    واپس;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.تالا);

  اگر (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ورنہ اگر (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    انتہا_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
صحیح
filestat(ساخت file *f, uint64 addr)
{
  ساخت proc *p = myproc();
  ساخت stat st;

  اگر (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    اگر (copyout(p->pagetable, p->sz, addr, (حرف *)&st, حجم(st)) < 0)
      واپس -1;
    واپس 0;
  }
  واپس -1;
}

// Read from file f.
// addr is a user virtual address.
صحیح
fileread(ساخت file *f, uint64 addr, صحیح n)
{
  صحیح r = 0;

  اگر (f->readable == 0 || n < 0)
    واپس -1;

  اگر (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ورنہ اگر (f->type == FD_DEVICE) {
    اگر (f->major < 0 || f->major >= NDEV || !devsw[f->major].اقرأ)
      واپس -1;
    r = devsw[f->major].اقرأ(1, addr, n);
  } ورنہ اگر (f->type == FD_INODE) {
    ilock(f->ip);
    اگر ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ورنہ {
    panic("fileread");
  }

  واپس r;
}

// Write to file f.
// addr is a user virtual address.
صحیح
filewrite(ساخت file *f, uint64 addr, صحیح n)
{
  صحیح r, ret = 0;

  اگر (f->writable == 0 || n < 0)
    واپس -1;

  اگر (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ورنہ اگر (f->type == FD_DEVICE) {
    اگر (f->major < 0 || f->major >= NDEV || !devsw[f->major].اكتب)
      واپس -1;
    ret = devsw[f->major].اكتب(1, addr, n);
  } ورنہ اگر (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    صحیح max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    صحیح i = 0;
    جب (i < n) {
      صحیح n1 = n - i;
      اگر (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      اگر ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      انتہا_op();

      اگر (r != n1) {
        // error from writei
        توڑو;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ورنہ {
    panic("filewrite");
  }

  واپس ret;
}
