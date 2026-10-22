// clang-format off
কাঠামো buf;
কাঠামো context;
কাঠামো file;
কাঠামো inode;
কাঠামো pipe;
কাঠামো proc;
কাঠামো spinlock;
কাঠামো sleeplock;
কাঠামো stat;
কাঠামো superblock;

// bio.c
শূন্য            binit(শূন্য);
কাঠামো buf*     bread(uint, uint);
শূন্য            brelse(কাঠামো buf*);
শূন্য            bwrite(কাঠামো buf*);
শূন্য            bpin(কাঠামো buf*);
শূন্য            bunpin(কাঠামো buf*);

// console.c
শূন্য            consoleinit(শূন্য);
শূন্য            consoleintr(পূর্ণসংখ্যা);
শূন্য            consputc(পূর্ণসংখ্যা);

// exec.c
পূর্ণসংখ্যা             kexec(অক্ষর*, অক্ষর**);

// file.c
কাঠামো file*    filealloc(শূন্য);
শূন্য            fileclose(কাঠামো file*);
কাঠামো file*    filedup(কাঠামো file*);
শূন্য            fileinit(শূন্য);
পূর্ণসংখ্যা             fileread(কাঠামো file*, uint64, পূর্ণসংখ্যা n);
পূর্ণসংখ্যা             filestat(কাঠামো file*, uint64 addr);
পূর্ণসংখ্যা             filewrite(কাঠামো file*, uint64, পূর্ণসংখ্যা n);

// fs.c
শূন্য            fsinit(পূর্ণসংখ্যা);
পূর্ণসংখ্যা             dirlink(কাঠামো inode*, অক্ষর*, uint);
কাঠামো inode*   dirlookup(কাঠামো inode*, অক্ষর*, uint*);
কাঠামো inode*   ialloc(uint, short);
কাঠামো inode*   idup(কাঠামো inode*);
শূন্য            iinit();
শূন্য            ilock(কাঠামো inode*);
শূন্য            iput(কাঠামো inode*);
শূন্য            iunlock(কাঠামো inode*);
শূন্য            iunlockput(কাঠামো inode*);
শূন্য            iupdate(কাঠামো inode*);
পূর্ণসংখ্যা             namecmp(ধ্রুবক অক্ষর*, ধ্রুবক অক্ষর*);
কাঠামো inode*   namei(অক্ষর*);
কাঠামো inode*   nameiparent(অক্ষর*, অক্ষর*);
পূর্ণসংখ্যা             readi(কাঠামো inode*, পূর্ণসংখ্যা, uint64, uint, uint);
শূন্য            stati(কাঠামো inode*, কাঠামো stat*);
পূর্ণসংখ্যা             writei(কাঠামো inode*, পূর্ণসংখ্যা, uint64, uint, uint);
শূন্য            itrunc(কাঠামো inode*);
শূন্য            ireclaim(পূর্ণসংখ্যা);

// kalloc.c
শূন্য*           kalloc(শূন্য);
শূন্য            kfree(শূন্য *);
শূন্য            kinit(শূন্য);

// log.c
শূন্য            initlog(পূর্ণসংখ্যা, কাঠামো superblock*);
শূন্য            log_write(কাঠামো buf*);
শূন্য            begin_op(শূন্য);
শূন্য            end_op(শূন্য);

// pipe.c
পূর্ণসংখ্যা             pipealloc(কাঠামো file**, কাঠামো file**);
শূন্য            pipeclose(কাঠামো pipe*, পূর্ণসংখ্যা);
পূর্ণসংখ্যা             piperead(কাঠামো pipe*, uint64, পূর্ণসংখ্যা);
পূর্ণসংখ্যা             pipewrite(কাঠামো pipe*, uint64, পূর্ণসংখ্যা);

// printk.c
পূর্ণসংখ্যা             printk(অক্ষর*, ...) __attribute__ ((format (printf, 1, 2)));
শূন্য            panic(অক্ষর*) __attribute__((noreturn));
শূন্য            printkinit(শূন্য);

// proc.c
পূর্ণসংখ্যা             cpuid(শূন্য);
শূন্য            kexit(পূর্ণসংখ্যা);
পূর্ণসংখ্যা             kfork(শূন্য);
পূর্ণসংখ্যা             growproc(পূর্ণসংখ্যা);
শূন্য            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(কাঠামো proc *);
শূন্য            proc_freepagetable(pagetable_t, uint64);
পূর্ণসংখ্যা             kkill(পূর্ণসংখ্যা);
পূর্ণসংখ্যা             killed(কাঠামো proc*);
শূন্য            setkilled(কাঠামো proc*);
কাঠামো cpu*     mycpu(শূন্য);
কাঠামো proc*    myproc();
শূন্য            procinit(শূন্য);
শূন্য            scheduler(শূন্য) __attribute__((noreturn));
শূন্য            sched(শূন্য);
শূন্য            sleep_prepare(শূন্য*);
শূন্য            sleep(শূন্য);
শূন্য            userinit(শূন্য);
পূর্ণসংখ্যা             kwait(uint64);
শূন্য            wakeup(শূন্য*);
শূন্য            yield(শূন্য);
পূর্ণসংখ্যা             either_copyout(পূর্ণসংখ্যা user_dst, uint64 dst, শূন্য *src, uint64 len);
পূর্ণসংখ্যা             either_copyin(শূন্য *dst, পূর্ণসংখ্যা user_src, uint64 src, uint64 len);
শূন্য            procdump(শূন্য);

