// clang-format off
ਬਣਤਰ buf;
ਬਣਤਰ context;
ਬਣਤਰ file;
ਬਣਤਰ inode;
ਬਣਤਰ pipe;
ਬਣਤਰ proc;
ਬਣਤਰ spinlock;
ਬਣਤਰ sleeplock;
ਬਣਤਰ stat;
ਬਣਤਰ superblock;

// bio.c
ਖਾਲੀ            binit(ਖਾਲੀ);
ਬਣਤਰ buf*     bread(uint, uint);
ਖਾਲੀ            brelse(ਬਣਤਰ buf*);
ਖਾਲੀ            bwrite(ਬਣਤਰ buf*);
ਖਾਲੀ            bpin(ਬਣਤਰ buf*);
ਖਾਲੀ            bunpin(ਬਣਤਰ buf*);

// console.c
ਖਾਲੀ            consoleinit(ਖਾਲੀ);
ਖਾਲੀ            consoleintr(ਪੂਰਨ);
ਖਾਲੀ            consputc(ਪੂਰਨ);

// exec.c
ਪੂਰਨ             kexec(ਅੱਖਰ*, ਅੱਖਰ**);

// file.c
ਬਣਤਰ file*    filealloc(ਖਾਲੀ);
ਖਾਲੀ            fileclose(ਬਣਤਰ file*);
ਬਣਤਰ file*    filedup(ਬਣਤਰ file*);
ਖਾਲੀ            fileinit(ਖਾਲੀ);
ਪੂਰਨ             fileread(ਬਣਤਰ file*, uint64, ਪੂਰਨ n);
ਪੂਰਨ             filestat(ਬਣਤਰ file*, uint64 addr);
ਪੂਰਨ             filewrite(ਬਣਤਰ file*, uint64, ਪੂਰਨ n);

// fs.c
ਖਾਲੀ            fsinit(ਪੂਰਨ);
ਪੂਰਨ             dirlink(ਬਣਤਰ inode*, ਅੱਖਰ*, uint);
ਬਣਤਰ inode*   dirlookup(ਬਣਤਰ inode*, ਅੱਖਰ*, uint*);
ਬਣਤਰ inode*   ialloc(uint, ਛੋਟਾ);
ਬਣਤਰ inode*   idup(ਬਣਤਰ inode*);
ਖਾਲੀ            iinit();
ਖਾਲੀ            ilock(ਬਣਤਰ inode*);
ਖਾਲੀ            iput(ਬਣਤਰ inode*);
ਖਾਲੀ            iunlock(ਬਣਤਰ inode*);
ਖਾਲੀ            iunlockput(ਬਣਤਰ inode*);
ਖਾਲੀ            iupdate(ਬਣਤਰ inode*);
ਪੂਰਨ             namecmp(ਅਚਲ ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*);
ਬਣਤਰ inode*   namei(ਅੱਖਰ*);
ਬਣਤਰ inode*   nameiparent(ਅੱਖਰ*, ਅੱਖਰ*);
ਪੂਰਨ             readi(ਬਣਤਰ inode*, ਪੂਰਨ, uint64, uint, uint);
ਖਾਲੀ            stati(ਬਣਤਰ inode*, ਬਣਤਰ stat*);
ਪੂਰਨ             writei(ਬਣਤਰ inode*, ਪੂਰਨ, uint64, uint, uint);
ਖਾਲੀ            itrunc(ਬਣਤਰ inode*);
ਖਾਲੀ            ireclaim(ਪੂਰਨ);

// kalloc.c
ਖਾਲੀ*           kalloc(ਖਾਲੀ);
ਖਾਲੀ            kfree(ਖਾਲੀ *);
ਖਾਲੀ            kinit(ਖਾਲੀ);

// log.c
ਖਾਲੀ            initlog(ਪੂਰਨ, ਬਣਤਰ superblock*);
ਖਾਲੀ            log_write(ਬਣਤਰ buf*);
ਖਾਲੀ            begin_op(ਖਾਲੀ);
ਖਾਲੀ            ਸਿਰਾ_op(ਖਾਲੀ);

