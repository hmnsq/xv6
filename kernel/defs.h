// clang-format off
بنية buf;
بنية context;
بنية file;
بنية inode;
بنية pipe;
بنية proc;
بنية spinlock;
بنية sleeplock;
بنية stat;
بنية superblock;

// bio.c
فراغ            binit(فراغ);
بنية buf*     bread(uint, uint);
فراغ            brelse(بنية buf*);
فراغ            bwrite(بنية buf*);
فراغ            bpin(بنية buf*);
فراغ            bunpin(بنية buf*);

// console.c
فراغ            consoleinit(فراغ);
فراغ            consoleintr(صحيح);
فراغ            consputc(صحيح);

// exec.c
صحيح             kexec(حرف*, حرف**);

// file.c
بنية file*    filealloc(فراغ);
فراغ            fileclose(بنية file*);
بنية file*    filedup(بنية file*);
فراغ            fileinit(فراغ);
صحيح             fileread(بنية file*, uint64, صحيح n);
صحيح             filestat(بنية file*, uint64 addr);
صحيح             filewrite(بنية file*, uint64, صحيح n);

// fs.c
فراغ            fsinit(صحيح);
صحيح             dirlink(بنية inode*, حرف*, uint);
بنية inode*   dirlookup(بنية inode*, حرف*, uint*);
بنية inode*   ialloc(uint, چھوٹا);
بنية inode*   idup(بنية inode*);
فراغ            iinit();
فراغ            ilock(بنية inode*);
فراغ            iput(بنية inode*);
فراغ            iunlock(بنية inode*);
فراغ            iunlockput(بنية inode*);
فراغ            iupdate(بنية inode*);
صحيح             namecmp(ثابت حرف*, ثابت حرف*);
بنية inode*   namei(حرف*);
بنية inode*   nameiparent(حرف*, حرف*);
صحيح             readi(بنية inode*, صحيح, uint64, uint, uint);
فراغ            stati(بنية inode*, بنية stat*);
صحيح             writei(بنية inode*, صحيح, uint64, uint, uint);
فراغ            itrunc(بنية inode*);
فراغ            ireclaim(صحيح);

// kalloc.c
فراغ*           kalloc(فراغ);
فراغ            kfree(فراغ *);
فراغ            kinit(فراغ);

// log.c
فراغ            initlog(صحيح, بنية superblock*);
فراغ            log_write(بنية buf*);
فراغ            begin_op(فراغ);
فراغ            سرا_op(فراغ);

// pipe.c
صحيح             pipealloc(بنية file**, بنية file**);
فراغ            pipeclose(بنية pipe*, صحيح);
صحيح             piperead(بنية pipe*, uint64, صحيح);
صحيح             pipewrite(بنية pipe*, uint64, صحيح);

// printk.c
صحيح             printk(حرف*, ...) __attribute__ ((format (لکھو, 1, 2)));
فراغ            panic(حرف*) __attribute__((noreturn));
فراغ            printkinit(فراغ);

// proc.c
صحيح             cpuid(فراغ);
فراغ            kexit(صحيح);
صحيح             kfork(فراغ);
صحيح             growproc(صحيح);
فراغ            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(بنية proc *);
فراغ            proc_freepagetable(pagetable_t, uint64);
صحيح             kkill(صحيح);
صحيح             killed(بنية proc*);
فراغ            setkilled(بنية proc*);
بنية cpu*     mycpu(فراغ);
بنية proc*    myproc();
فراغ            procinit(فراغ);
فراغ            scheduler(فراغ) __attribute__((noreturn));
فراغ            sched(فراغ);
فراغ            sleep_prepare(فراغ*);
فراغ            sleep(فراغ);
فراغ            userinit(فراغ);
صحيح             kwait(uint64);
فراغ            wakeup(فراغ*);
فراغ            yield(فراغ);
صحيح             either_copyout(صحيح user_dst, uint64 dst, فراغ *src, uint64 len);
صحيح             either_copyin(فراغ *dst, صحيح user_src, uint64 src, uint64 len);
فراغ            procdump(فراغ);

// swtch.S
فراغ            swtch(بنية context*, بنية context*);

// spinlock.c
فراغ            acquire(بنية spinlock*);
صحيح             holding(بنية spinlock*);
فراغ            initlock(بنية spinlock*, حرف*);
فراغ            release(بنية spinlock*);
فراغ            ڈالو_off(فراغ);
فراغ            نکالو_off(فراغ);

// sleeplock.c
فراغ            acquiresleep(بنية sleeplock*);
فراغ            releasesleep(بنية sleeplock*);
صحيح             holdingsleep(بنية sleeplock*);
فراغ            initsleeplock(بنية sleeplock*, حرف*);

// string.c
صحيح             memcmp(ثابت فراغ*, ثابت فراغ*, uint);
فراغ*           memmove(فراغ*, ثابت فراغ*, uint);
فراغ*           memset(فراغ*, صحيح, uint);
حرف*           safestrcpy(حرف*, ثابت حرف*, صحيح);
صحيح             لڑی_لمبائی(ثابت حرف*);
صحيح             لڑی_مقابلہ(ثابت حرف*, ثابت حرف*, uint);
حرف*           strncpy(حرف*, ثابت حرف*, صحيح);

// syscall.c
فراغ            argint(صحيح, صحيح*);
صحيح             argstr(صحيح, حرف*, صحيح);
فراغ            argaddr(صحيح, uint64 *);
صحيح             fetchstr(uint64, حرف*, صحيح);
صحيح             fetchaddr(uint64, uint64*);
فراغ            syscall();

// trap.c
خارجي uint     ticks;
فراغ            trapinit(فراغ);
فراغ            trapinithart(فراغ);
خارجي بنية spinlock tickslock;
فراغ            prepare_return(فراغ);

// uart.c
فراغ            uartinit(فراغ);
فراغ            uartintr(فراغ);
فراغ            uartwrite(حرف [], صحيح);
فراغ            uartputc_sync(صحيح);

// vm.c
فراغ            kvminit(فراغ);
فراغ            kvminithart(فراغ);
فراغ            kvmmap(pagetable_t, uint64, uint64, uint64, صحيح);
صحيح             mappages(pagetable_t, uint64, uint64, uint64, صحيح);
pagetable_t     uvmcreate(فراغ);
uint64          uvmalloc(pagetable_t, uint64, uint64, صحيح);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
صحيح             uvmcopy(pagetable_t, pagetable_t, uint64);
فراغ            uvmfree(pagetable_t, uint64);
فراغ            uvmunmap(pagetable_t, uint64, uint64, صحيح);
فراغ            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, صحيح);
uint64          walkaddr(pagetable_t, uint64);
صحيح             copyout(pagetable_t, uint64, uint64, حرف *, uint64);
صحيح             copyin(pagetable_t, uint64, حرف *, uint64, uint64);
صحيح             copyinstr(pagetable_t, uint64, حرف *, uint64, uint64);
صحيح             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, صحيح);

// plic.c
فراغ            plicinit(فراغ);
فراغ            plicinithart(فراغ);
صحيح             plic_claim(فراغ);
فراغ            plic_complete(صحيح);

// virtio_disk.c
فراغ            virtio_disk_init(فراغ);
فراغ            virtio_disk_rw(بنية buf *, صحيح);
فراغ            virtio_disk_intr(فراغ);

// number of elements in fixed-size array
#تعریف_کرو NELEM(x) (حجم(x) / حجم((x)[0]))
