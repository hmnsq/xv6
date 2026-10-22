// clang-format off
ସଂରଚନା buf;
ସଂରଚନା context;
ସଂରଚନା file;
ସଂରଚନା inode;
ସଂରଚନା pipe;
ସଂରଚନା proc;
ସଂରଚନା spinlock;
ସଂରଚନା sleeplock;
ସଂରଚନା stat;
ସଂରଚନା superblock;

// bio.c
ଶୂନ୍ୟ            binit(ଶୂନ୍ୟ);
ସଂରଚନା buf*     bread(uint, uint);
ଶୂନ୍ୟ            brelse(ସଂରଚନା buf*);
ଶୂନ୍ୟ            bwrite(ସଂରଚନା buf*);
ଶୂନ୍ୟ            bpin(ସଂରଚନା buf*);
ଶୂନ୍ୟ            bunpin(ସଂରଚନା buf*);

// console.c
ଶୂନ୍ୟ            consoleinit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            consoleintr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            consputc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// exec.c
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             kexec(ଅକ୍ଷର*, ଅକ୍ଷର**);

// file.c
ସଂରଚନା file*    filealloc(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            fileclose(ସଂରଚନା file*);
ସଂରଚନା file*    filedup(ସଂରଚନା file*);
ଶୂନ୍ୟ            fileinit(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             fileread(ସଂରଚନା file*, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             filestat(ସଂରଚନା file*, uint64 addr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             filewrite(ସଂରଚନା file*, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);

// fs.c
ଶୂନ୍ୟ            fsinit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             dirlink(ସଂରଚନା inode*, ଅକ୍ଷର*, uint);
ସଂରଚନା inode*   dirlookup(ସଂରଚନା inode*, ଅକ୍ଷର*, uint*);
ସଂରଚନା inode*   ialloc(uint, short);
ସଂରଚନା inode*   idup(ସଂରଚନା inode*);
ଶୂନ୍ୟ            iinit();
ଶୂନ୍ୟ            ilock(ସଂରଚନା inode*);
ଶୂନ୍ୟ            iput(ସଂରଚନା inode*);
ଶୂନ୍ୟ            iunlock(ସଂରଚନା inode*);
ଶୂନ୍ୟ            iunlockput(ସଂରଚନା inode*);
ଶୂନ୍ୟ            iupdate(ସଂରଚନା inode*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             namecmp(ଅଚଳ ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*);
ସଂରଚନା inode*   namei(ଅକ୍ଷର*);
ସଂରଚନା inode*   nameiparent(ଅକ୍ଷର*, ଅକ୍ଷର*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             readi(ସଂରଚନା inode*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint64, uint, uint);
ଶୂନ୍ୟ            stati(ସଂରଚନା inode*, ସଂରଚନା stat*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             writei(ସଂରଚନା inode*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint64, uint, uint);
ଶୂନ୍ୟ            itrunc(ସଂରଚନା inode*);
ଶୂନ୍ୟ            ireclaim(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// kalloc.c
ଶୂନ୍ୟ*           kalloc(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            kfree(ଶୂନ୍ୟ *);
ଶୂନ୍ୟ            kinit(ଶୂନ୍ୟ);

// log.c
ଶୂନ୍ୟ            initlog(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ସଂରଚନା superblock*);
ଶୂନ୍ୟ            log_write(ସଂରଚନା buf*);
ଶୂନ୍ୟ            begin_op(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            end_op(ଶୂନ୍ୟ);

// pipe.c
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             pipealloc(ସଂରଚନା file**, ସଂରଚନା file**);
ଶୂନ୍ୟ            pipeclose(ସଂରଚନା pipe*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             piperead(ସଂରଚନା pipe*, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             pipewrite(ସଂରଚନା pipe*, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// printk.c
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             printk(ଅକ୍ଷର*, ...) __attribute__ ((format (printf, 1, 2)));
ଶୂନ୍ୟ            panic(ଅକ୍ଷର*) __attribute__((noreturn));
ଶୂନ୍ୟ            printkinit(ଶୂନ୍ୟ);

// proc.c
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             cpuid(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            kexit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             kfork(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             growproc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ସଂରଚନା proc *);
ଶୂନ୍ୟ            proc_freepagetable(pagetable_t, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             kkill(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             killed(ସଂରଚନା proc*);
ଶୂନ୍ୟ            setkilled(ସଂରଚନା proc*);
ସଂରଚନା cpu*     mycpu(ଶୂନ୍ୟ);
ସଂରଚନା proc*    myproc();
ଶୂନ୍ୟ            procinit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            scheduler(ଶୂନ୍ୟ) __attribute__((noreturn));
ଶୂନ୍ୟ            sched(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            sleep_prepare(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ            sleep(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            userinit(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             kwait(uint64);
ଶୂନ୍ୟ            wakeup(ଶୂନ୍ୟ*);
ଶୂନ୍ୟ            yield(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             either_copyout(ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_dst, uint64 dst, ଶୂନ୍ୟ *src, uint64 len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             either_copyin(ଶୂନ୍ୟ *dst, ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_src, uint64 src, uint64 len);
ଶୂନ୍ୟ            procdump(ଶୂନ୍ୟ);

// swtch.S
ଶୂନ୍ୟ            swtch(ସଂରଚନା context*, ସଂରଚନା context*);

// spinlock.c
ଶୂନ୍ୟ            acquire(ସଂରଚନା spinlock*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             holding(ସଂରଚନା spinlock*);
ଶୂନ୍ୟ            initlock(ସଂରଚନା spinlock*, ଅକ୍ଷର*);
ଶୂନ୍ୟ            release(ସଂରଚନା spinlock*);
ଶୂନ୍ୟ            ଠେଲ_off(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            କାଢ଼_off(ଶୂନ୍ୟ);

// sleeplock.c
ଶୂନ୍ୟ            acquiresleep(ସଂରଚନା sleeplock*);
ଶୂନ୍ୟ            releasesleep(ସଂରଚନା sleeplock*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             holdingsleep(ସଂରଚନା sleeplock*);
ଶୂନ୍ୟ            initsleeplock(ସଂରଚନା sleeplock*, ଅକ୍ଷର*);

// string.c
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             memcmp(ଅଚଳ ଶୂନ୍ୟ*, ଅଚଳ ଶୂନ୍ୟ*, uint);
ଶୂନ୍ୟ*           memmove(ଶୂନ୍ୟ*, ଅଚଳ ଶୂନ୍ୟ*, uint);
ଶୂନ୍ୟ*           memset(ଶୂନ୍ୟ*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint);
ଅକ୍ଷର*           safestrcpy(ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             strlen(ଅଚଳ ଅକ୍ଷର*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             strncmp(ଅଚଳ ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*, uint);
ଅକ୍ଷର*           strncpy(ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// syscall.c
ଶୂନ୍ୟ            argint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା*);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             argstr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            argaddr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint64 *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             fetchstr(uint64, ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             fetchaddr(uint64, uint64*);
ଶୂନ୍ୟ            syscall();

// trap.c
ବାହ୍ୟ uint     ticks;
ଶୂନ୍ୟ            trapinit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            trapinithart(ଶୂନ୍ୟ);
ବାହ୍ୟ ସଂରଚନା spinlock tickslock;
ଶୂନ୍ୟ            prepare_return(ଶୂନ୍ୟ);

// uart.c
ଶୂନ୍ୟ            uartinit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            uartintr(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            uartwrite(ଅକ୍ଷର [], ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            uartputc_sync(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// vm.c
ଶୂନ୍ୟ            kvminit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            kvminithart(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            kvmmap(pagetable_t, uint64, uint64, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             mappages(pagetable_t, uint64, uint64, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
pagetable_t     uvmcreate(ଶୂନ୍ୟ);
uint64          uvmalloc(pagetable_t, uint64, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             uvmcopy(pagetable_t, pagetable_t, uint64);
ଶୂନ୍ୟ            uvmfree(pagetable_t, uint64);
ଶୂନ୍ୟ            uvmunmap(pagetable_t, uint64, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
uint64          walkaddr(pagetable_t, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             copyout(pagetable_t, uint64, uint64, ଅକ୍ଷର *, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             copyin(pagetable_t, uint64, ଅକ୍ଷର *, uint64, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             copyinstr(pagetable_t, uint64, ଅକ୍ଷର *, uint64, uint64);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// plic.c
ଶୂନ୍ୟ            plicinit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            plicinithart(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା             plic_claim(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            plic_complete(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

// virtio_disk.c
ଶୂନ୍ୟ            virtio_disk_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ            virtio_disk_rw(ସଂରଚନା buf *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ            virtio_disk_intr(ଶୂନ୍ୟ);

// number of elements in fixed-size array
#define NELEM(x) (ଆକାର(x) / ଆକାର((x)[0]))
