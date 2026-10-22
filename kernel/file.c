//
// Support functions for system calls that involve file descriptors.
//

#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "stat.h"
#అన్తర్భావ "proc.h"

నిర్మాణం devsw devsw[NDEV];
నిర్మాణం {
  నిర్మాణం spinlock తాళం;
  నిర్మాణం file file[NFILE];
} ftable;

శూన్య
fileinit(శూన్య)
{
  initlock(&ftable.తాళం, "ftable");
}

// Allocate a file structure.
నిర్మాణం file *
filealloc(శూన్య)
{
  నిర్మాణం file *f;

  acquire(&ftable.తాళం);
  కొరకు (f = ftable.file; f < ftable.file + NFILE; f++) {
    ఒకవేళ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.తాళం);
      తిరిగి f;
    }
  }
  release(&ftable.తాళం);
  తిరిగి 0;
}

// Increment ref count for file f.
నిర్మాణం file *
filedup(నిర్మాణం file *f)
{
  acquire(&ftable.తాళం);
  ఒకవేళ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.తాళం);
  తిరిగి f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
శూన్య
fileclose(నిర్మాణం file *f)
{
  నిర్మాణం file ff;

  acquire(&ftable.తాళం);
  ఒకవేళ (f->ref < 1)
    panic("fileclose");
  ఒకవేళ (--f->ref > 0) {
    release(&ftable.తాళం);
    తిరిగి;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.తాళం);

  ఒకవేళ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } లేకపోతే ఒకవేళ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    చివర_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
పూర్ణాంకం
filestat(నిర్మాణం file *f, uint64 addr)
{
  నిర్మాణం proc *p = myproc();
  నిర్మాణం stat st;

  ఒకవేళ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ఒకవేళ (copyout(p->pagetable, p->sz, addr, (అక్షరం *)&st, పరిమాణం(st)) < 0)
      తిరిగి -1;
    తిరిగి 0;
  }
  తిరిగి -1;
}

// Read from file f.
// addr is a user virtual address.
పూర్ణాంకం
fileread(నిర్మాణం file *f, uint64 addr, పూర్ణాంకం n)
{
  పూర్ణాంకం r = 0;

  ఒకవేళ (f->readable == 0 || n < 0)
    తిరిగి -1;

  ఒకవేళ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } లేకపోతే ఒకవేళ (f->type == FD_DEVICE) {
    ఒకవేళ (f->major < 0 || f->major >= NDEV || !devsw[f->major].పఠ)
      తిరిగి -1;
    r = devsw[f->major].పఠ(1, addr, n);
  } లేకపోతే ఒకవేళ (f->type == FD_INODE) {
    ilock(f->ip);
    ఒకవేళ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } లేకపోతే {
    panic("fileread");
  }

  తిరిగి r;
}

// Write to file f.
// addr is a user virtual address.
పూర్ణాంకం
filewrite(నిర్మాణం file *f, uint64 addr, పూర్ణాంకం n)
{
  పూర్ణాంకం r, ret = 0;

  ఒకవేళ (f->writable == 0 || n < 0)
    తిరిగి -1;

  ఒకవేళ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } లేకపోతే ఒకవేళ (f->type == FD_DEVICE) {
    ఒకవేళ (f->major < 0 || f->major >= NDEV || !devsw[f->major].లిఖ)
      తిరిగి -1;
    ret = devsw[f->major].లిఖ(1, addr, n);
  } లేకపోతే ఒకవేళ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    పూర్ణాంకం max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    పూర్ణాంకం i = 0;
    అయితే (i < n) {
      పూర్ణాంకం n1 = n - i;
      ఒకవేళ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ఒకవేళ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      చివర_op();

      ఒకవేళ (r != n1) {
        // error from writei
        ఆపు;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } లేకపోతే {
    panic("filewrite");
  }

  తిరిగి ret;
}
