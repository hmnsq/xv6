// clang-format off
ساخت buf;
ساخت context;
ساخت file;
ساخت inode;
ساخت pipe;
ساخت proc;
ساخت spinlock;
ساخت sleeplock;
ساخت stat;
ساخت superblock;

// bio.c
خالی            binit(خالی);
ساخت buf*     bread(uint, uint);
خالی            brelse(ساخت buf*);
خالی            bwrite(ساخت buf*);
خالی            bpin(ساخت buf*);
خالی            bunpin(ساخت buf*);

// console.c
خالی            consoleinit(خالی);
خالی            consoleintr(صحیح);
خالی            consputc(صحیح);

// exec.c
صحیح             kexec(حرف*, حرف**);

// file.c
ساخت file*    filealloc(خالی);
خالی            fileclose(ساخت file*);
ساخت file*    filedup(ساخت file*);
خالی            fileinit(خالی);
صحیح             fileread(ساخت file*, uint64, صحیح n);
صحیح             filestat(ساخت file*, uint64 addr);
صحیح             filewrite(ساخت file*, uint64, صحیح n);

// fs.c
خالی            fsinit(صحیح);
صحیح             dirlink(ساخت inode*, حرف*, uint);
ساخت inode*   dirlookup(ساخت inode*, حرف*, uint*);
ساخت inode*   ialloc(uint, مختصر);
ساخت inode*   idup(ساخت inode*);
خالی            iinit();
خالی            ilock(ساخت inode*);
خالی            iput(ساخت inode*);
خالی            iunlock(ساخت inode*);
خالی            iunlockput(ساخت inode*);
خالی            iupdate(ساخت inode*);
صحیح             namecmp(ثابت حرف*, ثابت حرف*);
ساخت inode*   namei(حرف*);
ساخت inode*   nameiparent(حرف*, حرف*);
صحیح             readi(ساخت inode*, صحیح, uint64, uint, uint);
خالی            stati(ساخت inode*, ساخت stat*);
صحیح             writei(ساخت inode*, صحیح, uint64, uint, uint);
خالی            itrunc(ساخت inode*);
خالی            ireclaim(صحیح);

// kalloc.c
خالی*           kalloc(خالی);
خالی            kfree(خالی *);
خالی            kinit(خالی);

// log.c
خالی            initlog(صحیح, ساخت superblock*);
خالی            log_write(ساخت buf*);
خالی            begin_op(خالی);
خالی            انتہا_op(خالی);

// pipe.c
صحیح             pipealloc(ساخت file**, ساخت file**);
خالی            pipeclose(ساخت pipe*, صحیح);
صحیح             piperead(ساخت pipe*, uint64, صحیح);
صحیح             pipewrite(ساخت pipe*, uint64, صحیح);

// printk.c
صحیح             printk(حرف*, ...) __attribute__ ((format (لکھو, 1, 2)));
خالی            panic(حرف*) __attribute__((noreturn));
خالی            printkinit(خالی);

// proc.c
صحیح             cpuid(خالی);
خالی            kexit(صحیح);
صحیح             kfork(خالی);
صحیح             growproc(صحیح);
خالی            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ساخت proc *);
خالی            proc_freepagetable(pagetable_t, uint64);
صحیح             kkill(صحیح);
صحیح             killed(ساخت proc*);
خالی            setkilled(ساخت proc*);
ساخت cpu*     mycpu(خالی);
ساخت proc*    myproc();
خالی            procinit(خالی);
خالی            scheduler(خالی) __attribute__((noreturn));
خالی            sched(خالی);
خالی            sleep_prepare(خالی*);
خالی            sleep(خالی);
خالی            userinit(خالی);
صحیح             kwait(uint64);
خالی            wakeup(خالی*);
خالی            yield(خالی);
صحیح             either_copyout(صحیح user_dst, uint64 dst, خالی *src, uint64 len);
صحیح             either_copyin(خالی *dst, صحیح user_src, uint64 src, uint64 len);
خالی            procdump(خالی);

// swtch.S
خالی            swtch(ساخت context*, ساخت context*);

// spinlock.c
خالی            acquire(ساخت spinlock*);
صحیح             holding(ساخت spinlock*);
خالی            initlock(ساخت spinlock*, حرف*);
خالی            release(ساخت spinlock*);
خالی            ڈالو_off(خالی);
خالی            نکالو_off(خالی);

// sleeplock.c
خالی            acquiresleep(ساخت sleeplock*);
خالی            releasesleep(ساخت sleeplock*);
صحیح             holdingsleep(ساخت sleeplock*);
خالی            initsleeplock(ساخت sleeplock*, حرف*);

// string.c
صحیح             memcmp(ثابت خالی*, ثابت خالی*, uint);
خالی*           memmove(خالی*, ثابت خالی*, uint);
خالی*           memset(خالی*, صحیح, uint);
حرف*           safestrcpy(حرف*, ثابت حرف*, صحیح);
صحیح             لڑی_لمبائی(ثابت حرف*);
صحیح             لڑی_موازنہ(ثابت حرف*, ثابت حرف*, uint);
حرف*           strncpy(حرف*, ثابت حرف*, صحیح);

// syscall.c
خالی            argint(صحیح, صحیح*);
صحیح             argstr(صحیح, حرف*, صحیح);
خالی            argaddr(صحیح, uint64 *);
صحیح             fetchstr(uint64, حرف*, صحیح);
صحیح             fetchaddr(uint64, uint64*);
خالی            syscall();

// trap.c
خارجی uint     ticks;
خالی            trapinit(خالی);
خالی            trapinithart(خالی);
خارجی ساخت spinlock tickslock;
خالی            prepare_return(خالی);

// uart.c
خالی            uartinit(خالی);
خالی            uartintr(خالی);
خالی            uartwrite(حرف [], صحیح);
خالی            uartputc_sync(صحیح);

// vm.c
خالی            kvminit(خالی);
خالی            kvminithart(خالی);
خالی            kvmmap(pagetable_t, uint64, uint64, uint64, صحیح);
صحیح             mappages(pagetable_t, uint64, uint64, uint64, صحیح);
pagetable_t     uvmcreate(خالی);
uint64          uvmalloc(pagetable_t, uint64, uint64, صحیح);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
صحیح             uvmcopy(pagetable_t, pagetable_t, uint64);
خالی            uvmfree(pagetable_t, uint64);
خالی            uvmunmap(pagetable_t, uint64, uint64, صحیح);
خالی            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, صحیح);
uint64          walkaddr(pagetable_t, uint64);
صحیح             copyout(pagetable_t, uint64, uint64, حرف *, uint64);
صحیح             copyin(pagetable_t, uint64, حرف *, uint64, uint64);
صحیح             copyinstr(pagetable_t, uint64, حرف *, uint64, uint64);
صحیح             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, صحیح);

// plic.c
خالی            plicinit(خالی);
خالی            plicinithart(خالی);
صحیح             plic_claim(خالی);
خالی            plic_complete(صحیح);

// virtio_disk.c
خالی            virtio_disk_init(خالی);
خالی            virtio_disk_rw(ساخت buf *, صحیح);
خالی            virtio_disk_intr(خالی);

// number of elements in fixed-size array
#تعریف_کرو NELEM(x) (حجم(x) / حجم((x)[0]))
