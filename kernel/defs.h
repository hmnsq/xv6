// clang-format off
నిర్మాణం buf;
నిర్మాణం context;
నిర్మాణం file;
నిర్మాణం inode;
నిర్మాణం pipe;
నిర్మాణం proc;
నిర్మాణం spinlock;
నిర్మాణం sleeplock;
నిర్మాణం stat;
నిర్మాణం superblock;

// bio.c
శూన్య            binit(శూన్య);
నిర్మాణం buf*     bread(uint, uint);
శూన్య            brelse(నిర్మాణం buf*);
శూన్య            bwrite(నిర్మాణం buf*);
శూన్య            bpin(నిర్మాణం buf*);
శూన్య            bunpin(నిర్మాణం buf*);

// console.c
శూన్య            consoleinit(శూన్య);
శూన్య            consoleintr(పూర్ణాంకం);
శూన్య            consputc(పూర్ణాంకం);

// exec.c
పూర్ణాంకం             kexec(అక్షరం*, అక్షరం**);

// file.c
నిర్మాణం file*    filealloc(శూన్య);
శూన్య            fileclose(నిర్మాణం file*);
నిర్మాణం file*    filedup(నిర్మాణం file*);
శూన్య            fileinit(శూన్య);
పూర్ణాంకం             fileread(నిర్మాణం file*, uint64, పూర్ణాంకం n);
పూర్ణాంకం             filestat(నిర్మాణం file*, uint64 addr);
పూర్ణాంకం             filewrite(నిర్మాణం file*, uint64, పూర్ణాంకం n);

// fs.c
శూన్య            fsinit(పూర్ణాంకం);
పూర్ణాంకం             dirlink(నిర్మాణం inode*, అక్షరం*, uint);
నిర్మాణం inode*   dirlookup(నిర్మాణం inode*, అక్షరం*, uint*);
నిర్మాణం inode*   ialloc(uint, పొట్టి);
నిర్మాణం inode*   idup(నిర్మాణం inode*);
శూన్య            iinit();
శూన్య            ilock(నిర్మాణం inode*);
శూన్య            iput(నిర్మాణం inode*);
శూన్య            iunlock(నిర్మాణం inode*);
శూన్య            iunlockput(నిర్మాణం inode*);
శూన్య            iupdate(నిర్మాణం inode*);
పూర్ణాంకం             namecmp(స్థిరరాశి అక్షరం*, స్థిరరాశి అక్షరం*);
నిర్మాణం inode*   namei(అక్షరం*);
నిర్మాణం inode*   nameiparent(అక్షరం*, అక్షరం*);
పూర్ణాంకం             readi(నిర్మాణం inode*, పూర్ణాంకం, uint64, uint, uint);
శూన్య            stati(నిర్మాణం inode*, నిర్మాణం stat*);
పూర్ణాంకం             writei(నిర్మాణం inode*, పూర్ణాంకం, uint64, uint, uint);
శూన్య            itrunc(నిర్మాణం inode*);
శూన్య            ireclaim(పూర్ణాంకం);

// kalloc.c
శూన్య*           kalloc(శూన్య);
శూన్య            kfree(శూన్య *);
శూన్య            kinit(శూన్య);

// log.c
శూన్య            initlog(పూర్ణాంకం, నిర్మాణం superblock*);
శూన్య            log_write(నిర్మాణం buf*);
శూన్య            begin_op(శూన్య);
శూన్య            చివర_op(శూన్య);

// pipe.c
పూర్ణాంకం             pipealloc(నిర్మాణం file**, నిర్మాణం file**);
శూన్య            pipeclose(నిర్మాణం pipe*, పూర్ణాంకం);
పూర్ణాంకం             piperead(నిర్మాణం pipe*, uint64, పూర్ణాంకం);
పూర్ణాంకం             pipewrite(నిర్మాణం pipe*, uint64, పూర్ణాంకం);

// printk.c
పూర్ణాంకం             printk(అక్షరం*, ...) __attribute__ ((format (రాయి, 1, 2)));
శూన్య            panic(అక్షరం*) __attribute__((noreturn));
శూన్య            printkinit(శూన్య);

// proc.c
పూర్ణాంకం             cpuid(శూన్య);
శూన్య            kexit(పూర్ణాంకం);
పూర్ణాంకం             kfork(శూన్య);
పూర్ణాంకం             growproc(పూర్ణాంకం);
శూన్య            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(నిర్మాణం proc *);
శూన్య            proc_freepagetable(pagetable_t, uint64);
పూర్ణాంకం             kkill(పూర్ణాంకం);
పూర్ణాంకం             killed(నిర్మాణం proc*);
శూన్య            setkilled(నిర్మాణం proc*);
నిర్మాణం cpu*     mycpu(శూన్య);
నిర్మాణం proc*    myproc();
శూన్య            procinit(శూన్య);
శూన్య            scheduler(శూన్య) __attribute__((noreturn));
శూన్య            sched(శూన్య);
శూన్య            sleep_prepare(శూన్య*);
శూన్య            sleep(శూన్య);
శూన్య            userinit(శూన్య);
పూర్ణాంకం             kwait(uint64);
శూన్య            wakeup(శూన్య*);
శూన్య            yield(శూన్య);
పూర్ణాంకం             either_copyout(పూర్ణాంకం user_dst, uint64 dst, శూన్య *src, uint64 len);
పూర్ణాంకం             either_copyin(శూన్య *dst, పూర్ణాంకం user_src, uint64 src, uint64 len);
శూన్య            procdump(శూన్య);

