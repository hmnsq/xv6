//
// Support functions for system calls that involve file descriptors.
//

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "stat.h"
#ಅನ್ತರ್ಭಾವ "proc.h"

ಸಂರಚನಾ devsw devsw[NDEV];
ಸಂರಚನಾ {
  ಸಂರಚನಾ spinlock lock;
  ಸಂರಚನಾ file file[NFILE];
} ftable;

ಶೂನ್ಯ
fileinit(ಶೂನ್ಯ)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
ಸಂರಚನಾ file *
filealloc(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ file *f;

  acquire(&ftable.lock);
  ಪ್ರತಿ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ಯದಿ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      ಪ್ರತ್ಯಾಗಮ f;
    }
  }
  release(&ftable.lock);
  ಪ್ರತ್ಯಾಗಮ 0;
}

// Increment ref count for file f.
ಸಂರಚನಾ file *
filedup(ಸಂರಚನಾ file *f)
{
  acquire(&ftable.lock);
  ಯದಿ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  ಪ್ರತ್ಯಾಗಮ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
ಶೂನ್ಯ
fileclose(ಸಂರಚನಾ file *f)
{
  ಸಂರಚನಾ file ff;

  acquire(&ftable.lock);
  ಯದಿ (f->ref < 1)
    panic("fileclose");
  ಯದಿ (--f->ref > 0) {
    release(&ftable.lock);
    ಪ್ರತ್ಯಾಗಮ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  ಯದಿ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ಅನ್ಯಥಾ ಯದಿ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ಪೂರ್ಣಾಙ್ಕ
filestat(ಸಂರಚನಾ file *f, uint64 addr)
{
  ಸಂರಚನಾ proc *p = myproc();
  ಸಂರಚನಾ stat st;

  ಯದಿ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ಯದಿ (copyout(p->pagetable, p->sz, addr, (ವರ್ಣ *)&st, ಪರಿಮಾಣ(st)) < 0)
      ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

// Read from file f.
// addr is a user virtual address.
ಪೂರ್ಣಾಙ್ಕ
fileread(ಸಂರಚನಾ file *f, uint64 addr, ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪೂರ್ಣಾಙ್ಕ r = 0;

  ಯದಿ (f->readable == 0 || n < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  ಯದಿ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ಅನ್ಯಥಾ ಯದಿ (f->type == FD_DEVICE) {
    ಯದಿ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ಪಠ)
      ಪ್ರತ್ಯಾಗಮ -1;
    r = devsw[f->major].ಪಠ(1, addr, n);
  } ಅನ್ಯಥಾ ಯದಿ (f->type == FD_INODE) {
    ilock(f->ip);
    ಯದಿ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ಅನ್ಯಥಾ {
    panic("fileread");
  }

  ಪ್ರತ್ಯಾಗಮ r;
}

// Write to file f.
// addr is a user virtual address.
ಪೂರ್ಣಾಙ್ಕ
filewrite(ಸಂರಚನಾ file *f, uint64 addr, ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

  ಯದಿ (f->writable == 0 || n < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  ಯದಿ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ಅನ್ಯಥಾ ಯದಿ (f->type == FD_DEVICE) {
    ಯದಿ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ಲಿಖ)
      ಪ್ರತ್ಯಾಗಮ -1;
    ret = devsw[f->major].ಲಿಖ(1, addr, n);
  } ಅನ್ಯಥಾ ಯದಿ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ಪೂರ್ಣಾಙ್ಕ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ಪೂರ್ಣಾಙ್ಕ i = 0;
    ಯಾವತ್ (i < n) {
      ಪೂರ್ಣಾಙ್ಕ n1 = n - i;
      ಯದಿ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ಯದಿ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      ಯದಿ (r != n1) {
        // error from writei
        ಭಙ್ಗ;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ಅನ್ಯಥಾ {
    panic("filewrite");
  }

  ಪ್ರತ್ಯಾಗಮ ret;
}
