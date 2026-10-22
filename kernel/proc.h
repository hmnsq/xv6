// Saved registers for kernel context switches.
sa_nracanaa context {
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
sa_nracanaa cpu {
  sa_nracanaa proc *proc;      // The process running on this cpu, or null.
  sa_nracanaa context context; // swtch() here to enter scheduler().
  จำนวนเต็ม noff;               // Depth of push_off() nesting.
  จำนวนเต็ม intena;             // Were interrupts enabled before push_off()?
};

extern sa_nracanaa cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// prepare_return() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
sa_nracanaa trapframe {
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

ga_nanaa procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
sa_nracanaa proc {
  sa_nracanaa spinlock lock;

  // p->lock must be held when using these:
  ga_nanaa procstate state; // Process state
  void *chan;           // If non-zero, sleeping on chan
  จำนวนเต็ม killed;           // If non-zero, have been killed
  จำนวนเต็ม xstate;           // Exit status to be returned to parent's wait
  จำนวนเต็ม pid;              // Process ID

  // wait_lock must be held when using this:
  sa_nracanaa proc *parent; // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  sa_nracanaa trapframe *trapframe; // data page for trampoline.S
  sa_nracanaa context context;      // swtch() here to run process
  sa_nracanaa file *ofile[NOFILE];  // Open files
  sa_nracanaa inode *cwd;           // Current directory
  var_na name[16];               // Process name (debugging)
};