// swtch.S
শূন্য            swtch(কাঠামো context*, কাঠামো context*);

// spinlock.c
শূন্য            acquire(কাঠামো spinlock*);
পূর্ণসংখ্যা             holding(কাঠামো spinlock*);
শূন্য            initlock(কাঠামো spinlock*, অক্ষর*);
শূন্য            release(কাঠামো spinlock*);
শূন্য            ঠেলো_off(শূন্য);
শূন্য            বের_করো_off(শূন্য);

// sleeplock.c
শূন্য            acquiresleep(কাঠামো sleeplock*);
শূন্য            releasesleep(কাঠামো sleeplock*);
পূর্ণসংখ্যা             holdingsleep(কাঠামো sleeplock*);
শূন্য            initsleeplock(কাঠামো sleeplock*, অক্ষর*);

// string.c
পূর্ণসংখ্যা             memcmp(ধ্রুবক শূন্য*, ধ্রুবক শূন্য*, uint);
শূন্য*           memmove(শূন্য*, ধ্রুবক শূন্য*, uint);
শূন্য*           memset(শূন্য*, পূর্ণসংখ্যা, uint);
অক্ষর*           safestrcpy(অক্ষর*, ধ্রুবক অক্ষর*, পূর্ণসংখ্যা);
পূর্ণসংখ্যা             strlen(ধ্রুবক অক্ষর*);
পূর্ণসংখ্যা             strncmp(ধ্রুবক অক্ষর*, ধ্রুবক অক্ষর*, uint);
অক্ষর*           strncpy(অক্ষর*, ধ্রুবক অক্ষর*, পূর্ণসংখ্যা);

// syscall.c
শূন্য            argint(পূর্ণসংখ্যা, পূর্ণসংখ্যা*);
পূর্ণসংখ্যা             argstr(পূর্ণসংখ্যা, অক্ষর*, পূর্ণসংখ্যা);
শূন্য            argaddr(পূর্ণসংখ্যা, uint64 *);
পূর্ণসংখ্যা             fetchstr(uint64, অক্ষর*, পূর্ণসংখ্যা);
পূর্ণসংখ্যা             fetchaddr(uint64, uint64*);
শূন্য            syscall();

// trap.c
বাহ্যিক uint     ticks;
শূন্য            trapinit(শূন্য);
শূন্য            trapinithart(শূন্য);
বাহ্যিক কাঠামো spinlock tickslock;
শূন্য            prepare_return(শূন্য);

// uart.c
শূন্য            uartinit(শূন্য);
শূন্য            uartintr(শূন্য);
শূন্য            uartwrite(অক্ষর [], পূর্ণসংখ্যা);
শূন্য            uartputc_sync(পূর্ণসংখ্যা);

// vm.c
শূন্য            kvminit(শূন্য);
শূন্য            kvminithart(শূন্য);
শূন্য            kvmmap(pagetable_t, uint64, uint64, uint64, পূর্ণসংখ্যা);
পূর্ণসংখ্যা             mappages(pagetable_t, uint64, uint64, uint64, পূর্ণসংখ্যা);
pagetable_t     uvmcreate(শূন্য);
uint64          uvmalloc(pagetable_t, uint64, uint64, পূর্ণসংখ্যা);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
পূর্ণসংখ্যা             uvmcopy(pagetable_t, pagetable_t, uint64);
শূন্য            uvmfree(pagetable_t, uint64);
শূন্য            uvmunmap(pagetable_t, uint64, uint64, পূর্ণসংখ্যা);
শূন্য            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, পূর্ণসংখ্যা);
uint64          walkaddr(pagetable_t, uint64);
পূর্ণসংখ্যা             copyout(pagetable_t, uint64, uint64, অক্ষর *, uint64);
পূর্ণসংখ্যা             copyin(pagetable_t, uint64, অক্ষর *, uint64, uint64);
পূর্ণসংখ্যা             copyinstr(pagetable_t, uint64, অক্ষর *, uint64, uint64);
পূর্ণসংখ্যা             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, পূর্ণসংখ্যা);

// plic.c
শূন্য            plicinit(শূন্য);
শূন্য            plicinithart(শূন্য);
পূর্ণসংখ্যা             plic_claim(শূন্য);
শূন্য            plic_complete(পূর্ণসংখ্যা);

// virtio_disk.c
শূন্য            virtio_disk_init(শূন্য);
শূন্য            virtio_disk_rw(কাঠামো buf *, পূর্ণসংখ্যা);
শূন্য            virtio_disk_intr(শূন্য);

// number of elements in fixed-size array
#define NELEM(x) (আকার(x) / আকার((x)[0]))
