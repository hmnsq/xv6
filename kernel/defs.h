// clang-format off
ಸಂರಚನಾ buf;
ಸಂರಚನಾ context;
ಸಂರಚನಾ file;
ಸಂರಚನಾ inode;
ಸಂರಚನಾ pipe;
ಸಂರಚನಾ proc;
ಸಂರಚನಾ spinlock;
ಸಂರಚನಾ sleeplock;
ಸಂರಚನಾ stat;
ಸಂರಚನಾ superblock;

// bio.c
ಶೂನ್ಯ            binit(ಶೂನ್ಯ);
ಸಂರಚನಾ buf*     bread(uint, uint);
ಶೂನ್ಯ            brelse(ಸಂರಚನಾ buf*);
ಶೂನ್ಯ            bwrite(ಸಂರಚನಾ buf*);
ಶೂನ್ಯ            bpin(ಸಂರಚನಾ buf*);
ಶೂನ್ಯ            bunpin(ಸಂರಚನಾ buf*);

// console.c
ಶೂನ್ಯ            consoleinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            consoleintr(ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            consputc(ಪೂರ್ಣಾಙ್ಕ);

// exec.c
ಪೂರ್ಣಾಙ್ಕ             kexec(ವರ್ಣ*, ವರ್ಣ**);

// file.c
ಸಂರಚನಾ file*    filealloc(ಶೂನ್ಯ);
ಶೂನ್ಯ            fileclose(ಸಂರಚನಾ file*);
ಸಂರಚನಾ file*    filedup(ಸಂರಚನಾ file*);
ಶೂನ್ಯ            fileinit(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ             fileread(ಸಂರಚನಾ file*, uint64, ಪೂರ್ಣಾಙ್ಕ n);
ಪೂರ್ಣಾಙ್ಕ             filestat(ಸಂರಚನಾ file*, uint64 addr);
ಪೂರ್ಣಾಙ್ಕ             filewrite(ಸಂರಚನಾ file*, uint64, ಪೂರ್ಣಾಙ್ಕ n);

// fs.c
ಶೂನ್ಯ            fsinit(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             dirlink(ಸಂರಚನಾ inode*, ವರ್ಣ*, uint);
ಸಂರಚನಾ inode*   dirlookup(ಸಂರಚನಾ inode*, ವರ್ಣ*, uint*);
ಸಂರಚನಾ inode*   ialloc(uint, short);
ಸಂರಚನಾ inode*   idup(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            iinit();
ಶೂನ್ಯ            ilock(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            iput(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            iunlock(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            iunlockput(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            iupdate(ಸಂರಚನಾ inode*);
ಪೂರ್ಣಾಙ್ಕ             namecmp(ಧ್ರುವ ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*);
ಸಂರಚನಾ inode*   namei(ವರ್ಣ*);
ಸಂರಚನಾ inode*   nameiparent(ವರ್ಣ*, ವರ್ಣ*);
ಪೂರ್ಣಾಙ್ಕ             readi(ಸಂರಚನಾ inode*, ಪೂರ್ಣಾಙ್ಕ, uint64, uint, uint);
ಶೂನ್ಯ            stati(ಸಂರಚನಾ inode*, ಸಂರಚನಾ stat*);
ಪೂರ್ಣಾಙ್ಕ             writei(ಸಂರಚನಾ inode*, ಪೂರ್ಣಾಙ್ಕ, uint64, uint, uint);
ಶೂನ್ಯ            itrunc(ಸಂರಚನಾ inode*);
ಶೂನ್ಯ            ireclaim(ಪೂರ್ಣಾಙ್ಕ);

// kalloc.c
ಶೂನ್ಯ*           kalloc(ಶೂನ್ಯ);
ಶೂನ್ಯ            kfree(ಶೂನ್ಯ *);
ಶೂನ್ಯ            kinit(ಶೂನ್ಯ);

// log.c
ಶೂನ್ಯ            initlog(ಪೂರ್ಣಾಙ್ಕ, ಸಂರಚನಾ superblock*);
ಶೂನ್ಯ            log_write(ಸಂರಚನಾ buf*);
ಶೂನ್ಯ            begin_op(ಶೂನ್ಯ);
ಶೂನ್ಯ            end_op(ಶೂನ್ಯ);

// pipe.c
ಪೂರ್ಣಾಙ್ಕ             pipealloc(ಸಂರಚನಾ file**, ಸಂರಚನಾ file**);
ಶೂನ್ಯ            pipeclose(ಸಂರಚನಾ pipe*, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             piperead(ಸಂರಚನಾ pipe*, uint64, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             pipewrite(ಸಂರಚನಾ pipe*, uint64, ಪೂರ್ಣಾಙ್ಕ);

// printk.c
ಪೂರ್ಣಾಙ್ಕ             printk(ವರ್ಣ*, ...) __attribute__ ((format (printf, 1, 2)));
ಶೂನ್ಯ            panic(ವರ್ಣ*) __attribute__((noreturn));
ಶೂನ್ಯ            printkinit(ಶೂನ್ಯ);

// proc.c
ಪೂರ್ಣಾಙ್ಕ             cpuid(ಶೂನ್ಯ);
ಶೂನ್ಯ            kexit(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             kfork(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ             growproc(ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ಸಂರಚನಾ proc *);
ಶೂನ್ಯ            proc_freepagetable(pagetable_t, uint64);
ಪೂರ್ಣಾಙ್ಕ             kkill(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             killed(ಸಂರಚನಾ proc*);
ಶೂನ್ಯ            setkilled(ಸಂರಚನಾ proc*);
ಸಂರಚನಾ cpu*     mycpu(ಶೂನ್ಯ);
ಸಂರಚನಾ proc*    myproc();
ಶೂನ್ಯ            procinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            scheduler(ಶೂನ್ಯ) __attribute__((noreturn));
ಶೂನ್ಯ            sched(ಶೂನ್ಯ);
ಶೂನ್ಯ            sleep_prepare(ಶೂನ್ಯ*);
ಶೂನ್ಯ            sleep(ಶೂನ್ಯ);
ಶೂನ್ಯ            userinit(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ             kwait(uint64);
ಶೂನ್ಯ            wakeup(ಶೂನ್ಯ*);
ಶೂನ್ಯ            yield(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ             either_copyout(ಪೂರ್ಣಾಙ್ಕ user_dst, uint64 dst, ಶೂನ್ಯ *src, uint64 len);
ಪೂರ್ಣಾಙ್ಕ             either_copyin(ಶೂನ್ಯ *dst, ಪೂರ್ಣಾಙ್ಕ user_src, uint64 src, uint64 len);
ಶೂನ್ಯ            procdump(ಶೂನ್ಯ);

// swtch.S
ಶೂನ್ಯ            swtch(ಸಂರಚನಾ context*, ಸಂರಚನಾ context*);

// spinlock.c
ಶೂನ್ಯ            acquire(ಸಂರಚನಾ spinlock*);
ಪೂರ್ಣಾಙ್ಕ             holding(ಸಂರಚನಾ spinlock*);
ಶೂನ್ಯ            initlock(ಸಂರಚನಾ spinlock*, ವರ್ಣ*);
ಶೂನ್ಯ            release(ಸಂರಚನಾ spinlock*);
ಶೂನ್ಯ            ತಳ್ಳು_off(ಶೂನ್ಯ);
ಶೂನ್ಯ            ತೆಗೆ_off(ಶೂನ್ಯ);

// sleeplock.c
ಶೂನ್ಯ            acquiresleep(ಸಂರಚನಾ sleeplock*);
ಶೂನ್ಯ            releasesleep(ಸಂರಚನಾ sleeplock*);
ಪೂರ್ಣಾಙ್ಕ             holdingsleep(ಸಂರಚನಾ sleeplock*);
ಶೂನ್ಯ            initsleeplock(ಸಂರಚನಾ sleeplock*, ವರ್ಣ*);

// string.c
ಪೂರ್ಣಾಙ್ಕ             memcmp(ಧ್ರುವ ಶೂನ್ಯ*, ಧ್ರುವ ಶೂನ್ಯ*, uint);
ಶೂನ್ಯ*           memmove(ಶೂನ್ಯ*, ಧ್ರುವ ಶೂನ್ಯ*, uint);
ಶೂನ್ಯ*           memset(ಶೂನ್ಯ*, ಪೂರ್ಣಾಙ್ಕ, uint);
ವರ್ಣ*           safestrcpy(ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             strlen(ಧ್ರುವ ವರ್ಣ*);
ಪೂರ್ಣಾಙ್ಕ             strncmp(ಧ್ರುವ ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*, uint);
ವರ್ಣ*           strncpy(ವರ್ಣ*, ಧ್ರುವ ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ);

// syscall.c
ಶೂನ್ಯ            argint(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ*);
ಪೂರ್ಣಾಙ್ಕ             argstr(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            argaddr(ಪೂರ್ಣಾಙ್ಕ, uint64 *);
ಪೂರ್ಣಾಙ್ಕ             fetchstr(uint64, ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             fetchaddr(uint64, uint64*);
ಶೂನ್ಯ            syscall();

// trap.c
ಬಾಹ್ಯ uint     ticks;
ಶೂನ್ಯ            trapinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            trapinithart(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಸಂರಚನಾ spinlock tickslock;
ಶೂನ್ಯ            prepare_return(ಶೂನ್ಯ);

// uart.c
ಶೂನ್ಯ            uartinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            uartintr(ಶೂನ್ಯ);
ಶೂನ್ಯ            uartwrite(ವರ್ಣ [], ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            uartputc_sync(ಪೂರ್ಣಾಙ್ಕ);

// vm.c
ಶೂನ್ಯ            kvminit(ಶೂನ್ಯ);
ಶೂನ್ಯ            kvminithart(ಶೂನ್ಯ);
ಶೂನ್ಯ            kvmmap(pagetable_t, uint64, uint64, uint64, ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ             mappages(pagetable_t, uint64, uint64, uint64, ಪೂರ್ಣಾಙ್ಕ);
pagetable_t     uvmcreate(ಶೂನ್ಯ);
uint64          uvmalloc(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಙ್ಕ);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ಪೂರ್ಣಾಙ್ಕ             uvmcopy(pagetable_t, pagetable_t, uint64);
ಶೂನ್ಯ            uvmfree(pagetable_t, uint64);
ಶೂನ್ಯ            uvmunmap(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ಪೂರ್ಣಾಙ್ಕ);
uint64          walkaddr(pagetable_t, uint64);
ಪೂರ್ಣಾಙ್ಕ             copyout(pagetable_t, uint64, uint64, ವರ್ಣ *, uint64);
ಪೂರ್ಣಾಙ್ಕ             copyin(pagetable_t, uint64, ವರ್ಣ *, uint64, uint64);
ಪೂರ್ಣಾಙ್ಕ             copyinstr(pagetable_t, uint64, ವರ್ಣ *, uint64, uint64);
ಪೂರ್ಣಾಙ್ಕ             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ಪೂರ್ಣಾಙ್ಕ);

// plic.c
ಶೂನ್ಯ            plicinit(ಶೂನ್ಯ);
ಶೂನ್ಯ            plicinithart(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ             plic_claim(ಶೂನ್ಯ);
ಶೂನ್ಯ            plic_complete(ಪೂರ್ಣಾಙ್ಕ);

// virtio_disk.c
ಶೂನ್ಯ            virtio_disk_init(ಶೂನ್ಯ);
ಶೂನ್ಯ            virtio_disk_rw(ಸಂರಚನಾ buf *, ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ            virtio_disk_intr(ಶೂನ್ಯ);

// number of elements in fixed-size array
#define NELEM(x) (ಪರಿಮಾಣ(x) / ಪರಿಮಾಣ((x)[0]))
