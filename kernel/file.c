//
// Support functions for system calls that involve file descriptors.
//

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "file.h"
#അന്തര്ഭാവ "stat.h"
#അന്തര്ഭാവ "proc.h"

ഘടന devsw devsw[NDEV];
ഘടന {
  ഘടന spinlock lock;
  ഘടന file file[NFILE];
} ftable;

ശൂന്യം
fileinit(ശൂന്യം)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
ഘടന file *
filealloc(ശൂന്യം)
{
  ഘടന file *f;

  acquire(&ftable.lock);
  വേണ്ടി (f = ftable.file; f < ftable.file + NFILE; f++) {
    എങ്കിൽ (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      തിരികെ f;
    }
  }
  release(&ftable.lock);
  തിരികെ 0;
}

// Increment ref count for file f.
ഘടന file *
filedup(ഘടന file *f)
{
  acquire(&ftable.lock);
  എങ്കിൽ (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  തിരികെ f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
ശൂന്യം
fileclose(ഘടന file *f)
{
  ഘടന file ff;

  acquire(&ftable.lock);
  എങ്കിൽ (f->ref < 1)
    panic("fileclose");
  എങ്കിൽ (--f->ref > 0) {
    release(&ftable.lock);
    തിരികെ;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  എങ്കിൽ (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } അല്ലെങ്കിൽ എങ്കിൽ (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
പൂർണ്ണസംഖ്യ
filestat(ഘടന file *f, uint64 addr)
{
  ഘടന proc *p = myproc();
  ഘടന stat st;

  എങ്കിൽ (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    എങ്കിൽ (copyout(p->pagetable, p->sz, addr, (അക്ഷരം *)&st, വലുപ്പം(st)) < 0)
      തിരികെ -1;
    തിരികെ 0;
  }
  തിരികെ -1;
}

// Read from file f.
// addr is a user virtual address.
പൂർണ്ണസംഖ്യ
fileread(ഘടന file *f, uint64 addr, പൂർണ്ണസംഖ്യ n)
{
  പൂർണ്ണസംഖ്യ r = 0;

  എങ്കിൽ (f->readable == 0 || n < 0)
    തിരികെ -1;

  എങ്കിൽ (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } അല്ലെങ്കിൽ എങ്കിൽ (f->type == FD_DEVICE) {
    എങ്കിൽ (f->major < 0 || f->major >= NDEV || !devsw[f->major].പഠ)
      തിരികെ -1;
    r = devsw[f->major].പഠ(1, addr, n);
  } അല്ലെങ്കിൽ എങ്കിൽ (f->type == FD_INODE) {
    ilock(f->ip);
    എങ്കിൽ ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } അല്ലെങ്കിൽ {
    panic("fileread");
  }

  തിരികെ r;
}

// Write to file f.
// addr is a user virtual address.
പൂർണ്ണസംഖ്യ
filewrite(ഘടന file *f, uint64 addr, പൂർണ്ണസംഖ്യ n)
{
  പൂർണ്ണസംഖ്യ r, ret = 0;

  എങ്കിൽ (f->writable == 0 || n < 0)
    തിരികെ -1;

  എങ്കിൽ (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } അല്ലെങ്കിൽ എങ്കിൽ (f->type == FD_DEVICE) {
    എങ്കിൽ (f->major < 0 || f->major >= NDEV || !devsw[f->major].ലിഖ)
      തിരികെ -1;
    ret = devsw[f->major].ലിഖ(1, addr, n);
  } അല്ലെങ്കിൽ എങ്കിൽ (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    പൂർണ്ണസംഖ്യ max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    പൂർണ്ണസംഖ്യ i = 0;
    ആകുമ്പോൾ (i < n) {
      പൂർണ്ണസംഖ്യ n1 = n - i;
      എങ്കിൽ (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      എങ്കിൽ ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      എങ്കിൽ (r != n1) {
        // error from writei
        നിർത്തുക;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } അല്ലെങ്കിൽ {
    panic("filewrite");
  }

  തിരികെ ret;
}
