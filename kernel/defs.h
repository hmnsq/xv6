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
void            consoleintr(ကိန်းပြည့်);
void            consputc(ကိန်းပြည့်);

// exec.c
ကိန်းပြည့်             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
ကိန်းပြည့်             fileread(sa_nracanaa file*, uint64, ကိန်းပြည့် n);
ကိန်းပြည့်             filestat(sa_nracanaa file*, uint64 addr);
ကိန်းပြည့်             filewrite(sa_nracanaa file*, uint64, ကိန်းပြည့် n);

// fs.c
void            fsinit(ကိန်းပြည့်);
ကိန်းပြည့်             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
ကိန်းပြည့်             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
ကိန်းပြည့်             readi(sa_nracanaa inode*, ကိန်းပြည့်, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
ကိန်းပြည့်             writei(sa_nracanaa inode*, ကိန်းပြည့်, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(ကိန်းပြည့်);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(ကိန်းပြည့်, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
ကိန်းပြည့်             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, ကိန်းပြည့်);
ကိန်းပြည့်             piperead(sa_nracanaa pipe*, uint64, ကိန်းပြည့်);
ကိန်းပြည့်             pipewrite(sa_nracanaa pipe*, uint64, ကိန်းပြည့်);

// printk.c
ကိန်းပြည့်             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
ကိန်းပြည့်             cpuid(void);
void            kexit(ကိန်းပြည့်);
ကိန်းပြည့်             kfork(void);
ကိန်းပြည့်             growproc(ကိန်းပြည့်);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
ကိန်းပြည့်             kkill(ကိန်းပြည့်);
ကိန်းပြည့်             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
ကိန်းပြည့်             kwait(uint64);
void            wakeup(void*);
void            yield(void);
ကိန်းပြည့်             either_copyout(ကိန်းပြည့် user_dst, uint64 dst, void *src, uint64 len);
ကိန်းပြည့်             either_copyin(void *dst, ကိန်းပြည့် user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
ကိန်းပြည့်             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
ကိန်းပြည့်             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
ကိန်းပြည့်             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, ကိန်းပြည့်, uint);
var_na*           safestrcpy(var_na*, const var_na*, ကိန်းပြည့်);
ကိန်းပြည့်             strlen(const var_na*);
ကိန်းပြည့်             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, ကိန်းပြည့်);

// syscall.c
void            argint(ကိန်းပြည့်, ကိန်းပြည့်*);
ကိန်းပြည့်             argstr(ကိန်းပြည့်, var_na*, ကိန်းပြည့်);
void            argaddr(ကိန်းပြည့်, uint64 *);
ကိန်းပြည့်             fetchstr(uint64, var_na*, ကိန်းပြည့်);
ကိန်းပြည့်             fetchaddr(uint64, uint64*);
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
void            uartwrite(var_na [], ကိန်းပြည့်);
void            uartputc_sync(ကိန်းပြည့်);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, ကိန်းပြည့်);
ကိန်းပြည့်             mappages(pagetable_t, uint64, uint64, uint64, ကိန်းပြည့်);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, ကိန်းပြည့်);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ကိန်းပြည့်             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, ကိန်းပြည့်);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ကိန်းပြည့်);
uint64          walkaddr(pagetable_t, uint64);
ကိန်းပြည့်             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
ကိန်းပြည့်             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
ကိန်းပြည့်             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
ကိန်းပြည့်             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ကိန်းပြည့်);

// plic.c
void            plicinit(void);
void            plicinithart(void);
ကိန်းပြည့်             plic_claim(void);
void            plic_complete(ကိန်းပြည့်);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, ကိန်းပြည့်);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
