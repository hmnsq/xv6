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
void            consoleintr(පූර්ණ_සංඛ්‍යා);
void            consputc(පූර්ණ_සංඛ්‍යා);

// exec.c
පූර්ණ_සංඛ්‍යා             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
පූර්ණ_සංඛ්‍යා             fileread(sa_nracanaa file*, uint64, පූර්ණ_සංඛ්‍යා n);
පූර්ණ_සංඛ්‍යා             filestat(sa_nracanaa file*, uint64 addr);
පූර්ණ_සංඛ්‍යා             filewrite(sa_nracanaa file*, uint64, පූර්ණ_සංඛ්‍යා n);

// fs.c
void            fsinit(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
පූර්ණ_සංඛ්‍යා             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
පූර්ණ_සංඛ්‍යා             readi(sa_nracanaa inode*, පූර්ණ_සංඛ්‍යා, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
පූර්ණ_සංඛ්‍යා             writei(sa_nracanaa inode*, පූර්ණ_සංඛ්‍යා, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(පූර්ණ_සංඛ්‍යා);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(පූර්ණ_සංඛ්‍යා, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
පූර්ණ_සංඛ්‍යා             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             piperead(sa_nracanaa pipe*, uint64, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             pipewrite(sa_nracanaa pipe*, uint64, පූර්ණ_සංඛ්‍යා);

// printk.c
පූර්ණ_සංඛ්‍යා             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
පූර්ණ_සංඛ්‍යා             cpuid(void);
void            kexit(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             kfork(void);
පූර්ණ_සංඛ්‍යා             growproc(පූර්ණ_සංඛ්‍යා);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
පූර්ණ_සංඛ්‍යා             kkill(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
පූර්ණ_සංඛ්‍යා             kwait(uint64);
void            wakeup(void*);
void            yield(void);
පූර්ණ_සංඛ්‍යා             either_copyout(පූර්ණ_සංඛ්‍යා user_dst, uint64 dst, void *src, uint64 len);
පූර්ණ_සංඛ්‍යා             either_copyin(void *dst, පූර්ණ_සංඛ්‍යා user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
පූර්ණ_සංඛ්‍යා             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
පූර්ණ_සංඛ්‍යා             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
පූර්ණ_සංඛ්‍යා             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, පූර්ණ_සංඛ්‍යා, uint);
var_na*           safestrcpy(var_na*, const var_na*, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             strlen(const var_na*);
පූර්ණ_සංඛ්‍යා             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, පූර්ණ_සංඛ්‍යා);

// syscall.c
void            argint(පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා*);
පූර්ණ_සංඛ්‍යා             argstr(පූර්ණ_සංඛ්‍යා, var_na*, පූර්ණ_සංඛ්‍යා);
void            argaddr(පූර්ණ_සංඛ්‍යා, uint64 *);
පූර්ණ_සංඛ්‍යා             fetchstr(uint64, var_na*, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             fetchaddr(uint64, uint64*);
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
void            uartwrite(var_na [], පූර්ණ_සංඛ්‍යා);
void            uartputc_sync(පූර්ණ_සංඛ්‍යා);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා             mappages(pagetable_t, uint64, uint64, uint64, පූර්ණ_සංඛ්‍යා);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, පූර්ණ_සංඛ්‍යා);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
පූර්ණ_සංඛ්‍යා             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, පූර්ණ_සංඛ්‍යා);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, පූර්ණ_සංඛ්‍යා);
uint64          walkaddr(pagetable_t, uint64);
පූර්ණ_සංඛ්‍යා             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
පූර්ණ_සංඛ්‍යා             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
පූර්ණ_සංඛ්‍යා             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
පූර්ණ_සංඛ්‍යා             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, පූර්ණ_සංඛ්‍යා);

// plic.c
void            plicinit(void);
void            plicinithart(void);
පූර්ණ_සංඛ්‍යා             plic_claim(void);
void            plic_complete(පූර්ණ_සංඛ්‍යා);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, පූර්ණ_සංඛ්‍යා);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
