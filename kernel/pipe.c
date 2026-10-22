#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "file.h"

#ವ್ಯಾಖ್ಯಾನಿಸು PIPESIZE 512

ರಚನೆ pipe {
  ರಚನೆ spinlock ಬೀಗ;
  ಅಕ್ಷರ ದತ್ತಾಂಶ[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  ಪೂರ್ಣಾಂಕ readopen;  // read fd is still open
  ಪೂರ್ಣಾಂಕ writeopen; // write fd is still open
};

ಪೂರ್ಣಾಂಕ
pipealloc(ರಚನೆ file **f0, ರಚನೆ file **f1)
{
  ರಚನೆ pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ಆದರೆ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    ಹೋಗು bad;
  ಆದರೆ ((pi = (ರಚನೆ pipe *)kalloc()) == 0)
    ಹೋಗು bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->ಬೀಗ, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  ಹಿಂತಿರುಗಿ 0;

bad:
  ಆದರೆ (pi)
    kfree((ಅಕ್ಷರ *)pi);
  ಆದರೆ (*f0)
    fileclose(*f0);
  ಆದರೆ (*f1)
    fileclose(*f1);
  ಹಿಂತಿರುಗಿ -1;
}

ಶೂನ್ಯ
pipeclose(ರಚನೆ pipe *pi, ಪೂರ್ಣಾಂಕ writable)
{
  acquire(&pi->ಬೀಗ);
  ಆದರೆ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ಇಲ್ಲದಿದ್ದರೆ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ಆದರೆ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->ಬೀಗ);
    kfree((ಅಕ್ಷರ *)pi);
  } ಇಲ್ಲದಿದ್ದರೆ
    release(&pi->ಬೀಗ);
}

ಪೂರ್ಣಾಂಕ
pipewrite(ರಚನೆ pipe *pi, uint64 addr, ಪೂರ್ಣಾಂಕ n)
{
  ಪೂರ್ಣಾಂಕ i = 0;
  ರಚನೆ proc *pr = myproc();

  acquire(&pi->ಬೀಗ);
  ಆಗ (i < n) {
    ಆದರೆ (pi->readopen == 0 || killed(pr)) {
      release(&pi->ಬೀಗ);
      ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->ಬೀಗ);
      sleep();
      acquire(&pi->ಬೀಗ);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಅಕ್ಷರ ch;
      ಆದರೆ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ಆದರೆ (i == 0)
          i = -1;
        ನಿಲ್ಲಿಸು;
      }
      pi->ದತ್ತಾಂಶ[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->ಬೀಗ);

  ಹಿಂತಿರುಗಿ i;
}

ಪೂರ್ಣಾಂಕ
piperead(ರಚನೆ pipe *pi, uint64 addr, ಪೂರ್ಣಾಂಕ n)
{
  ಪೂರ್ಣಾಂಕ i;
  ರಚನೆ proc *pr = myproc();
  ಅಕ್ಷರ ch;

  acquire(&pi->ಬೀಗ);
  ಆಗ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ಆದರೆ (killed(pr)) {
      release(&pi->ಬೀಗ);
      ಹಿಂತಿರುಗಿ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->ಬೀಗ);
    sleep();
    acquire(&pi->ಬೀಗ);
  }
  ಗಾಗಿ (i = 0; i < n; i++) { //DOC: piperead-copy
    ಆದರೆ (pi->nread == pi->nwrite)
      ನಿಲ್ಲಿಸು;
    ch = pi->ದತ್ತಾಂಶ[pi->nread % PIPESIZE];
    ಆದರೆ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ಆದರೆ (i == 0)
        i = -1;
      ನಿಲ್ಲಿಸು;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->ಬೀಗ);
  ಹಿಂತಿರುಗಿ i;
}
