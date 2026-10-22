// clang-format off
ರಚನೆ buf;
ರಚನೆ context;
ರಚನೆ file;
ರಚನೆ inode;
ರಚನೆ pipe;
ರಚನೆ proc;
ರಚನೆ spinlock;
ರಚನೆ sleeplock;
ರಚನೆ stat;
ರಚನೆ superblock;

// bio.c
ಶೂನ್ಯ            binit(ಶೂನ್ಯ);
ರಚನೆ buf*     bread(uint, uint);
ಶೂನ್ಯ            brelse(ರಚನೆ buf*);
ಶೂನ್ಯ            bwrite(ರಚನೆ buf*);
ಶೂನ್ಯ            bpin(ರಚನೆ buf*);
ಶೂನ್ಯ            bunpin(ರಚನೆ buf*);

// console.c
ಶೂನ್ಯ            consoleinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            consoleintr(ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            consputc(ಪೂರ್ಣಾಂಕ);

// exec.c
ಪೂರ್ಣಾಂಕ             kexec(ಅಕ್ಷರ*, ಅಕ್ಷರ**);

// file.c
ರಚನೆ file*    filealloc(ಶೂನ್ಯ);
ಶೂನ್ಯ            fileclose(ರಚನೆ file*);
ರಚನೆ file*    filedup(ರಚನೆ file*);
ಶೂನ್ಯ            fileinit(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ             fileread(ರಚನೆ file*, uint64, ಪೂರ್ಣಾಂಕ n);
ಪೂರ್ಣಾಂಕ             filestat(ರಚನೆ file*, uint64 addr);
ಪೂರ್ಣಾಂಕ             filewrite(ರಚನೆ file*, uint64, ಪೂರ್ಣಾಂಕ n);

// fs.c
ಶೂನ್ಯ            fsinit(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             dirlink(ರಚನೆ inode*, ಅಕ್ಷರ*, uint);
ರಚನೆ inode*   dirlookup(ರಚನೆ inode*, ಅಕ್ಷರ*, uint*);
ರಚನೆ inode*   ialloc(uint, ಗಿಡ್ಡ);
ರಚನೆ inode*   idup(ರಚನೆ inode*);
ಶೂನ್ಯ            iinit();
ಶೂನ್ಯ            ilock(ರಚನೆ inode*);
ಶೂನ್ಯ            iput(ರಚನೆ inode*);
ಶೂನ್ಯ            iunlock(ರಚನೆ inode*);
ಶೂನ್ಯ            iunlockput(ರಚನೆ inode*);
ಶೂನ್ಯ            iupdate(ರಚನೆ inode*);
ಪೂರ್ಣಾಂಕ             namecmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ರಚನೆ inode*   namei(ಅಕ್ಷರ*);
ರಚನೆ inode*   nameiparent(ಅಕ್ಷರ*, ಅಕ್ಷರ*);
ಪೂರ್ಣಾಂಕ             readi(ರಚನೆ inode*, ಪೂರ್ಣಾಂಕ, uint64, uint, uint);
ಶೂನ್ಯ            stati(ರಚನೆ inode*, ರಚನೆ stat*);
ಪೂರ್ಣಾಂಕ             writei(ರಚನೆ inode*, ಪೂರ್ಣಾಂಕ, uint64, uint, uint);
ಶೂನ್ಯ            itrunc(ರಚನೆ inode*);
ಶೂನ್ಯ            ireclaim(ಪೂರ್ಣಾಂಕ);

// kalloc.c
ಶೂನ್ಯ*           kalloc(ಶೂನ್ಯ);
ಶೂನ್ಯ            kfree(ಶೂನ್ಯ *);
ಶೂನ್ಯ            kinit(ಶೂನ್ಯ);

// log.c
ಶೂನ್ಯ            initlog(ಪೂರ್ಣಾಂಕ, ರಚನೆ superblock*);
ಶೂನ್ಯ            log_write(ರಚನೆ buf*);
ಶೂನ್ಯ            begin_op(ಶೂನ್ಯ);
ಶೂನ್ಯ            ಕೊನೆ_op(ಶೂನ್ಯ);

// pipe.c
ಪೂರ್ಣಾಂಕ             pipealloc(ರಚನೆ file**, ರಚನೆ file**);
ಶೂನ್ಯ            pipeclose(ರಚನೆ pipe*, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             piperead(ರಚನೆ pipe*, uint64, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             pipewrite(ರಚನೆ pipe*, uint64, ಪೂರ್ಣಾಂಕ);

// printk.c
ಪೂರ್ಣಾಂಕ             printk(ಅಕ್ಷರ*, ...) __attribute__ ((format (ಬರೆ, 1, 2)));
ಶೂನ್ಯ            panic(ಅಕ್ಷರ*) __attribute__((noreturn));
ಶೂನ್ಯ            printkinit(ಶೂನ್ಯ);

// proc.c
ಪೂರ್ಣಾಂಕ             cpuid(ಶೂನ್ಯ);
ಶೂನ್ಯ            kexit(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             kfork(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ             growproc(ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ರಚನೆ proc *);
ಶೂನ್ಯ            proc_freepagetable(pagetable_t, uint64);
ಪೂರ್ಣಾಂಕ             kkill(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             killed(ರಚನೆ proc*);
ಶೂನ್ಯ            setkilled(ರಚನೆ proc*);
ರಚನೆ cpu*     mycpu(ಶೂನ್ಯ);
ರಚನೆ proc*    myproc();
ಶೂನ್ಯ            procinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            scheduler(ಶೂನ್ಯ) __attribute__((noreturn));
ಶೂನ್ಯ            sched(ಶೂನ್ಯ);
ಶೂನ್ಯ            sleep_prepare(ಶೂನ್ಯ*);
ಶೂನ್ಯ            sleep(ಶೂನ್ಯ);
ಶೂನ್ಯ            userinit(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ             kwait(uint64);
ಶೂನ್ಯ            wakeup(ಶೂನ್ಯ*);
ಶೂನ್ಯ            yield(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ             either_copyout(ಪೂರ್ಣಾಂಕ user_dst, uint64 dst, ಶೂನ್ಯ *src, uint64 len);
ಪೂರ್ಣಾಂಕ             either_copyin(ಶೂನ್ಯ *dst, ಪೂರ್ಣಾಂಕ user_src, uint64 src, uint64 len);
ಶೂನ್ಯ            procdump(ಶೂನ್ಯ);

// swtch.S
ಶೂನ್ಯ            swtch(ರಚನೆ context*, ರಚನೆ context*);

// spinlock.c
ಶೂನ್ಯ            acquire(ರಚನೆ spinlock*);
ಪೂರ್ಣಾಂಕ             holding(ರಚನೆ spinlock*);
ಶೂನ್ಯ            initlock(ರಚನೆ spinlock*, ಅಕ್ಷರ*);
ಶೂನ್ಯ            release(ರಚನೆ spinlock*);
ಶೂನ್ಯ            ತಳ್ಳು_off(ಶೂನ್ಯ);
ಶೂನ್ಯ            ತೆಗೆ_off(ಶೂನ್ಯ);

// sleeplock.c
ಶೂನ್ಯ            acquiresleep(ರಚನೆ sleeplock*);
ಶೂನ್ಯ            releasesleep(ರಚನೆ sleeplock*);
ಪೂರ್ಣಾಂಕ             holdingsleep(ರಚನೆ sleeplock*);
ಶೂನ್ಯ            initsleeplock(ರಚನೆ sleeplock*, ಅಕ್ಷರ*);

// string.c
ಪೂರ್ಣಾಂಕ             memcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*, uint);
ಶೂನ್ಯ*           memmove(ಶೂನ್ಯ*, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*, uint);
ಶೂನ್ಯ*           memset(ಶೂನ್ಯ*, ಪೂರ್ಣಾಂಕ, uint);
ಅಕ್ಷರ*           safestrcpy(ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಪೂರ್ಣಾಂಕ             ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, uint);
ಅಕ್ಷರ*           strncpy(ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ);

// syscall.c
ಶೂನ್ಯ            argint(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ*);
ಪೂರ್ಣಾಂಕ             argstr(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            argaddr(ಪೂರ್ಣಾಂಕ, uint64 *);
ಪೂರ್ಣಾಂಕ             fetchstr(uint64, ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             fetchaddr(uint64, uint64*);
ಶೂನ್ಯ            syscall();

// trap.c
ಬಾಹ್ಯ uint     ticks;
ಶೂನ್ಯ            trapinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            trapinithart(ಶೂನ್ಯ);
ಬಾಹ್ಯ ರಚನೆ spinlock tickslock;
ಶೂನ್ಯ            prepare_return(ಶೂನ್ಯ);

// uart.c
ಶೂನ್ಯ            uartinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            uartintr(ಶೂನ್ಯ);
ಶೂನ್ಯ            uartwrite(ಅಕ್ಷರ [], ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            uartputc_sync(ಪೂರ್ಣಾಂಕ);

// vm.c
ಶೂನ್ಯ            kvminit(ಶೂನ್ಯ);
ಶೂನ್ಯ            kvminithart(ಶೂನ್ಯ);
ಶೂನ್ಯ            kvmmap(pagetable_t, uint64, uint64, uint64, ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ             mappages(pagetable_t, uint64, uint64, uint64, ಪೂರ್ಣಾಂಕ);
pagetable_t     uvmcreate(ಶೂನ್ಯ);
uint64          uvmalloc(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಂಕ);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ಪೂರ್ಣಾಂಕ             uvmcopy(pagetable_t, pagetable_t, uint64);
ಶೂನ್ಯ            uvmfree(pagetable_t, uint64);
ಶೂನ್ಯ            uvmunmap(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ಪೂರ್ಣಾಂಕ);
uint64          walkaddr(pagetable_t, uint64);
ಪೂರ್ಣಾಂಕ             copyout(pagetable_t, uint64, uint64, ಅಕ್ಷರ *, uint64);
ಪೂರ್ಣಾಂಕ             copyin(pagetable_t, uint64, ಅಕ್ಷರ *, uint64, uint64);
ಪೂರ್ಣಾಂಕ             copyinstr(pagetable_t, uint64, ಅಕ್ಷರ *, uint64, uint64);
ಪೂರ್ಣಾಂಕ             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಂಕ);

// plic.c
ಶೂನ್ಯ            plicinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            plicinithart(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ             plic_claim(ಶೂನ್ಯ);
ಶೂನ್ಯ            plic_complete(ಪೂರ್ಣಾಂಕ);

// virtio_disk.c
ಶೂನ್ಯ            virtio_disk_init(ಶೂನ್ಯ);
ಶೂನ್ಯ            virtio_disk_rw(ರಚನೆ buf *, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ            virtio_disk_intr(ಶೂನ್ಯ);

// number of elements in fixed-size array
#ವ್ಯಾಖ್ಯಾನಿಸು NELEM(x) (ಗಾತ್ರ(x) / ಗಾತ್ರ((x)[0]))
