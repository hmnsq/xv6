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
void            consoleintr(số_nguyên);
void            consputc(số_nguyên);

// exec.c
số_nguyên             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
số_nguyên             fileread(struct file*, uint64, số_nguyên n);
số_nguyên             filestat(struct file*, uint64 addr);
số_nguyên             filewrite(struct file*, uint64, số_nguyên n);

// fs.c
void            fsinit(số_nguyên);
số_nguyên             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
số_nguyên             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
số_nguyên             readi(struct inode*, số_nguyên, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
số_nguyên             writei(struct inode*, số_nguyên, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(số_nguyên);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(số_nguyên, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
số_nguyên             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, số_nguyên);
số_nguyên             piperead(struct pipe*, uint64, số_nguyên);
số_nguyên             pipewrite(struct pipe*, uint64, số_nguyên);

// printk.c
số_nguyên             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
số_nguyên             cpuid(void);
void            kexit(số_nguyên);
số_nguyên             kfork(void);
số_nguyên             growproc(số_nguyên);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
số_nguyên             kkill(số_nguyên);
số_nguyên             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
số_nguyên             kwait(uint64);
void            wakeup(void*);
void            yield(void);
số_nguyên             either_copyout(số_nguyên user_dst, uint64 dst, void *src, uint64 len);
số_nguyên             either_copyin(void *dst, số_nguyên user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
số_nguyên             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
số_nguyên             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
số_nguyên             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, số_nguyên, uint);
char*           safestrcpy(char*, const char*, số_nguyên);
số_nguyên             strlen(const char*);
số_nguyên             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, số_nguyên);

// syscall.c
void            argint(số_nguyên, số_nguyên*);
số_nguyên             argstr(số_nguyên, char*, số_nguyên);
void            argaddr(số_nguyên, uint64 *);
số_nguyên             fetchstr(uint64, char*, số_nguyên);
số_nguyên             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], số_nguyên);
void            uartputc_sync(số_nguyên);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, số_nguyên);
số_nguyên             mappages(pagetable_t, uint64, uint64, uint64, số_nguyên);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, số_nguyên);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
số_nguyên             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, số_nguyên);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, số_nguyên);
uint64          walkaddr(pagetable_t, uint64);
số_nguyên             copyout(pagetable_t, uint64, uint64, char *, uint64);
số_nguyên             copyin(pagetable_t, uint64, char *, uint64, uint64);
số_nguyên             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
số_nguyên             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, số_nguyên);

// plic.c
void            plicinit(void);
void            plicinithart(void);
số_nguyên             plic_claim(void);
void            plic_complete(số_nguyên);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, số_nguyên);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
