//
// Support functions for system calls that involve file descriptors.
//

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "stat.h"
#அந்தர்பாவ "proc.h"

கட்டமைப்பு devsw devsw[NDEV];
கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;
  கட்டமைப்பு file file[NFILE];
} ftable;

வெறுமை
fileinit(வெறுமை)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
கட்டமைப்பு file *
filealloc(வெறுமை)
{
  கட்டமைப்பு file *f;

  acquire(&ftable.lock);
  க்கு (f = ftable.file; f < ftable.file + NFILE; f++) {
    எனில் (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      திரும்பு f;
    }
  }
  release(&ftable.lock);
  திரும்பு 0;
}

// Increment ref count for file f.
கட்டமைப்பு file *
filedup(கட்டமைப்பு file *f)
{
  acquire(&ftable.lock);
  எனில் (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  திரும்பு f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
வெறுமை
fileclose(கட்டமைப்பு file *f)
{
  கட்டமைப்பு file ff;

  acquire(&ftable.lock);
  எனில் (f->ref < 1)
    panic("fileclose");
  எனில் (--f->ref > 0) {
    release(&ftable.lock);
    திரும்பு;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  எனில் (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } இல்லையெனில் எனில் (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
முழு
filestat(கட்டமைப்பு file *f, uint64 addr)
{
  கட்டமைப்பு proc *p = myproc();
  கட்டமைப்பு stat st;

  எனில் (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    எனில் (copyout(p->pagetable, p->sz, addr, (எழுத்து *)&st, அளவு(st)) < 0)
      திரும்பு -1;
    திரும்பு 0;
  }
  திரும்பு -1;
}

// Read from file f.
// addr is a user virtual address.
முழு
fileread(கட்டமைப்பு file *f, uint64 addr, முழு n)
{
  முழு r = 0;

  எனில் (f->readable == 0 || n < 0)
    திரும்பு -1;

  எனில் (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } இல்லையெனில் எனில் (f->type == FD_DEVICE) {
    எனில் (f->major < 0 || f->major >= NDEV || !devsw[f->major].பட)
      திரும்பு -1;
    r = devsw[f->major].பட(1, addr, n);
  } இல்லையெனில் எனில் (f->type == FD_INODE) {
    ilock(f->ip);
    எனில் ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } இல்லையெனில் {
    panic("fileread");
  }

  திரும்பு r;
}

// Write to file f.
// addr is a user virtual address.
முழு
filewrite(கட்டமைப்பு file *f, uint64 addr, முழு n)
{
  முழு r, ret = 0;

  எனில் (f->writable == 0 || n < 0)
    திரும்பு -1;

  எனில் (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } இல்லையெனில் எனில் (f->type == FD_DEVICE) {
    எனில் (f->major < 0 || f->major >= NDEV || !devsw[f->major].லிக)
      திரும்பு -1;
    ret = devsw[f->major].லிக(1, addr, n);
  } இல்லையெனில் எனில் (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    முழு max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    முழு i = 0;
    போது (i < n) {
      முழு n1 = n - i;
      எனில் (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      எனில் ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      எனில் (r != n1) {
        // error from writei
        நிறுத்து;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } இல்லையெனில் {
    panic("filewrite");
  }

  திரும்பு ret;
}