// pipe.c
ਪੂਰਨ             pipealloc(ਬਣਤਰ file**, ਬਣਤਰ file**);
ਖਾਲੀ            pipeclose(ਬਣਤਰ pipe*, ਪੂਰਨ);
ਪੂਰਨ             piperead(ਬਣਤਰ pipe*, uint64, ਪੂਰਨ);
ਪੂਰਨ             pipewrite(ਬਣਤਰ pipe*, uint64, ਪੂਰਨ);

// printk.c
ਪੂਰਨ             printk(ਅੱਖਰ*, ...) __attribute__ ((format (ਲਿਖੋ, 1, 2)));
ਖਾਲੀ            panic(ਅੱਖਰ*) __attribute__((noreturn));
ਖਾਲੀ            printkinit(ਖਾਲੀ);

// proc.c
ਪੂਰਨ             cpuid(ਖਾਲੀ);
ਖਾਲੀ            kexit(ਪੂਰਨ);
ਪੂਰਨ             kfork(ਖਾਲੀ);
ਪੂਰਨ             growproc(ਪੂਰਨ);
ਖਾਲੀ            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ਬਣਤਰ proc *);
ਖਾਲੀ            proc_freepagetable(pagetable_t, uint64);
ਪੂਰਨ             kkill(ਪੂਰਨ);
ਪੂਰਨ             killed(ਬਣਤਰ proc*);
ਖਾਲੀ            setkilled(ਬਣਤਰ proc*);
ਬਣਤਰ cpu*     mycpu(ਖਾਲੀ);
ਬਣਤਰ proc*    myproc();
ਖਾਲੀ            procinit(ਖਾਲੀ);
ਖਾਲੀ            scheduler(ਖਾਲੀ) __attribute__((noreturn));
ਖਾਲੀ            sched(ਖਾਲੀ);
ਖਾਲੀ            sleep_prepare(ਖਾਲੀ*);
ਖਾਲੀ            sleep(ਖਾਲੀ);
ਖਾਲੀ            userinit(ਖਾਲੀ);
ਪੂਰਨ             kwait(uint64);
ਖਾਲੀ            wakeup(ਖਾਲੀ*);
ਖਾਲੀ            yield(ਖਾਲੀ);
ਪੂਰਨ             either_copyout(ਪੂਰਨ user_dst, uint64 dst, ਖਾਲੀ *src, uint64 len);
ਪੂਰਨ             either_copyin(ਖਾਲੀ *dst, ਪੂਰਨ user_src, uint64 src, uint64 len);
ਖਾਲੀ            procdump(ਖਾਲੀ);

// swtch.S
ਖਾਲੀ            swtch(ਬਣਤਰ context*, ਬਣਤਰ context*);

// spinlock.c
ਖਾਲੀ            acquire(ਬਣਤਰ spinlock*);
ਪੂਰਨ             holding(ਬਣਤਰ spinlock*);
ਖਾਲੀ            initlock(ਬਣਤਰ spinlock*, ਅੱਖਰ*);
ਖਾਲੀ            release(ਬਣਤਰ spinlock*);
ਖਾਲੀ            ਧੱਕੋ_off(ਖਾਲੀ);
ਖਾਲੀ            ਕੱਢੋ_off(ਖਾਲੀ);

// sleeplock.c
ਖਾਲੀ            acquiresleep(ਬਣਤਰ sleeplock*);
ਖਾਲੀ            releasesleep(ਬਣਤਰ sleeplock*);
ਪੂਰਨ             holdingsleep(ਬਣਤਰ sleeplock*);
ਖਾਲੀ            initsleeplock(ਬਣਤਰ sleeplock*, ਅੱਖਰ*);

