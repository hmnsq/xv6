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
void            consoleintr(จำนวนเต็ม);
void            consputc(จำนวนเต็ม);

// exec.c
จำนวนเต็ม             kexec(var_na*, var_na**);

// file.c
sa_nracanaa file*    filealloc(void);
void            fileclose(sa_nracanaa file*);
sa_nracanaa file*    filedup(sa_nracanaa file*);
void            fileinit(void);
จำนวนเต็ม             fileread(sa_nracanaa file*, uint64, จำนวนเต็ม n);
จำนวนเต็ม             filestat(sa_nracanaa file*, uint64 addr);
จำนวนเต็ม             filewrite(sa_nracanaa file*, uint64, จำนวนเต็ม n);

// fs.c
void            fsinit(จำนวนเต็ม);
จำนวนเต็ม             dirlink(sa_nracanaa inode*, var_na*, uint);
sa_nracanaa inode*   dirlookup(sa_nracanaa inode*, var_na*, uint*);
sa_nracanaa inode*   ialloc(uint, short);
sa_nracanaa inode*   idup(sa_nracanaa inode*);
void            iinit();
void            ilock(sa_nracanaa inode*);
void            iput(sa_nracanaa inode*);
void            iunlock(sa_nracanaa inode*);
void            iunlockput(sa_nracanaa inode*);
void            iupdate(sa_nracanaa inode*);
จำนวนเต็ม             namecmp(const var_na*, const var_na*);
sa_nracanaa inode*   namei(var_na*);
sa_nracanaa inode*   nameiparent(var_na*, var_na*);
จำนวนเต็ม             readi(sa_nracanaa inode*, จำนวนเต็ม, uint64, uint, uint);
void            stati(sa_nracanaa inode*, sa_nracanaa stat*);
จำนวนเต็ม             writei(sa_nracanaa inode*, จำนวนเต็ม, uint64, uint, uint);
void            itrunc(sa_nracanaa inode*);
void            ireclaim(จำนวนเต็ม);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(จำนวนเต็ม, sa_nracanaa superblock*);
void            log_write(sa_nracanaa buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
จำนวนเต็ม             pipealloc(sa_nracanaa file**, sa_nracanaa file**);
void            pipeclose(sa_nracanaa pipe*, จำนวนเต็ม);
จำนวนเต็ม             piperead(sa_nracanaa pipe*, uint64, จำนวนเต็ม);
จำนวนเต็ม             pipewrite(sa_nracanaa pipe*, uint64, จำนวนเต็ม);

// printk.c
จำนวนเต็ม             printk(var_na*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(var_na*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
จำนวนเต็ม             cpuid(void);
void            kexit(จำนวนเต็ม);
จำนวนเต็ม             kfork(void);
จำนวนเต็ม             growproc(จำนวนเต็ม);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(sa_nracanaa proc *);
void            proc_freepagetable(pagetable_t, uint64);
จำนวนเต็ม             kkill(จำนวนเต็ม);
จำนวนเต็ม             killed(sa_nracanaa proc*);
void            setkilled(sa_nracanaa proc*);
sa_nracanaa cpu*     mycpu(void);
sa_nracanaa proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
จำนวนเต็ม             kwait(uint64);
void            wakeup(void*);
void            yield(void);
จำนวนเต็ม             either_copyout(จำนวนเต็ม user_dst, uint64 dst, void *src, uint64 len);
จำนวนเต็ม             either_copyin(void *dst, จำนวนเต็ม user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(sa_nracanaa context*, sa_nracanaa context*);

// spinlock.c
void            acquire(sa_nracanaa spinlock*);
จำนวนเต็ม             holding(sa_nracanaa spinlock*);
void            initlock(sa_nracanaa spinlock*, var_na*);
void            release(sa_nracanaa spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(sa_nracanaa sleeplock*);
void            releasesleep(sa_nracanaa sleeplock*);
จำนวนเต็ม             holdingsleep(sa_nracanaa sleeplock*);
void            initsleeplock(sa_nracanaa sleeplock*, var_na*);

// string.c
จำนวนเต็ม             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, จำนวนเต็ม, uint);
var_na*           safestrcpy(var_na*, const var_na*, จำนวนเต็ม);
จำนวนเต็ม             strlen(const var_na*);
จำนวนเต็ม             strncmp(const var_na*, const var_na*, uint);
var_na*           strncpy(var_na*, const var_na*, จำนวนเต็ม);

// syscall.c
void            argint(จำนวนเต็ม, จำนวนเต็ม*);
จำนวนเต็ม             argstr(จำนวนเต็ม, var_na*, จำนวนเต็ม);
void            argaddr(จำนวนเต็ม, uint64 *);
จำนวนเต็ม             fetchstr(uint64, var_na*, จำนวนเต็ม);
จำนวนเต็ม             fetchaddr(uint64, uint64*);
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
void            uartwrite(var_na [], จำนวนเต็ม);
void            uartputc_sync(จำนวนเต็ม);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, จำนวนเต็ม);
จำนวนเต็ม             mappages(pagetable_t, uint64, uint64, uint64, จำนวนเต็ม);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, จำนวนเต็ม);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
จำนวนเต็ม             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, จำนวนเต็ม);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, จำนวนเต็ม);
uint64          walkaddr(pagetable_t, uint64);
จำนวนเต็ม             copyout(pagetable_t, uint64, uint64, var_na *, uint64);
จำนวนเต็ม             copyin(pagetable_t, uint64, var_na *, uint64, uint64);
จำนวนเต็ม             copyinstr(pagetable_t, uint64, var_na *, uint64, uint64);
จำนวนเต็ม             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, จำนวนเต็ม);

// plic.c
void            plicinit(void);
void            plicinithart(void);
จำนวนเต็ม             plic_claim(void);
void            plic_complete(จำนวนเต็ม);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(sa_nracanaa buf *, จำนวนเต็ม);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (parimaa_na(x) / parimaa_na((x)[0]))
