//
// Support functions for system calls that involve file descriptors.
//

#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "fs.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "file.h"
#समावेश "stat.h"
#समावेश "proc.h"

काष्ठा devsw devsw[NDEV];
काष्ठा {
  काष्ठा spinlock lock;
  काष्ठा file file[NFILE];
} ftable;

व्योम
fileinit(व्योम)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
काष्ठा file *
filealloc(व्योम)
{
  काष्ठा file *f;

  acquire(&ftable.lock);
  क्रम (f = ftable.file; f < ftable.file + NFILE; f++) {
    अगर (f->ref == 0) {
      f->ref = 1;
      release(&ftable.lock);
      वापस f;
    }
  }
  release(&ftable.lock);
  वापस 0;
}

// Increment ref count for file f.
काष्ठा file *
filedup(काष्ठा file *f)
{
  acquire(&ftable.lock);
  अगर (f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  वापस f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
व्योम
fileclose(काष्ठा file *f)
{
  काष्ठा file ff;

  acquire(&ftable.lock);
  अगर (f->ref < 1)
    panic("fileclose");
  अगर (--f->ref > 0) {
    release(&ftable.lock);
    वापस;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);

  अगर (ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  } अन्यथा अगर (ff.type == FD_INODE || ff.type == FD_DEVICE) {
    begin_op();
    iput(ff.ip);
    end_op();
  }
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
पूर्णांक
filestat(काष्ठा file *f, uint64 addr)
{
  काष्ठा proc *p = myproc();
  काष्ठा stat st;

  अगर (f->type == FD_INODE || f->type == FD_DEVICE) {
    ilock(f->ip);
    stati(f->ip, &st);
    iunlock(f->ip);
    अगर (copyout(p->pagetable, p->sz, addr, (अक्षर *)&st, माप(st)) < 0)
      वापस -1;
    वापस 0;
  }
  वापस -1;
}

// Read from file f.
// addr is a user virtual address.
पूर्णांक
fileread(काष्ठा file *f, uint64 addr, पूर्णांक n)
{
  पूर्णांक r = 0;

  अगर (f->readable == 0 || n < 0)
    वापस -1;

  अगर (f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } अन्यथा अगर (f->type == FD_DEVICE) {
    अगर (f->major < 0 || f->major >= NDEV || !devsw[f->major].पढ़ो)
      वापस -1;
    r = devsw[f->major].पढ़ो(1, addr, n);
  } अन्यथा अगर (f->type == FD_INODE) {
    ilock(f->ip);
    अगर ((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } अन्यथा {
    panic("fileread");
  }

  वापस r;
}

// Write to file f.
// addr is a user virtual address.
पूर्णांक
filewrite(काष्ठा file *f, uint64 addr, पूर्णांक n)
{
  पूर्णांक r, ret = 0;

  अगर (f->writable == 0 || n < 0)
    वापस -1;

  अगर (f->type == FD_PIPE) {
    ret = pipewrite(f->pipe, addr, n);
  } अन्यथा अगर (f->type == FD_DEVICE) {
    अगर (f->major < 0 || f->major >= NDEV || !devsw[f->major].ग_लिखो)
      वापस -1;
    ret = devsw[f->major].ग_लिखो(1, addr, n);
  } अन्यथा अगर (f->type == FD_INODE) {
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    पूर्णांक max = ((MAXOPBLOCKS - 1 - 1 - 2) / 2) * BSIZE;
    पूर्णांक i = 0;
    जबतक (i < n) {
      पूर्णांक n1 = n - i;
      अगर (n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      अगर ((r = writei(f->ip, 1, addr + i, f->off, n1)) > 0)
        f->off += r;
      iunlock(f->ip);
      end_op();

      अगर (r != n1) {
        // error from writei
        अवरोध;
      }
      i += r;
    }
    ret = (i == n ? n : -1);
  } अन्यथा {
    panic("filewrite");
  }

  वापस ret;
}