// string.c
ਪੂਰਨ             memcmp(ਅਚਲ ਖਾਲੀ*, ਅਚਲ ਖਾਲੀ*, uint);
ਖਾਲੀ*           memmove(ਖਾਲੀ*, ਅਚਲ ਖਾਲੀ*, uint);
ਖਾਲੀ*           memset(ਖਾਲੀ*, ਪੂਰਨ, uint);
ਅੱਖਰ*           safestrcpy(ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*, ਪੂਰਨ);
ਪੂਰਨ             ਲੜੀ_ਲੰਬਾਈ(ਅਚਲ ਅੱਖਰ*);
ਪੂਰਨ             ਲੜੀ_ਤੁਲਨਾ(ਅਚਲ ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*, uint);
ਅੱਖਰ*           strncpy(ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*, ਪੂਰਨ);

// syscall.c
ਖਾਲੀ            argint(ਪੂਰਨ, ਪੂਰਨ*);
ਪੂਰਨ             argstr(ਪੂਰਨ, ਅੱਖਰ*, ਪੂਰਨ);
ਖਾਲੀ            argaddr(ਪੂਰਨ, uint64 *);
ਪੂਰਨ             fetchstr(uint64, ਅੱਖਰ*, ਪੂਰਨ);
ਪੂਰਨ             fetchaddr(uint64, uint64*);
ਖਾਲੀ            syscall();

// trap.c
ਬਾਹਰੀ uint     ticks;
ਖਾਲੀ            trapinit(ਖਾਲੀ);
ਖਾਲੀ            trapinithart(ਖਾਲੀ);
ਬਾਹਰੀ ਬਣਤਰ spinlock tickslock;
ਖਾਲੀ            prepare_return(ਖਾਲੀ);

// uart.c
ਖਾਲੀ            uartinit(ਖਾਲੀ);
ਖਾਲੀ            uartintr(ਖਾਲੀ);
ਖਾਲੀ            uartwrite(ਅੱਖਰ [], ਪੂਰਨ);
ਖਾਲੀ            uartputc_sync(ਪੂਰਨ);

// vm.c
ਖਾਲੀ            kvminit(ਖਾਲੀ);
ਖਾਲੀ            kvminithart(ਖਾਲੀ);
ਖਾਲੀ            kvmmap(pagetable_t, uint64, uint64, uint64, ਪੂਰਨ);
ਪੂਰਨ             mappages(pagetable_t, uint64, uint64, uint64, ਪੂਰਨ);
pagetable_t     uvmcreate(ਖਾਲੀ);
uint64          uvmalloc(pagetable_t, uint64, uint64, ਪੂਰਨ);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ਪੂਰਨ             uvmcopy(pagetable_t, pagetable_t, uint64);
ਖਾਲੀ            uvmfree(pagetable_t, uint64);
ਖਾਲੀ            uvmunmap(pagetable_t, uint64, uint64, ਪੂਰਨ);
ਖਾਲੀ            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ਪੂਰਨ);
uint64          walkaddr(pagetable_t, uint64);
ਪੂਰਨ             copyout(pagetable_t, uint64, uint64, ਅੱਖਰ *, uint64);
ਪੂਰਨ             copyin(pagetable_t, uint64, ਅੱਖਰ *, uint64, uint64);
ਪੂਰਨ             copyinstr(pagetable_t, uint64, ਅੱਖਰ *, uint64, uint64);
ਪੂਰਨ             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ਪੂਰਨ);

// plic.c
ਖਾਲੀ            plicinit(ਖਾਲੀ);
ਖਾਲੀ            plicinithart(ਖਾਲੀ);
ਪੂਰਨ             plic_claim(ਖਾਲੀ);
ਖਾਲੀ            plic_complete(ਪੂਰਨ);

// virtio_disk.c
ਖਾਲੀ            virtio_disk_init(ਖਾਲੀ);
ਖਾਲੀ            virtio_disk_rw(ਬਣਤਰ buf *, ਪੂਰਨ);
ਖਾਲੀ            virtio_disk_intr(ਖਾਲੀ);

// number of elements in fixed-size array
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NELEM(x) (ਆਕਾਰ(x) / ਆਕਾਰ((x)[0]))
