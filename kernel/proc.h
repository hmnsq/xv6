// Saved registers for kernel context switches.
నిర్మాణం context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
నిర్మాణం cpu {
  నిర్మాణం proc *proc;      // The process running on this cpu, or null.
  నిర్మాణం context context; // swtch() here to enter scheduler().
  పూర్ణాంకం noff;               // Depth of push_off() nesting.
  పూర్ణాంకం intena;             // Were interrupts enabled before push_off()?
};

బాహ్య నిర్మాణం cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// prepare_return() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
నిర్మాణం trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

గణన procstate { వాడని, వాడిన, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
నిర్మాణం proc {
  నిర్మాణం spinlock తాళం;

  // p->lock must be held when using these:
  గణన procstate state; // Process state
  శూన్య *chan;           // If non-zero, sleeping on chan
  పూర్ణాంకం killed;           // If non-zero, have been killed
  పూర్ణాంకం xstate;           // Exit status to be returned to parent's wait
  పూర్ణాంకం pid;              // Process ID

  // wait_lock must be held when using this:
  నిర్మాణం proc *మాతృ; // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  నిర్మాణం trapframe *trapframe; // data page for trampoline.S
  నిర్మాణం context context;      // swtch() here to run process
  నిర్మాణం file *ofile[NOFILE];  // Open files
  నిర్మాణం inode *cwd;           // Current directory
  అక్షరం name[16];               // Process name (debugging)
};
