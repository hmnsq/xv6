// clang-format off
সংরচনা buf;
সংরচনা context;
সংরচনা file;
সংরচনা inode;
সংরচনা pipe;
সংরচনা proc;
সংরচনা spinlock;
সংরচনা sleeplock;
সংরচনা stat;
সংরচনা superblock;

// bio.c
শূন্য            binit(শূন্য);
সংরচনা buf*     bread(uint, uint);
শূন্য            brelse(সংরচনা buf*);
শূন্য            bwrite(সংরচনা buf*);
শূন্য            bpin(সংরচনা buf*);
শূন্য            bunpin(সংরচনা buf*);

// console.c
শূন্য            consoleinit(শূন্য);
শূন্য            consoleintr(পূর্ণাঙ্ক);
শূন্য            consputc(পূর্ণাঙ্ক);

// exec.c
পূর্ণাঙ্ক             kexec(ৱর্ণ*, ৱর্ণ**);

// file.c
সংরচনা file*    filealloc(শূন্য);
শূন্য            fileclose(সংরচনা file*);
সংরচনা file*    filedup(সংরচনা file*);
শূন্য            fileinit(শূন্য);
পূর্ণাঙ্ক             fileread(সংরচনা file*, uint64, পূর্ণাঙ্ক n);
পূর্ণাঙ্ক             filestat(সংরচনা file*, uint64 addr);
পূর্ণাঙ্ক             filewrite(সংরচনা file*, uint64, পূর্ণাঙ্ক n);

// fs.c
শূন্য            fsinit(পূর্ণাঙ্ক);
পূর্ণাঙ্ক             dirlink(সংরচনা inode*, ৱর্ণ*, uint);
সংরচনা inode*   dirlookup(সংরচনা inode*, ৱর্ণ*, uint*);
সংরচনা inode*   ialloc(uint, short);
সংরচনা inode*   idup(সংরচনা inode*);
শূন্য            iinit();
শূন্য            ilock(সংরচনা inode*);
শূন্য            iput(সংরচনা inode*);
শূন্য            iunlock(সংরচনা inode*);
শূন্য            iunlockput(সংরচনা inode*);
শূন্য            iupdate(সংরচনা inode*);
পূর্ণাঙ্ক             namecmp(ধ্রুৱ ৱর্ণ*, ধ্রুৱ ৱর্ণ*);
সংরচনা inode*   namei(ৱর্ণ*);
সংরচনা inode*   nameiparent(ৱর্ণ*, ৱর্ণ*);
পূর্ণাঙ্ক             readi(সংরচনা inode*, পূর্ণাঙ্ক, uint64, uint, uint);
শূন্য            stati(সংরচনা inode*, সংরচনা stat*);
পূর্ণাঙ্ক             writei(সংরচনা inode*, পূর্ণাঙ্ক, uint64, uint, uint);
শূন্য            itrunc(সংরচনা inode*);
শূন্য            ireclaim(পূর্ণাঙ্ক);

// kalloc.c
শূন্য*           kalloc(শূন্য);
শূন্য            kfree(শূন্য *);
শূন্য            kinit(শূন্য);

// log.c
শূন্য            initlog(পূর্ণাঙ্ক, সংরচনা superblock*);
শূন্য            log_write(সংরচনা buf*);
শূন্য            begin_op(শূন্য);
শূন্য            end_op(শূন্য);

// pipe.c
পূর্ণাঙ্ক             pipealloc(সংরচনা file**, সংরচনা file**);
শূন্য            pipeclose(সংরচনা pipe*, পূর্ণাঙ্ক);
পূর্ণাঙ্ক             piperead(সংরচনা pipe*, uint64, পূর্ণাঙ্ক);
পূর্ণাঙ্ক             pipewrite(সংরচনা pipe*, uint64, পূর্ণাঙ্ক);

// printk.c
পূর্ণাঙ্ক             printk(ৱর্ণ*, ...) __attribute__ ((format (printf, 1, 2)));
শূন্য            panic(ৱর্ণ*) __attribute__((noreturn));
শূন্য            printkinit(শূন্য);

// proc.c
পূর্ণাঙ্ক             cpuid(শূন্য);
শূন্য            kexit(পূর্ণাঙ্ক);
পূর্ণাঙ্ক             kfork(শূন্য);
পূর্ণাঙ্ক             growproc(পূর্ণাঙ্ক);
শূন্য            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(সংরচনা proc *);
শূন্য            proc_freepagetable(pagetable_t, uint64);
পূর্ণাঙ্ক             kkill(পূর্ণাঙ্ক);
পূর্ণাঙ্ক             killed(সংরচনা proc*);
শূন্য            setkilled(সংরচনা proc*);
সংরচনা cpu*     mycpu(শূন্য);
সংরচনা proc*    myproc();
শূন্য            procinit(শূন্য);
শূন্য            scheduler(শূন্য) __attribute__((noreturn));
শূন্য            sched(শূন্য);
শূন্য            sleep_prepare(শূন্য*);
শূন্য            sleep(শূন্য);
শূন্য            userinit(শূন্য);
পূর্ণাঙ্ক             kwait(uint64);
শূন্য            wakeup(শূন্য*);
শূন্য            yield(শূন্য);
পূর্ণাঙ্ক             either_copyout(পূর্ণাঙ্ক user_dst, uint64 dst, শূন্য *src, uint64 len);
পূর্ণাঙ্ক             either_copyin(শূন্য *dst, পূর্ণাঙ্ক user_src, uint64 src, uint64 len);
শূন্য            procdump(শূন্য);

