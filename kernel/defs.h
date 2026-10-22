// clang-format off
struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);
void            bpin(struct buf*);
void            bunpin(struct buf*);

// console.c
void            consoleinit(void);
void            consoleintr(цяло);
void            consputc(цяло);

// exec.c
цяло             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
цяло             fileread(struct file*, uint64, цяло n);
цяло             filestat(struct file*, uint64 addr);
цяло             filewrite(struct file*, uint64, цяло n);

// fs.c
void            fsinit(цяло);
цяло             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
цяло             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
цяло             readi(struct inode*, цяло, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
цяло             writei(struct inode*, цяло, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(цяло);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(цяло, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
цяло             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, цяло);
цяло             piperead(struct pipe*, uint64, цяло);
цяло             pipewrite(struct pipe*, uint64, цяло);

// printk.c
цяло             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
цяло             cpuid(void);
void            kexit(цяло);
цяло             kfork(void);
цяло             growproc(цяло);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
цяло             kkill(цяло);
цяло             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
цяло             kwait(uint64);
void            wakeup(void*);
void            yield(void);
цяло             either_copyout(цяло user_dst, uint64 dst, void *src, uint64 len);
цяло             either_copyin(void *dst, цяло user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
цяло             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
цяло             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
цяло             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, цяло, uint);
char*           safestrcpy(char*, const char*, цяло);
цяло             strlen(const char*);
цяло             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, цяло);

// syscall.c
void            argint(цяло, цяло*);
цяло             argstr(цяло, char*, цяло);
void            argaddr(цяло, uint64 *);
цяло             fetchstr(uint64, char*, цяло);
цяло             fetchaddr(uint64, uint64*);
void            syscall();

// trap.c
extern uint     ticks;
void            trapinit(void);
void            trapinithart(void);
extern struct spinlock tickslock;
void            prepare_return(void);

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartwrite(char [], цяло);
void            uartputc_sync(цяло);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, цяло);
цяло             mappages(pagetable_t, uint64, uint64, uint64, цяло);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, цяло);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
цяло             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, цяло);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, цяло);
uint64          walkaddr(pagetable_t, uint64);
цяло             copyout(pagetable_t, uint64, uint64, char *, uint64);
цяло             copyin(pagetable_t, uint64, char *, uint64, uint64);
цяло             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
цяло             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, цяло);

// plic.c
void            plicinit(void);
void            plicinithart(void);
цяло             plic_claim(void);
void            plic_complete(цяло);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, цяло);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
