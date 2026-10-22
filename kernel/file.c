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
  अत्थाय (f = ftable.file; f < ftable.file + NFILE; f++) {
    सचे (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      पटिनिवत्त f;
    }
  }
  release(&ftable.lock);
  पटिनिवत्त 0;
}

// Increment ref count for file f.
संरचना file *
filedup(संरचना file *f)
{
  acquire(&ftable.lock);
  सचे (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  पटिनिवत्त f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
शून्य
fileclose(संरचना file *f)
{
  संरचना file ff;

  acquire(&ftable.lock);
  सचे (f->ref < 1)
    panic("fileclose");
  सचे (--f->ref > 0) {
    release(&ftable.lock);
    पटिनिवत्त;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  सचे (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } अञ्ञथा सचे (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
पूर्णाङ्क
filestat(संरचना file *f, uint64 addr)
{
  संरचना proc *p = myproc();
  संरचना stat st;

  सचे (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    सचे (copyout(p->pagetable, p->sz, addr, (वर्ण *)&st, परिमाण(st)) < 0)
      पटिनिवत्त -1;
    पटिनिवत्त 0;
  }
  पटिनिवत्त -1;
}

// Read from file f.
// addr is a user virtual address.
पूर्णाङ्क
fileread(संरचना file *f, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क r = 0;

  सचे (f->readable == 0 || n < 0)
    पटिनिवत्त -1;

  सचे (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } अञ्ञथा सचे (f->type == FD_DEVICE) {
    सचे (f->major < 0 || f->major >= NDEV || !devsw[f->major].पठ)
      पटिनिवत्त -1;
    r = devsw[f->major].पठ(1, addr, n);
  } अञ्ञथा सचे (f->type == FD_INODE) {
    ilock(f->ip);
    सचे ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } अञ्ञथा {
    panic("fileread");
  }

  पटिनिवत्त r;
}

// Write to file f.
// addr is a user virtual address.
पूर्णाङ्क
filewrite(संरचना file *f, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क r, ret = 0;

  सचे (f->writable == 0 || n < 0)
    पटिनिवत्त -1;

  सचे (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } अञ्ञथा सचे (f->type == FD_DEVICE) {
    सचे (f->major < 0 || f->major >= NDEV || !devsw[f->major].लिख)
      पटिनिवत्त -1;
    ret = devsw[f->major].लिख(1, addr, n);
  } अञ्ञथा सचे (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    पूर्णाङ्क max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    पूर्णाङ्क i = 0;
    याव (i < n) {
      पूर्णाङ्क n1 = n - i;
      सचे (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      सचे ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      सचे (r != n1) {
        // error from writei
        भङ्ग;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } अञ्ञथा {
    panic("filewrite");
  }

  पटिनिवत्त ret;
}
