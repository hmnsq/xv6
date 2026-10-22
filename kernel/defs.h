// clang-format off
ساختار buf;
ساختار context;
ساختار file;
ساختار inode;
ساختار pipe;
ساختار proc;
ساختار spinlock;
ساختار sleeplock;
ساختار stat;
ساختار superblock;

// bio.c
تهی            binit(تهی);
ساختار buf*     bread(uint, uint);
تهی            brelse(ساختار buf*);
تهی            bwrite(ساختار buf*);
تهی            bpin(ساختار buf*);
تهی            bunpin(ساختار buf*);

// console.c
تهی            consoleinit(تهی);
تهی            consoleintr(صحیح);
تهی            consputc(صحیح);

// exec.c
صحیح             kexec(نویسه*, نویسه**);

// file.c
ساختار file*    filealloc(تهی);
تهی            fileclose(ساختار file*);
ساختار file*    filedup(ساختار file*);
تهی            fileinit(تهی);
صحیح             fileread(ساختار file*, uint64, صحیح n);
صحیح             filestat(ساختار file*, uint64 addr);
صحیح             filewrite(ساختار file*, uint64, صحیح n);

// fs.c
تهی            fsinit(صحیح);
صحیح             dirlink(ساختار inode*, نویسه*, uint);
ساختار inode*   dirlookup(ساختار inode*, نویسه*, uint*);
ساختار inode*   ialloc(uint, short);
ساختار inode*   idup(ساختار inode*);
تهی            iinit();
تهی            ilock(ساختار inode*);
تهی            iput(ساختار inode*);
تهی            iunlock(ساختار inode*);
تهی            iunlockput(ساختار inode*);
تهی            iupdate(ساختار inode*);
صحیح             namecmp(ثابت نویسه*, ثابت نویسه*);
ساختار inode*   namei(نویسه*);
ساختار inode*   nameiparent(نویسه*, نویسه*);
صحیح             readi(ساختار inode*, صحیح, uint64, uint, uint);
تهی            stati(ساختار inode*, ساختار stat*);
صحیح             writei(ساختار inode*, صحیح, uint64, uint, uint);
تهی            itrunc(ساختار inode*);
تهی            ireclaim(صحیح);

// kalloc.c
تهی*           kalloc(تهی);
تهی            kfree(تهی *);
تهی            kinit(تهی);

// log.c
تهی            initlog(صحیح, ساختار superblock*);
تهی            log_write(ساختار buf*);
تهی            begin_op(تهی);
تهی            end_op(تهی);

// pipe.c
صحیح             pipealloc(ساختار file**, ساختار file**);
تهی            pipeclose(ساختار pipe*, صحیح);
صحیح             piperead(ساختار pipe*, uint64, صحیح);
صحیح             pipewrite(ساختار pipe*, uint64, صحیح);

// printk.c
صحیح             printk(نویسه*, ...) __attribute__ ((format (printf, 1, 2)));
تهی            panic(نویسه*) __attribute__((noreturn));
تهی            printkinit(تهی);

// proc.c
صحیح             cpuid(تهی);
تهی            kexit(صحیح);
صحیح             kfork(تهی);
صحیح             growproc(صحیح);
تهی            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ساختار proc *);
تهی            proc_freepagetable(pagetable_t, uint64);
صحیح             kkill(صحیح);
صحیح             killed(ساختار proc*);
تهی            setkilled(ساختار proc*);
ساختار cpu*     mycpu(تهی);
ساختار proc*    myproc();
تهی            procinit(تهی);
تهی            scheduler(تهی) __attribute__((noreturn));
تهی            sched(تهی);
تهی            sleep_prepare(تهی*);
تهی            sleep(تهی);
تهی            userinit(تهی);
صحیح             kwait(uint64);
تهی            wakeup(تهی*);
تهی            yield(تهی);
صحیح             either_copyout(صحیح user_dst, uint64 dst, تهی *src, uint64 len);
صحیح             either_copyin(تهی *dst, صحیح user_src, uint64 src, uint64 len);
تهی            procdump(تهی);

// swtch.S
تهی            swtch(ساختار context*, ساختار context*);

// spinlock.c
تهی            acquire(ساختار spinlock*);
صحیح             holding(ساختار spinlock*);
تهی            initlock(ساختار spinlock*, نویسه*);
تهی            release(ساختار spinlock*);
تهی            افزودن_off(تهی);
تهی            برداشت_off(تهی);

// sleeplock.c
تهی            acquiresleep(ساختار sleeplock*);
تهی            releasesleep(ساختار sleeplock*);
صحیح             holdingsleep(ساختار sleeplock*);
تهی            initsleeplock(ساختار sleeplock*, نویسه*);

// string.c
صحیح             memcmp(ثابت تهی*, ثابت تهی*, uint);
تهی*           memmove(تهی*, ثابت تهی*, uint);
تهی*           memset(تهی*, صحیح, uint);
نویسه*           safestrcpy(نویسه*, ثابت نویسه*, صحیح);
صحیح             strlen(ثابت نویسه*);
صحیح             strncmp(ثابت نویسه*, ثابت نویسه*, uint);
نویسه*           strncpy(نویسه*, ثابت نویسه*, صحیح);

// syscall.c
تهی            argint(صحیح, صحیح*);
صحیح             argstr(صحیح, نویسه*, صحیح);
تهی            argaddr(صحیح, uint64 *);
صحیح             fetchstr(uint64, نویسه*, صحیح);
صحیح             fetchaddr(uint64, uint64*);
تهی            syscall();

// trap.c
بیرونی uint     ticks;
تهی            trapinit(تهی);
تهی            trapinithart(تهی);
بیرونی ساختار spinlock tickslock;
تهی            prepare_return(تهی);

// uart.c
تهی            uartinit(تهی);
تهی            uartintr(تهی);
تهی            uartwrite(نویسه [], صحیح);
تهی            uartputc_sync(صحیح);

// vm.c
تهی            kvminit(تهی);
تهی            kvminithart(تهی);
تهی            kvmmap(pagetable_t, uint64, uint64, uint64, صحیح);
صحیح             mappages(pagetable_t, uint64, uint64, uint64, صحیح);
pagetable_t     uvmcreate(تهی);
uint64          uvmalloc(pagetable_t, uint64, uint64, صحیح);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
صحیح             uvmcopy(pagetable_t, pagetable_t, uint64);
تهی            uvmfree(pagetable_t, uint64);
تهی            uvmunmap(pagetable_t, uint64, uint64, صحیح);
تهی            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, صحیح);
uint64          walkaddr(pagetable_t, uint64);
صحیح             copyout(pagetable_t, uint64, uint64, نویسه *, uint64);
صحیح             copyin(pagetable_t, uint64, نویسه *, uint64, uint64);
صحیح             copyinstr(pagetable_t, uint64, نویسه *, uint64, uint64);
صحیح             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, صحیح);

// plic.c
تهی            plicinit(تهی);
تهی            plicinithart(تهی);
صحیح             plic_claim(تهی);
تهی            plic_complete(صحیح);

// virtio_disk.c
تهی            virtio_disk_init(تهی);
تهی            virtio_disk_rw(ساختار buf *, صحیح);
تهی            virtio_disk_intr(تهی);

// number of elements in fixed-size array
#define NELEM(x) (اندازه(x) / اندازه((x)[0]))
