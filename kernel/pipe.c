#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "fs.h"
#شامل "sleeplock.h"
#شامل "file.h"

#تعریف_کرو PIPESIZE 512

ساخت pipe {
  ساخت spinlock تالا;
  حرف ڈیٹا[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  صحیح readopen;  // read fd is still open
  صحیح writeopen; // write fd is still open
};

صحیح
pipealloc(ساخت file **f0, ساخت file **f1)
{
  ساخت pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  اگر ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    جاؤ bad;
  اگر ((pi = (ساخت pipe *)kalloc()) == 0)
    جاؤ bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->تالا, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  واپس 0;

bad:
  اگر (pi)
    kfree((حرف *)pi);
  اگر (*f0)
    fileclose(*f0);
  اگر (*f1)
    fileclose(*f1);
  واپس -1;
}

خالی
pipeclose(ساخت pipe *pi, صحیح writable)
{
  acquire(&pi->تالا);
  اگر (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ورنہ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  اگر (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->تالا);
    kfree((حرف *)pi);
  } ورنہ
    release(&pi->تالا);
}

صحیح
pipewrite(ساخت pipe *pi, uint64 addr, صحیح n)
{
  صحیح i = 0;
  ساخت proc *pr = myproc();

  acquire(&pi->تالا);
  جب (i < n) {
    اگر (pi->readopen == 0 || killed(pr)) {
      release(&pi->تالا);
      واپس -1;
    }
    اگر (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->تالا);
      sleep();
      acquire(&pi->تالا);
    } ورنہ {
      حرف ch;
      اگر (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        اگر (i == 0)
          i = -1;
        توڑو;
      }
      pi->ڈیٹا[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->تالا);

  واپس i;
}

صحیح
piperead(ساخت pipe *pi, uint64 addr, صحیح n)
{
  صحیح i;
  ساخت proc *pr = myproc();
  حرف ch;

  acquire(&pi->تالا);
  جب (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    اگر (killed(pr)) {
      release(&pi->تالا);
      واپس -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->تالا);
    sleep();
    acquire(&pi->تالا);
  }
  برائے (i = 0; i < n; i++) { //DOC: piperead-copy
    اگر (pi->nread == pi->nwrite)
      توڑو;
    ch = pi->ڈیٹا[pi->nread % PIPESIZE];
    اگر (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      اگر (i == 0)
        i = -1;
      توڑو;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->تالا);
  واپس i;
}
