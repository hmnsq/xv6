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
void            consoleintr(정수);
void            consputc(정수);

// exec.c
정수             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
정수             fileread(struct file*, uint64, 정수 n);
정수             filestat(struct file*, uint64 addr);
정수             filewrite(struct file*, uint64, 정수 n);

// fs.c
void            fsinit(정수);
정수             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
정수             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
정수             readi(struct inode*, 정수, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
정수             writei(struct inode*, 정수, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(정수);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(정수, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
정수             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, 정수);
정수             piperead(struct pipe*, uint64, 정수);
정수             pipewrite(struct pipe*, uint64, 정수);

// printk.c
정수             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
정수             cpuid(void);
void            kexit(정수);
정수             kfork(void);
정수             growproc(정수);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
정수             kkill(정수);
정수             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
정수             kwait(uint64);
void            wakeup(void*);
void            yield(void);
정수             either_copyout(정수 user_dst, uint64 dst, void *src, uint64 len);
정수             either_copyin(void *dst, 정수 user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
정수             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
정수             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
정수             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, 정수, uint);
char*           safestrcpy(char*, const char*, 정수);
정수             strlen(const char*);
정수             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, 정수);

// syscall.c
void            argint(정수, 정수*);
정수             argstr(정수, char*, 정수);
void            argaddr(정수, uint64 *);
정수             fetchstr(uint64, char*, 정수);
정수             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], 정수);
void            uartputc_sync(정수);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, 정수);
정수             mappages(pagetable_t, uint64, uint64, uint64, 정수);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, 정수);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
정수             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, 정수);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, 정수);
uint64          walkaddr(pagetable_t, uint64);
정수             copyout(pagetable_t, uint64, uint64, char *, uint64);
정수             copyin(pagetable_t, uint64, char *, uint64, uint64);
정수             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
정수             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, 정수);

// plic.c
void            plicinit(void);
void            plicinithart(void);
정수             plic_claim(void);
void            plic_complete(정수);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, 정수);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
