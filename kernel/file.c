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

ರಚನೆ devsw devsw[NDEV];
ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;
  ರಚನೆ file file[NFILE];
} ftable;

ಶೂನ್ಯ
fileinit(ಶೂನ್ಯ)
{
  initlock(&ftable.ಬೀಗ, "ftable");
}

// Allocate a file structure.
ರಚನೆ file *
filealloc(ಶೂನ್ಯ)
{
  ರಚನೆ file *f;

  acquire(&ftable.ಬೀಗ);
  ಗಾಗಿ (f = ftable.file; f < ftable.file + NFILE; f++) {
    ಆದರೆ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.ಬೀಗ);
      ಹಿಂತಿರುಗಿ f;
    }
  }
  release(&ftable.ಬೀಗ);
  ಹಿಂತಿರುಗಿ 0;
}

// Increment ref count for file f.
ರಚನೆ file *
filedup(ರಚನೆ file *f)
{
  acquire(&ftable.ಬೀಗ);
  ಆದರೆ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.ಬೀಗ);
  ಹಿಂತಿರುಗಿ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
ಶೂನ್ಯ
fileclose(ರಚನೆ file *f)
{
  ರಚನೆ file ff;

  acquire(&ftable.ಬೀಗ);
  ಆದರೆ (f->ref < 1)
    panic("fileclose");
  ಆದರೆ (--f->ref > 0) {
    release(&ftable.ಬೀಗ);
    ಹಿಂತಿರುಗಿ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.ಬೀಗ);

  ಆದರೆ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    ಕೊನೆ_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
ಪೂರ್ಣಾಂಕ
filestat(ರಚನೆ file *f, uint64 addr)
{
  ರಚನೆ proc *p = myproc();
  ರಚನೆ stat st;

  ಆದರೆ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    ಆದರೆ (copyout(p->pagetable, p->sz, addr, (ಅಕ್ಷರ *)&st, ಗಾತ್ರ(st)) < 0)
      ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ -1;
}

// Read from file f.
// addr is a user virtual address.
ಪೂರ್ಣಾಂಕ
fileread(ರಚನೆ file *f, uint64 addr, ಪೂರ್ಣಾಂಕ n)
{
  ಪೂರ್ಣಾಂಕ r = 0;

  ಆದರೆ (f->readable == 0 || n < 0)
    ಹಿಂತಿರುಗಿ -1;

  ಆದರೆ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f->type == FD_DEVICE) {
    ಆದರೆ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ಪಠ)
      ಹಿಂತಿರುಗಿ -1;
    r = devsw[f->major].ಪಠ(1, addr, n);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f->type == FD_INODE) {
    ilock(f->ip);
    ಆದರೆ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } ಇಲ್ಲದಿದ್ದರೆ {
    panic("fileread");
  }

  ಹಿಂತಿರುಗಿ r;
}

// Write to file f.
// addr is a user virtual address.
ಪೂರ್ಣಾಂಕ
filewrite(ರಚನೆ file *f, uint64 addr, ಪೂರ್ಣಾಂಕ n)
{
  ಪೂರ್ಣಾಂಕ r, ret = 0;

  ಆದರೆ (f->writable == 0 || n < 0)
    ಹಿಂತಿರುಗಿ -1;

  ಆದರೆ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f->type == FD_DEVICE) {
    ಆದರೆ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ಲಿಖ)
      ಹಿಂತಿರುಗಿ -1;
    ret = devsw[f->major].ಲಿಖ(1, addr, n);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    ಪೂರ್ಣಾಂಕ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    ಪೂರ್ಣಾಂಕ i = 0;
    ಆಗ (i < n) {
      ಪೂರ್ಣಾಂಕ n1 = n - i;
      ಆದರೆ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      ಆದರೆ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      ಕೊನೆ_op();

      ಆದರೆ (r != n1) {
        // error from writei
        ನಿಲ್ಲಿಸು;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } ಇಲ್ಲದಿದ್ದರೆ {
    panic("filewrite");
  }

  ಹಿಂತಿರುಗಿ ret;
}
