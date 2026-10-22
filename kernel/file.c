//
// Support functions for system calls that involve file descriptors.
//

#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "stat.h"
#अन्तर्भाव "proc.h"

संरचना devsw devsw[NDEV];
संरचना {
  संरचना spinlock lock;
  संरचना file file[NFILE];
} ftable;

शून्य
fileinit(शून्य)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
संरचना file *
filealloc(शून्य)
{
  संरचना file *f;

  acquire(&ftable.lock);
  साठी (f = ftable.file; f < ftable.file + NFILE; f++) {
    जर (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      परत f;
    }
  }
  release(&ftable.lock);
  परत 0;
}

// Increment ref count for file f.
संरचना file *
filedup(संरचना file *f)
{
  acquire(&ftable.lock);
  जर (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  परत f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
शून्य
fileclose(संरचना file *f)
{
  संरचना file ff;

  acquire(&ftable.lock);
  जर (f->ref < 1)
    panic("fileclose");
  जर (--f->ref > 0) {
    release(&ftable.lock);
    परत;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  जर (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } अन्यथा जर (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
पूर्णांक
filestat(संरचना file *f, uint64 addr)
{
  संरचना proc *p = myproc();
  संरचना stat st;

  जर (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    जर (copyout(p->pagetable, p->sz, addr, (अक्षर *)&st, आकार(st)) < 0)
      परत -1;
    परत 0;
  }
  परत -1;
}

// Read from file f.
// addr is a user virtual address.
पूर्णांक
fileread(संरचना file *f, uint64 addr, पूर्णांक n)
{
  पूर्णांक r = 0;

  जर (f->readable == 0 || n < 0)
    परत -1;

  जर (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } अन्यथा जर (f->type == FD_DEVICE) {
    जर (f->major < 0 || f->major >= NDEV || !devsw[f->major].पठ)
      परत -1;
    r = devsw[f->major].पठ(1, addr, n);
  } अन्यथा जर (f->type == FD_INODE) {
    ilock(f->ip);
    जर ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } अन्यथा {
    panic("fileread");
  }

  परत r;
}

// Write to file f.
// addr is a user virtual address.
पूर्णांक
filewrite(संरचना file *f, uint64 addr, पूर्णांक n)
{
  पूर्णांक r, ret = 0;

  जर (f->writable == 0 || n < 0)
    परत -1;

  जर (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } अन्यथा जर (f->type == FD_DEVICE) {
    जर (f->major < 0 || f->major >= NDEV || !devsw[f->major].लिख)
      परत -1;
    ret = devsw[f->major].लिख(1, addr, n);
  } अन्यथा जर (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    पूर्णांक max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    पूर्णांक i = 0;
    पर्यंत (i < n) {
      पूर्णांक n1 = n - i;
      जर (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      जर ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      जर (r != n1) {
        // error from writei
        थांबा;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } अन्यथा {
    panic("filewrite");
  }

  परत ret;
}
