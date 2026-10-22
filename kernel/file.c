//
// Support functions for system calls that involve file descriptors.
//

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "stat.h"
#অন্তর্ভাব "proc.h"

কাঠামো devsw devsw[NDEV];
কাঠামো {
  কাঠামো spinlock lock;
  কাঠামো file file[NFILE];
} ftable;

শূন্য
fileinit(শূন্য)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
কাঠামো file *
filealloc(শূন্য)
{
  কাঠামো file *f;

  acquire(&ftable.lock);
  জন্য (f = ftable.file; f < ftable.file + NFILE; f++) {
    যদি (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ফেরত f;
    }
  }
  release(&ftable.lock);
  ফেরত 0;
}

// Increment ref count for file f.
কাঠামো file *
filedup(কাঠামো file *f)
{
  acquire(&ftable.lock);
  যদি (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ফেরত f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
শূন্য
fileclose(কাঠামো file *f)
{
  কাঠামো file ff;

  acquire(&ftable.lock);
  যদি (f->ref < 1)
    panic("fileclose");
  যদি (--f->ref > 0) {
    release(&ftable.lock);
    ফেরত;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  যদি (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } অন্যথা যদি (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
পূর্ণসংখ্যা
filestat(কাঠামো file *f, uint64 addr)
{
  কাঠামো proc *p = myproc();
  কাঠামো stat st;

  যদি (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    যদি (copyout(p->pagetable, p->sz, addr, (অক্ষর *)&st, আকার(st)) < 0)
      ফেরত -1;
    ফেরত 0;
  }
  ফেরত -1;
}

// Read from file f.
// addr is a user virtual address.
পূর্ণসংখ্যা
fileread(কাঠামো file *f, uint64 addr, পূর্ণসংখ্যা n)
{
  পূর্ণসংখ্যা r = 0;

  যদি (f->readable == 0 || n < 0)
    ফেরত -1;

  যদি (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } অন্যথা যদি (f->type == FD_DEVICE) {
    যদি (f->major < 0 || f->major >= NDEV || !devsw[f->major].পঠ)
      ফেরত -1;
    r = devsw[f->major].পঠ(1, addr, n);
  } অন্যথা যদি (f->type == FD_INODE) {
    ilock(f->ip);
    যদি ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } অন্যথা {
    panic("fileread");
  }

  ফেরত r;
}

// Write to file f.
// addr is a user virtual address.
পূর্ণসংখ্যা
filewrite(কাঠামো file *f, uint64 addr, পূর্ণসংখ্যা n)
{
  পূর্ণসংখ্যা r, ret = 0;

  যদি (f->writable == 0 || n < 0)
    ফেরত -1;

  যদি (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } অন্যথা যদি (f->type == FD_DEVICE) {
    যদি (f->major < 0 || f->major >= NDEV || !devsw[f->major].লিখ)
      ফেরত -1;
    ret = devsw[f->major].লিখ(1, addr, n);
  } অন্যথা যদি (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    পূর্ণসংখ্যা max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    পূর্ণসংখ্যা i = 0;
    যখন (i < n) {
      পূর্ণসংখ্যা n1 = n - i;
      যদি (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      যদি ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      যদি (r != n1) {
        // error from writei
        বিরতি;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } অন্যথা {
    panic("filewrite");
  }

  ফেরত ret;
}
