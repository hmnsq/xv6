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
void            consoleintr(puur_naa_nka);
void            consputc(puur_naa_nka);

// exec.c
puur_naa_nka             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
puur_naa_nka             fileread(sa_nracanaa file*, uint64, puur_naa_nka n);
puur_naa_nka             filestat(sa_nracanaa file*, uint64 addr);
puur_naa_nka             filewrite(sa_nracanaa file*, uint64, puur_naa_nka n);

// fs.c
void            fsinit(puur_naa_nka);
puur_naa_nka             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
puur_naa_nka             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
puur_naa_nka             readi(sa_nracanaa inode*, puur_naa_nka, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
puur_naa_nka             writei(sa_nracanaa inode*, puur_naa_nka, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(puur_naa_nka);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(puur_naa_nka, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
puur_naa_nka             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, puur_naa_nka);
puur_naa_nka             piperead(sa_nracanaa pipe*, uint64, puur_naa_nka);
puur_naa_nka             pipewrite(sa_nracanaa pipe*, uint64, puur_naa_nka);

// printk.c
puur_naa_nka             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
puur_naa_nka             cpuid(void);
void            kexit(puur_naa_nka);
puur_naa_nka             kfork(void);
puur_naa_nka             growproc(puur_naa_nka);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
puur_naa_nka             kkill(puur_naa_nka);
puur_naa_nka             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
puur_naa_nka             kwait(uint64);
void            wakeup(void*);
void            yield(void);
puur_naa_nka             either_copyout(puur_naa_nka user_dst, uint64 dst, void *src, uint64 len);
puur_naa_nka             either_copyin(void *dst, puur_naa_nka user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
puur_naa_nka             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
puur_naa_nka             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
puur_naa_nka             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, puur_naa_nka, uint);
var_na*           safestrcpy(var_na*, const var_na*, puur_naa_nka);
puur_naa_nka             strlen(const var_na*);
puur_naa_nka             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, puur_naa_nka);

// syscall.c
void            argint(puur_naa_nka, puur_naa_nka*);
puur_naa_nka             argstr(puur_naa_nka, var_na*, puur_naa_nka);
void            argaddr(puur_naa_nka, uint64 *);
puur_naa_nka             fetchstr(uint64, var_na*, puur_naa_nka);
puur_naa_nka             fetchaddr(uint64, uint64*);
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
void            uartwrite(var_na [], puur_naa_nka);
void            uartputc_sync(puur_naa_nka);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, puur_naa_nka);
puur_naa_nka             mappages(pagetable_t, uint64, uint64, uint64, puur_naa_nka);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, puur_naa_nka);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
puur_naa_nka             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, puur_naa_nka);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, puur_naa_nka);
uint64          walkaddr(pagetable_t, uint64);
puur_naa_nka             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
puur_naa_nka             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
puur_naa_nka             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
puur_naa_nka             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, puur_naa_nka);

// plic.c
void            plicinit(void);
void            plicinithart(void);
puur_naa_nka             plic_claim(void);
void            plic_complete(puur_naa_nka);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, puur_naa_nka);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
