//
// Support functions for system calls that involve file descriptors.
//

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "file.h"
#ଅନ୍ତର୍ଭାଵ "stat.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"

ସଂରଚନା devsw devsw[NDEV];
ସଂରଚନା {
  ସଂରଚନା spinlock lock;
  ସଂରଚନା file file[NFILE];
} ftable;

ଶୂନ୍ୟ
fileinit(ଶୂନ୍ୟ)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
ସଂରଚନା file *
filealloc(ଶୂନ୍ୟ)
{
  ସଂରଚନା file *f;

  acquire(&ftable.lock);
  ପାଇଁ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ଯଦି (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ଫେରନ୍ତୁ f;
    }
  }
  release(&ftable.lock);
  ଫେରନ୍ତୁ 0;
}

// Increment ref count for file f.
ସଂରଚନା file *
filedup(ସଂରଚନା file *f)
{
  acquire(&ftable.lock);
  ଯଦି (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ଫେରନ୍ତୁ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
ଶୂନ୍ୟ
fileclose(ସଂରଚନା file *f)
{
  ସଂରଚନା file ff;

  acquire(&ftable.lock);
  ଯଦି (f->ref < 1)
    panic("fileclose");
  ଯଦି (--f->ref > 0) {
    release(&ftable.lock);
    ଫେରନ୍ତୁ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ଯଦି (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ଅନ୍ୟଥା ଯଦି (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
filestat(ସଂରଚନା file *f, uint64 addr)
{
  ସଂରଚନା proc *p = myproc();
  ସଂରଚନା stat st;

  ଯଦି (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ଯଦି (copyout(p->pagetable, p->sz, addr, (ଅକ୍ଷର *)&st, ଆକାର(st)) < 0)
      ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ -1;
}

// Read from file f.
// addr is a user virtual address.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fileread(ସଂରଚନା file *f, uint64 addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

  ଯଦି (f->readable == 0 || n < 0)
    ଫେରନ୍ତୁ -1;

  ଯଦି (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ଅନ୍ୟଥା ଯଦି (f->type == FD_DEVICE) {
    ଯଦି (f->major < 0 || f->major >= NDEV || !devsw[f->major].ପଠ)
      ଫେରନ୍ତୁ -1;
    r = devsw[f->major].ପଠ(1, addr, n);
  } ଅନ୍ୟଥା ଯଦି (f->type == FD_INODE) {
    ilock(f->ip);
    ଯଦି ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ଅନ୍ୟଥା {
    panic("fileread");
  }

  ଫେରନ୍ତୁ r;
}

// Write to file f.
// addr is a user virtual address.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
filewrite(ସଂରଚନା file *f, uint64 addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

  ଯଦି (f->writable == 0 || n < 0)
    ଫେରନ୍ତୁ -1;

  ଯଦି (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ଅନ୍ୟଥା ଯଦି (f->type == FD_DEVICE) {
    ଯଦି (f->major < 0 || f->major >= NDEV || !devsw[f->major].ଲିଖ)
      ଫେରନ୍ତୁ -1;
    ret = devsw[f->major].ଲିଖ(1, addr, n);
  } ଅନ୍ୟଥା ଯଦି (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
    ଯେତେବେଳେ (i < n) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା n1 = n - i;
      ଯଦି (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ଯଦି ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ଯଦି (r != n1) {
        // error from writei
        ଭାଙ୍ଗ;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ଅନ୍ୟଥା {
    panic("filewrite");
  }

  ଫେରନ୍ତୁ ret;
}
