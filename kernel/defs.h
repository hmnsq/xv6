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
void            consoleintr(ཧྲིལ);
void            consputc(ཧྲིལ);

// exec.c
ཧྲིལ             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
ཧྲིལ             fileread(sa_nracanaa file*, uint64, ཧྲིལ n);
ཧྲིལ             filestat(sa_nracanaa file*, uint64 addr);
ཧྲིལ             filewrite(sa_nracanaa file*, uint64, ཧྲིལ n);

// fs.c
void            fsinit(ཧྲིལ);
ཧྲིལ             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
ཧྲིལ             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
ཧྲིལ             readi(sa_nracanaa inode*, ཧྲིལ, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
ཧྲིལ             writei(sa_nracanaa inode*, ཧྲིལ, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(ཧྲིལ);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(ཧྲིལ, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
ཧྲིལ             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, ཧྲིལ);
ཧྲིལ             piperead(sa_nracanaa pipe*, uint64, ཧྲིལ);
ཧྲིལ             pipewrite(sa_nracanaa pipe*, uint64, ཧྲིལ);

// printk.c
ཧྲིལ             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
ཧྲིལ             cpuid(void);
void            kexit(ཧྲིལ);
ཧྲིལ             kfork(void);
ཧྲིལ             growproc(ཧྲིལ);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
ཧྲིལ             kkill(ཧྲིལ);
ཧྲིལ             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
ཧྲིལ             kwait(uint64);
void            wakeup(void*);
void            yield(void);
ཧྲིལ             either_copyout(ཧྲིལ user_dst, uint64 dst, void *src, uint64 len);
ཧྲིལ             either_copyin(void *dst, ཧྲིལ user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
ཧྲིལ             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
ཧྲིལ             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
ཧྲིལ             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, ཧྲིལ, uint);
var_na*           safestrcpy(var_na*, const var_na*, ཧྲིལ);
ཧྲིལ             strlen(const var_na*);
ཧྲིལ             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, ཧྲིལ);

// syscall.c
void            argint(ཧྲིལ, ཧྲིལ*);
ཧྲིལ             argstr(ཧྲིལ, var_na*, ཧྲིལ);
void            argaddr(ཧྲིལ, uint64 *);
ཧྲིལ             fetchstr(uint64, var_na*, ཧྲིལ);
ཧྲིལ             fetchaddr(uint64, uint64*);
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
void            uartwrite(var_na [], ཧྲིལ);
void            uartputc_sync(ཧྲིལ);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, ཧྲིལ);
ཧྲིལ             mappages(pagetable_t, uint64, uint64, uint64, ཧྲིལ);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, ཧྲིལ);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ཧྲིལ             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, ཧྲིལ);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ཧྲིལ);
uint64          walkaddr(pagetable_t, uint64);
ཧྲིལ             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
ཧྲིལ             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
ཧྲིལ             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
ཧྲིལ             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ཧྲིལ);

// plic.c
void            plicinit(void);
void            plicinithart(void);
ཧྲིལ             plic_claim(void);
void            plic_complete(ཧྲིལ);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, ཧྲིལ);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
