#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "defs.h"

संरचना spinlock tickslock;
uint ticks;

बाह्य वर्ण trampoline[], uservec[];

// in kernelvec.S, calls kerneltrap().
शून्य kernelvec();

बाह्य पूर्णाङ्क devintr();

शून्य
trapinit(शून्य)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
शून्य
trapinithart(शून्य)
{
  w_stvec((uint64)kernelvec);
}

//
// handle an interrupt, exception, or system call from user space.
// called from, and returns to, trampoline.S
// return value is user satp for trampoline.S to switch to.
//
uint64
usertrap(शून्य)
{
  पूर्णाङ्क which_dev = 0;

  यदि ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec); //DOC: kernelvec

  संरचना proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  यदि (r_scause() == 8) {
    // system call

    यदि (killed(p))
      kexit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } अन्यथा यदि ((which_dev = devintr()) != 0) {
    // ok
  } अन्यथा यदि ((r_scause() == 15 || r_scause() == 13) &&
             vmfault(p->pagetable, p->sz, r_stval(),
                     (r_scause() == 13) ? 1 : 0) != 0) {
    // page fault on lazily-allocated page
  } अन्यथा {
    printk("usertrap(): unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printk("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  यदि (killed(p))
    kexit(-1);

  // give up the CPU if this is a timer interrupt.
  यदि (which_dev == 2)
    yield();

  prepare_return();

  // the user page table to switch to, for trampoline.S
  uint64 satp = MAKE_SATP(p->pagetable);

  // return to trampoline.S; satp value in a0.
  प्रत्यागम satp;
}

//
// set up trapframe and control registers for a return to user space
//
शून्य
prepare_return(शून्य)
{
  संरचना proc *p = myproc();

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
  अचिह्नित दीर्घ x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
शून्य
kerneltrap()
{
  पूर्णाङ्क which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  यदि ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  यदि (intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  यदि ((which_dev = devintr()) == 0) {
    // interrupt or trap from an unknown source
    printk("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(),
           r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  यदि (which_dev == 2 && myproc() != 0)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

शून्य
clockintr()
{
  यदि (cpuid() == 0) {
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
पूर्णाङ्क
devintr()
{
  uint64 scause = r_scause();

  यदि (scause == 0x8000000000000009L) {
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    पूर्णाङ्क irq = plic_claim();

    यदि (irq == UART0_IRQ) {
      uartintr();
    } अन्यथा यदि (irq == VIRTIO0_IRQ) {
      virtio_disk_intr();
    } अन्यथा यदि (irq) {
      printk("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    यदि (irq)
      plic_complete(irq);

    प्रत्यागम 1;
  } अन्यथा यदि (scause == 0x8000000000000005L) {
    // timer interrupt.
    clockintr();
    प्रत्यागम 2;
  } अन्यथा {
    प्रत्यागम 0;
  }
}
