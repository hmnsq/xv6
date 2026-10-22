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
void            consoleintr(整数);
void            consputc(整数);

// exec.c
整数             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
整数             fileread(struct file*, uint64, 整数 n);
整数             filestat(struct file*, uint64 addr);
整数             filewrite(struct file*, uint64, 整数 n);

// fs.c
void            fsinit(整数);
整数             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
整数             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
整数             readi(struct inode*, 整数, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
整数             writei(struct inode*, 整数, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(整数);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(整数, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
整数             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, 整数);
整数             piperead(struct pipe*, uint64, 整数);
整数             pipewrite(struct pipe*, uint64, 整数);

// printk.c
整数             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
整数             cpuid(void);
void            kexit(整数);
整数             kfork(void);
整数             growproc(整数);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
整数             kkill(整数);
整数             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
整数             kwait(uint64);
void            wakeup(void*);
void            yield(void);
整数             either_copyout(整数 user_dst, uint64 dst, void *src, uint64 len);
整数             either_copyin(void *dst, 整数 user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
整数             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
整数             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
整数             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, 整数, uint);
char*           safestrcpy(char*, const char*, 整数);
整数             strlen(const char*);
整数             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, 整数);

// syscall.c
void            argint(整数, 整数*);
整数             argstr(整数, char*, 整数);
void            argaddr(整数, uint64 *);
整数             fetchstr(uint64, char*, 整数);
整数             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], 整数);
void            uartputc_sync(整数);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, 整数);
整数             mappages(pagetable_t, uint64, uint64, uint64, 整数);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, 整数);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
整数             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, 整数);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, 整数);
uint64          walkaddr(pagetable_t, uint64);
整数             copyout(pagetable_t, uint64, uint64, char *, uint64);
整数             copyin(pagetable_t, uint64, char *, uint64, uint64);
整数             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
整数             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, 整数);

// plic.c
void            plicinit(void);
void            plicinithart(void);
整数             plic_claim(void);
void            plic_complete(整数);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, 整数);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
