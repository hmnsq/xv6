//
// Support functions for system calls that involve file descriptors.
//

#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "fs.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "sleeplock.h"
#অন্তর্ভাৱ "file.h"
#অন্তর্ভাৱ "stat.h"
#অন্তর্ভাৱ "proc.h"

সংরচনা devsw devsw[NDEV];
সংরচনা {
  সংরচনা spinlock lock;
  সংরচনা file file[NFILE];
} ftable;

শূন্য
fileinit(শূন্য)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
সংরচনা file *
filealloc(শূন্য)
{
  সংরচনা file *f;

  acquire(&ftable.lock);
  বাবে (f = ftable.file; f < ftable.file + NFILE; f++) {
    যদি (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ঘূৰাই f;
    }
  }
  release(&ftable.lock);
  ঘূৰাই 0;
}

// Increment ref count for file f.
সংরচনা file *
filedup(সংরচনা file *f)
{
  acquire(&ftable.lock);
  যদি (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ঘূৰাই f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
শূন্য
fileclose(সংরচনা file *f)
{
  সংরচনা file ff;

  acquire(&ftable.lock);
  যদি (f->ref < 1)
    panic("fileclose");
  যদি (--f->ref > 0) {
    release(&ftable.lock);
    ঘূৰাই;
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
পূর্ণাঙ্ক
filestat(সংরচনা file *f, uint64 addr)
{
  সংরচনা proc *p = myproc();
  সংরচনা stat st;

  যদি (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    যদি (copyout(p->pagetable, p->sz, addr, (ৱর্ণ *)&st, পরিমাণ(st)) < 0)
      ঘূৰাই -1;
    ঘূৰাই 0;
  }
  ঘূৰাই -1;
}

// Read from file f.
// addr is a user virtual address.
পূর্ণাঙ্ক
fileread(সংরচনা file *f, uint64 addr, পূর্ণাঙ্ক n)
{
  পূর্ণাঙ্ক r = 0;

  যদি (f->readable == 0 || n < 0)
    ঘূৰাই -1;

  যদি (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } অন্যথা যদি (f->type == FD_DEVICE) {
    যদি (f->major < 0 || f->major >= NDEV || !devsw[f->major].পঠ)
      ঘূৰাই -1;
    r = devsw[f->major].পঠ(1, addr, n);
  } অন্যথা যদি (f->type == FD_INODE) {
    ilock(f->ip);
    যদি ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } অন্যথা {
    panic("fileread");
  }

  ঘূৰাই r;
}

// Write to file f.
// addr is a user virtual address.
পূর্ণাঙ্ক
filewrite(সংরচনা file *f, uint64 addr, পূর্ণাঙ্ক n)
{
  পূর্ণাঙ্ক r, ret = 0;

  যদি (f->writable == 0 || n < 0)
    ঘূৰাই -1;

  যদি (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } অন্যথা যদি (f->type == FD_DEVICE) {
    যদি (f->major < 0 || f->major >= NDEV || !devsw[f->major].লিখ)
      ঘূৰাই -1;
    ret = devsw[f->major].লিখ(1, addr, n);
  } অন্যথা যদি (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    পূর্ণাঙ্ক max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    পূর্ণাঙ্ক i = 0;
    যেতিয়ালৈ (i < n) {
      পূর্ণাঙ্ক n1 = n - i;
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
        ভঙ্গ;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } অন্যথা {
    panic("filewrite");
  }

  ঘূৰাই ret;
}