// swtch.S
শূন্য            swtch(সংরচনা context*, সংরচনা context*);

// spinlock.c
শূন্য            acquire(সংরচনা spinlock*);
পূর্ণাঙ্ক             holding(সংরচনা spinlock*);
শূন্য            initlock(সংরচনা spinlock*, ৱর্ণ*);
শূন্য            release(সংরচনা spinlock*);
শূন্য            ঠেলো_off(শূন্য);
শূন্য            বেৰ_কৰো_off(শূন্য);

// sleeplock.c
শূন্য            acquiresleep(সংরচনা sleeplock*);
শূন্য            releasesleep(সংরচনা sleeplock*);
পূর্ণাঙ্ক             holdingsleep(সংরচনা sleeplock*);
শূন্য            initsleeplock(সংরচনা sleeplock*, ৱর্ণ*);

// string.c
পূর্ণাঙ্ক             memcmp(ধ্রুৱ শূন্য*, ধ্রুৱ শূন্য*, uint);
শূন্য*           memmove(শূন্য*, ধ্রুৱ শূন্য*, uint);
শূন্য*           memset(শূন্য*, পূর্ণাঙ্ক, uint);
ৱর্ণ*           safestrcpy(ৱর্ণ*, ধ্রুৱ ৱর্ণ*, পূর্ণাঙ্ক);
পূর্ণাঙ্ক             strlen(ধ্রুৱ ৱর্ণ*);
পূর্ণাঙ্ক             strncmp(ধ্রুৱ ৱর্ণ*, ধ্রুৱ ৱর্ণ*, uint);
ৱর্ণ*           strncpy(ৱর্ণ*, ধ্রুৱ ৱর্ণ*, পূর্ণাঙ্ক);

// syscall.c
শূন্য            argint(পূর্ণাঙ্ক, পূর্ণাঙ্ক*);
পূর্ণাঙ্ক             argstr(পূর্ণাঙ্ক, ৱর্ণ*, পূর্ণাঙ্ক);
শূন্য            argaddr(পূর্ণাঙ্ক, uint64 *);
পূর্ণাঙ্ক             fetchstr(uint64, ৱর্ণ*, পূর্ণাঙ্ক);
পূর্ণাঙ্ক             fetchaddr(uint64, uint64*);
শূন্য            syscall();

// trap.c
বাহ্য uint     ticks;
শূন্য            trapinit(শূন্য);
শূন্য            trapinithart(শূন্য);
বাহ্য সংরচনা spinlock tickslock;
শূন্য            prepare_return(শূন্য);

// uart.c
শূন্য            uartinit(শূন্য);
শূন্য            uartintr(শূন্য);
শূন্য            uartwrite(ৱর্ণ [], পূর্ণাঙ্ক);
শূন্য            uartputc_sync(পূর্ণাঙ্ক);

// vm.c
শূন্য            kvminit(শূন্য);
শূন্য            kvminithart(শূন্য);
শূন্য            kvmmap(pagetable_t, uint64, uint64, uint64, পূর্ণাঙ্ক);
পূর্ণাঙ্ক             mappages(pagetable_t, uint64, uint64, uint64, পূর্ণাঙ্ক);
pagetable_t     uvmcreate(শূন্য);
uint64          uvmalloc(pagetable_t, uint64, uint64, পূর্ণাঙ্ক);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
পূর্ণাঙ্ক             uvmcopy(pagetable_t, pagetable_t, uint64);
শূন্য            uvmfree(pagetable_t, uint64);
শূন্য            uvmunmap(pagetable_t, uint64, uint64, পূর্ণাঙ্ক);
শূন্য            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, পূর্ণাঙ্ক);
uint64          walkaddr(pagetable_t, uint64);
পূর্ণাঙ্ক             copyout(pagetable_t, uint64, uint64, ৱর্ণ *, uint64);
পূর্ণাঙ্ক             copyin(pagetable_t, uint64, ৱর্ণ *, uint64, uint64);
পূর্ণাঙ্ক             copyinstr(pagetable_t, uint64, ৱর্ণ *, uint64, uint64);
পূর্ণাঙ্ক             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, পূর্ণাঙ্ক);

// plic.c
শূন্য            plicinit(শূন্য);
শূন্য            plicinithart(শূন্য);
পূর্ণাঙ্ক             plic_claim(শূন্য);
শূন্য            plic_complete(পূর্ণাঙ্ক);

// virtio_disk.c
শূন্য            virtio_disk_init(শূন্য);
শূন্য            virtio_disk_rw(সংরচনা buf *, পূর্ণাঙ্ক);
শূন্য            virtio_disk_intr(শূন্য);

// number of elements in fixed-size array
#define NELEM(x) (পরিমাণ(x) / পরিমাণ((x)[0]))
