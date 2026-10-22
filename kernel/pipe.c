#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "fs.h"
#समावेश "sleeplock.h"
#समावेश "file.h"

#घोषणा PIPESIZE 512

काष्ठा pipe {
  काष्ठा spinlock lock;
  अक्षर तथ्य[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  पूर्णांक readopen;  // read fd is still open
  पूर्णांक writeopen; // write fd is still open
};

पूर्णांक
pipealloc(काष्ठा file **f0, काष्ठा file **f1)
{
  काष्ठा pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  अगर ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    जाओ bad;
  अगर ((pi = (काष्ठा pipe *)kalloc()) == 0)
    जाओ bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  वापस 0;

bad:
  अगर (pi)
    kfree((अक्षर *)pi);
  अगर (*f0)
    fileclose(*f0);
  अगर (*f1)
    fileclose(*f1);
  वापस -1;
}

व्योम
pipeclose(काष्ठा pipe *pi, पूर्णांक writable)
{
  acquire(&pi->lock);
  अगर (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अन्यथा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  अगर (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((अक्षर *)pi);
  } अन्यथा
    release(&pi->lock);
}

पूर्णांक
pipewrite(काष्ठा pipe *pi, uint64 addr, पूर्णांक n)
{
  पूर्णांक i = 0;
  काष्ठा proc *pr = myproc();

  acquire(&pi->lock);
  जबतक (i < n) {
    अगर (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      वापस -1;
    }
    अगर (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } अन्यथा {
      अक्षर ch;
      अगर (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        अगर (i == 0)
          i = -1;
        अवरोध;
      }
      pi->तथ्य[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  वापस i;
}

पूर्णांक
piperead(काष्ठा pipe *pi, uint64 addr, पूर्णांक n)
{
  पूर्णांक i;
  काष्ठा proc *pr = myproc();
  अक्षर ch;

  acquire(&pi->lock);
  जबतक (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    अगर (killed(pr)) {
      release(&pi->lock);
      वापस -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  क्रम (i = 0; i < n; i++) { //DOC: piperead-copy
    अगर (pi->nread == pi->nwrite)
      अवरोध;
    ch = pi->तथ्य[pi->nread % PIPESIZE];
    अगर (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      अगर (i == 0)
        i = -1;
      अवरोध;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  वापस i;
}
