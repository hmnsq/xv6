// clang-format off
sa_nracanaa buf;
sa_nracanaa context;
sa_nracanaa file;
sa_nracanaa inode;
sa_nracanaa pipe;
sa_nracanaa proc;
sa_nracanaa spinlock;
sa_nracanaa sleeplock;
sa_nracanaa stat;
sa_nracanaa superblock;

// bio.c
void            binit(void);
sa_nracanaa buf*     bread(uint, uint);
void            brelse(sa_nracanaa buf*);
void            bwrite(sa_nracanaa buf*);
void            bpin(sa_nracanaa buf*);
void            bunpin(sa_nracanaa buf*);

// console.c
void            consoleinit(void);
void            consoleintr(ចំនួនគត់);
void            consputc(ចំនួនគត់);

// exec.c
ចំនួនគត់             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
ចំនួនគត់             fileread(sa_nracanaa file*, uint64, ចំនួនគត់ n);
ចំនួនគត់             filestat(sa_nracanaa file*, uint64 addr);
ចំនួនគត់             filewrite(sa_nracanaa file*, uint64, ចំនួនគត់ n);

// fs.c
void            fsinit(ចំនួនគត់);
ចំនួនគត់             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
ចំនួនគត់             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
ចំនួនគត់             readi(sa_nracanaa inode*, ចំនួនគត់, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
ចំនួនគត់             writei(sa_nracanaa inode*, ចំនួនគត់, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(ចំនួនគត់);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(ចំនួនគត់, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
ចំនួនគត់             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, ចំនួនគត់);
ចំនួនគត់             piperead(sa_nracanaa pipe*, uint64, ចំនួនគត់);
ចំនួនគត់             pipewrite(sa_nracanaa pipe*, uint64, ចំនួនគត់);

// printk.c
ចំនួនគត់             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
ចំនួនគត់             cpuid(void);
void            kexit(ចំនួនគត់);
ចំនួនគត់             kfork(void);
ចំនួនគត់             growproc(ចំនួនគត់);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
ចំនួនគត់             kkill(ចំនួនគត់);
ចំនួនគត់             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
ចំនួនគត់             kwait(uint64);
void            wakeup(void*);
void            yield(void);
ចំនួនគត់             either_copyout(ចំនួនគត់ user_dst, uint64 dst, void *src, uint64 len);
ចំនួនគត់             either_copyin(void *dst, ចំនួនគត់ user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
ចំនួនគត់             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
ចំនួនគត់             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
ចំនួនគត់             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, ចំនួនគត់, uint);
var_na*           safestrcpy(var_na*, const var_na*, ចំនួនគត់);
ចំនួនគត់             strlen(const var_na*);
ចំនួនគត់             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, ចំនួនគត់);

// syscall.c
void            argint(ចំនួនគត់, ចំនួនគត់*);
ចំនួនគត់             argstr(ចំនួនគត់, var_na*, ចំនួនគត់);
void            argaddr(ចំនួនគត់, uint64 *);
ចំនួនគត់             fetchstr(uint64, var_na*, ចំនួនគត់);
ចំនួនគត់             fetchaddr(uint64, uint64*);
void            syscall();

// trap.c
extern uint     ticks;
void            trapinit(void);
void            trapinithart(void);
extern sa_nracanaa spinlock tickslock;
void            prepare_return(void);

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartwrite(var_na [], ចំនួនគត់);
void            uartputc_sync(ចំនួនគត់);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, ចំនួនគត់);
ចំនួនគត់             mappages(pagetable_t, uint64, uint64, uint64, ចំនួនគត់);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, ចំនួនគត់);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ចំនួនគត់             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, ចំនួនគត់);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ចំនួនគត់);
uint64          walkaddr(pagetable_t, uint64);
ចំនួនគត់             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
ចំនួនគត់             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
ចំនួនគត់             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
ចំនួនគត់             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ចំនួនគត់);

// plic.c
void            plicinit(void);
void            plicinithart(void);
ចំនួនគត់             plic_claim(void);
void            plic_complete(ចំនួនគត់);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, ចំនួនគត់);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