// swtch.S
శూన్య            swtch(నిర్మాణం context*, నిర్మాణం context*);

// spinlock.c
శూన్య            acquire(నిర్మాణం spinlock*);
పూర్ణాంకం             holding(నిర్మాణం spinlock*);
శూన్య            initlock(నిర్మాణం spinlock*, అక్షరం*);
శూన్య            release(నిర్మాణం spinlock*);
శూన్య            నెట్టు_off(శూన్య);
శూన్య            తీయి_off(శూన్య);

// sleeplock.c
శూన్య            acquiresleep(నిర్మాణం sleeplock*);
శూన్య            releasesleep(నిర్మాణం sleeplock*);
పూర్ణాంకం             holdingsleep(నిర్మాణం sleeplock*);
శూన్య            initsleeplock(నిర్మాణం sleeplock*, అక్షరం*);

// string.c
పూర్ణాంకం             memcmp(స్థిరరాశి శూన్య*, స్థిరరాశి శూన్య*, uint);
శూన్య*           memmove(శూన్య*, స్థిరరాశి శూన్య*, uint);
శూన్య*           memset(శూన్య*, పూర్ణాంకం, uint);
అక్షరం*           safestrcpy(అక్షరం*, స్థిరరాశి అక్షరం*, పూర్ణాంకం);
పూర్ణాంకం             అక్షరశ్రేణి_పొడవు(స్థిరరాశి అక్షరం*);
పూర్ణాంకం             అక్షరశ్రేణి_పోలిక(స్థిరరాశి అక్షరం*, స్థిరరాశి అక్షరం*, uint);
అక్షరం*           strncpy(అక్షరం*, స్థిరరాశి అక్షరం*, పూర్ణాంకం);

// syscall.c
శూన్య            argint(పూర్ణాంకం, పూర్ణాంకం*);
పూర్ణాంకం             argstr(పూర్ణాంకం, అక్షరం*, పూర్ణాంకం);
శూన్య            argaddr(పూర్ణాంకం, uint64 *);
పూర్ణాంకం             fetchstr(uint64, అక్షరం*, పూర్ణాంకం);
పూర్ణాంకం             fetchaddr(uint64, uint64*);
శూన్య            syscall();

// trap.c
బాహ్య uint     ticks;
శూన్య            trapinit(శూన్య);
శూన్య            trapinithart(శూన్య);
బాహ్య నిర్మాణం spinlock tickslock;
శూన్య            prepare_return(శూన్య);

// uart.c
శూన్య            uartinit(శూన్య);
శూన్య            uartintr(శూన్య);
శూన్య            uartwrite(అక్షరం [], పూర్ణాంకం);
శూన్య            uartputc_sync(పూర్ణాంకం);

// vm.c
శూన్య            kvminit(శూన్య);
శూన్య            kvminithart(శూన్య);
శూన్య            kvmmap(pagetable_t, uint64, uint64, uint64, పూర్ణాంకం);
పూర్ణాంకం             mappages(pagetable_t, uint64, uint64, uint64, పూర్ణాంకం);
pagetable_t     uvmcreate(శూన్య);
uint64          uvmalloc(pagetable_t, uint64, uint64, పూర్ణాంకం);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
పూర్ణాంకం             uvmcopy(pagetable_t, pagetable_t, uint64);
శూన్య            uvmfree(pagetable_t, uint64);
శూన్య            uvmunmap(pagetable_t, uint64, uint64, పూర్ణాంకం);
శూన్య            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, పూర్ణాంకం);
uint64          walkaddr(pagetable_t, uint64);
పూర్ణాంకం             copyout(pagetable_t, uint64, uint64, అక్షరం *, uint64);
పూర్ణాంకం             copyin(pagetable_t, uint64, అక్షరం *, uint64, uint64);
పూర్ణాంకం             copyinstr(pagetable_t, uint64, అక్షరం *, uint64, uint64);
పూర్ణాంకం             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, పూర్ణాంకం);

// plic.c
శూన్య            plicinit(శూన్య);
శూన్య            plicinithart(శూన్య);
పూర్ణాంకం             plic_claim(శూన్య);
శూన్య            plic_complete(పూర్ణాంకం);

// virtio_disk.c
శూన్య            virtio_disk_init(శూన్య);
శూన్య            virtio_disk_rw(నిర్మాణం buf *, పూర్ణాంకం);
శూన్య            virtio_disk_intr(శూన్య);

// number of elements in fixed-size array
#నిర్వచించు NELEM(x) (పరిమాణం(x) / పరిమాణం((x)[0]))
