//
// Support functions for system calls that involve file descriptors.
//

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "file.h"
#અન્તર્ભાવ "stat.h"
#અન્તર્ભાવ "proc.h"

સંરચના devsw devsw[NDEV];
સંરચના {
  સંરચના spinlock lock;
  સંરચના file file[NFILE];
} ftable;

શૂન્ય
fileinit(શૂન્ય)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
સંરચના file *
filealloc(શૂન્ય)
{
  સંરચના file *f;

  acquire(&ftable.lock);
  માટે (f = ftable.file; f < ftable.file + NFILE; f++) {
    જો (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      પાછા f;
    }
  }
  release(&ftable.lock);
  પાછા 0;
}

// Increment ref count for file f.
સંરચના file *
filedup(સંરચના file *f)
{
  acquire(&ftable.lock);
  જો (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  પાછા f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
શૂન્ય
fileclose(સંરચના file *f)
{
  સંરચના file ff;

  acquire(&ftable.lock);
  જો (f->ref < 1)
    panic("fileclose");
  જો (--f->ref > 0) {
    release(&ftable.lock);
    પાછા;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  જો (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } નહીંતર જો (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
પૂર્ણાંક
filestat(સંરચના file *f, uint64 addr)
{
  સંરચના proc *p = myproc();
  સંરચના stat st;

  જો (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    જો (copyout(p->pagetable, p->sz, addr, (અક્ષર *)&st, કદ(st)) < 0)
      પાછા -1;
    પાછા 0;
  }
  પાછા -1;
}

// Read from file f.
// addr is a user virtual address.
પૂર્ણાંક
fileread(સંરચના file *f, uint64 addr, પૂર્ણાંક n)
{
  પૂર્ણાંક r = 0;

  જો (f->readable == 0 || n < 0)
    પાછા -1;

  જો (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } નહીંતર જો (f->type == FD_DEVICE) {
    જો (f->major < 0 || f->major >= NDEV || !devsw[f->major].પઠ)
      પાછા -1;
    r = devsw[f->major].પઠ(1, addr, n);
  } નહીંતર જો (f->type == FD_INODE) {
    ilock(f->ip);
    જો ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } નહીંતર {
    panic("fileread");
  }

  પાછા r;
}

// Write to file f.
// addr is a user virtual address.
પૂર્ણાંક
filewrite(સંરચના file *f, uint64 addr, પૂર્ણાંક n)
{
  પૂર્ણાંક r, ret = 0;

  જો (f->writable == 0 || n < 0)
    પાછા -1;

  જો (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } નહીંતર જો (f->type == FD_DEVICE) {
    જો (f->major < 0 || f->major >= NDEV || !devsw[f->major].લિખ)
      પાછા -1;
    ret = devsw[f->major].લિખ(1, addr, n);
  } નહીંતર જો (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    પૂર્ણાંક max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    પૂર્ણાંક i = 0;
    જ્યારે (i < n) {
      પૂર્ણાંક n1 = n - i;
      જો (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      જો ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      જો (r != n1) {
        // error from writei
        તોડો;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } નહીંતર {
    panic("filewrite");
  }

  પાછા ret;
}
