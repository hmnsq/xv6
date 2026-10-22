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
  संरचना spinlock तालकम्;
  संरचना file file[NFILE];
} ftable;

शून्य
fileinit(शून्य)
{
  initlock(&ftable.तालकम्, "ftable");
}

// Allocate a file structure.
संरचना file *
filealloc(शून्य)
{
  संरचना file *f;

  acquire(&ftable.तालकम्);
  प्रति (f = ftable.file; f < ftable.file + NFILE; f++) {
    यदि (f->ref == 0) {
      f->ref = 1;
      release(&ftable.तालकम्);
      प्रत्यागम f;
    }
  }
  release(&ftable.तालकम्);
  प्रत्यागम 0;
}

// Increment ref count for file f.
संरचना file *
filedup(संरचना file *f)
{
  acquire(&ftable.तालकम्);
  यदि (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.तालकम्);
  प्रत्यागम f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
शून्य
fileclose(संरचना file *f)
{
  संरचना file ff;

  acquire(&ftable.तालकम्);
  यदि (f->ref < 1)
    panic("fileclose");
  यदि (--f->ref > 0) {
    release(&ftable.तालकम्);
    प्रत्यागम;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.तालकम्);

  यदि (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } अन्यथा यदि (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    अन्तः_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
पूर्णाङ्क
filestat(संरचना file *f, uint64 addr)
{
  संरचना proc *p = myproc();
  संरचना stat st;

  यदि (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    यदि (copyout(p->pagetable, p->sz, addr, (वर्ण *)&st, परिमाण(st)) < 0)
      प्रत्यागम -1;
    प्रत्यागम 0;
  }
  प्रत्यागम -1;
}

// Read from file f.
// addr is a user virtual address.
पूर्णाङ्क
fileread(संरचना file *f, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क r = 0;

  यदि (f->readable == 0 || n < 0)
    प्रत्यागम -1;

  यदि (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } अन्यथा यदि (f->type == FD_DEVICE) {
    यदि (f->major < 0 || f->major >= NDEV || !devsw[f->major].पठ)
      प्रत्यागम -1;
    r = devsw[f->major].पठ(1, addr, n);
  } अन्यथा यदि (f->type == FD_INODE) {
    ilock(f->ip);
    यदि ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } अन्यथा {
    panic("fileread");
  }

  प्रत्यागम r;
}

// Write to file f.
// addr is a user virtual address.
पूर्णाङ्क
filewrite(संरचना file *f, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क r, ret = 0;

  यदि (f->writable == 0 || n < 0)
    प्रत्यागम -1;

  यदि (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } अन्यथा यदि (f->type == FD_DEVICE) {
    यदि (f->major < 0 || f->major >= NDEV || !devsw[f->major].लिख)
      प्रत्यागम -1;
    ret = devsw[f->major].लिख(1, addr, n);
  } अन्यथा यदि (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    पूर्णाङ्क max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    पूर्णाङ्क i = 0;
    यावत् (i < n) {
      पूर्णाङ्क n1 = n - i;
      यदि (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      यदि ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      अन्तः_op();

      यदि (r != n1) {
        // error from writei
        भङ्ग;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } अन्यथा {
    panic("filewrite");
  }

  प्रत्यागम ret;
}
