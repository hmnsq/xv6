#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "defs.h"

काष्ठा spinlock tickslock;
uint ticks;

बाह्य अक्षर trampoline[], uservec[];

// in kernelvec.S, calls kerneltrap().
व्योम kernelvec();

बाह्य पूर्णांक devintr();

व्योम
trapinit(व्योम)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
व्योम
trapinithart(व्योम)
{
  w_stvec((uint64)kernelvec);
}

//
// handle an interrupt, exception, or system call from user space.
// called from, and returns to, trampoline.S
// return value is user satp for trampoline.S to switch to.
//
uint64
usertrap(व्योम)
{
  पूर्णांक which_dev = 0;

  अगर ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec); //DOC: kernelvec

  काष्ठा proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  अगर (r_scause() == 8) {
    // system call

    अगर (killed(p))
      kexit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } अन्यथा अगर ((which_dev = devintr()) != 0) {
    // ok
  } अन्यथा अगर ((r_scause() == 15 || r_scause() == 13) &&
             vmfault(p->pagetable, p->sz, r_stval(),
                     (r_scause() == 13) ? 1 : 0) != 0) {
    // page fault on lazily-allocated page
  } अन्यथा {
    printk("usertrap(): unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printk("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  अगर (killed(p))
    kexit(-1);

  // give up the CPU if this is a timer interrupt.
  अगर (which_dev == 2)
    yield();

  prepare_return();

  // the user page table to switch to, for trampoline.S
  uint64 satp = MAKE_SATP(p->pagetable);

  // return to trampoline.S; satp value in a0.
  वापस satp;
}

//
// set up trapframe and control registers for a return to user space
//
व्योम
prepare_return(व्योम)
{
  काष्ठा proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(). because a trap from kernel
  // code to usertrap would be a disaster, turn off interrupts.
  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp(); // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.

  // set S Previous Privilege mode to User.
  अचिन्हित दीर्घ x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
व्योम
kerneltrap()
{
  पूर्णांक which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  अगर ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  अगर (intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  अगर ((which_dev = devintr()) == 0) {
    // interrupt or trap from an unknown source
    printk("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(),
           r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  अगर (which_dev == 2 && myproc() != 0)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

व्योम
clockintr()
{
  अगर (cpuid() == 0) {
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  // ask for the next timer interrupt. this also clears
  // the interrupt request. 1000000 is about a tenth
  // of a second.
  w_stimecmp(r_time() + 1000000);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
पूर्णांक
devintr()
{
  uint64 scause = r_scause();

  अगर (scause == 0x8000000000000009L) {
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    पूर्णांक irq = plic_claim();

    अगर (irq == UART0_IRQ) {
      uartintr();
    } अन्यथा अगर (irq == VIRTIO0_IRQ) {
      virtio_disk_intr();
    } अन्यथा अगर (irq) {
      printk("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    अगर (irq)
      plic_complete(irq);

    वापस 1;
  } अन्यथा अगर (scause == 0x8000000000000005L) {
    // timer interrupt.
    clockintr();
    वापस 2;
  } अन्यथा {
    वापस 0;
  }
}
