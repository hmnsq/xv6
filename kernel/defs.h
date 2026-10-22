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
void            consoleintr(целое);
void            consputc(целое);

// exec.c
целое             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
целое             fileread(struct file*, uint64, целое n);
целое             filestat(struct file*, uint64 addr);
целое             filewrite(struct file*, uint64, целое n);

// fs.c
void            fsinit(целое);
целое             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
целое             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
целое             readi(struct inode*, целое, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
целое             writei(struct inode*, целое, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(целое);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(целое, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
целое             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, целое);
целое             piperead(struct pipe*, uint64, целое);
целое             pipewrite(struct pipe*, uint64, целое);

// printk.c
целое             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
целое             cpuid(void);
void            kexit(целое);
целое             kfork(void);
целое             growproc(целое);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
целое             kkill(целое);
целое             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
целое             kwait(uint64);
void            wakeup(void*);
void            yield(void);
целое             either_copyout(целое user_dst, uint64 dst, void *src, uint64 len);
целое             either_copyin(void *dst, целое user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
целое             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
целое             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
целое             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, целое, uint);
char*           safestrcpy(char*, const char*, целое);
целое             strlen(const char*);
целое             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, целое);

// syscall.c
void            argint(целое, целое*);
целое             argstr(целое, char*, целое);
void            argaddr(целое, uint64 *);
целое             fetchstr(uint64, char*, целое);
целое             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], целое);
void            uartputc_sync(целое);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, целое);
целое             mappages(pagetable_t, uint64, uint64, uint64, целое);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, целое);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
целое             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, целое);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, целое);
uint64          walkaddr(pagetable_t, uint64);
целое             copyout(pagetable_t, uint64, uint64, char *, uint64);
целое             copyin(pagetable_t, uint64, char *, uint64, uint64);
целое             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
целое             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, целое);

// plic.c
void            plicinit(void);
void            plicinithart(void);
целое             plic_claim(void);
void            plic_complete(целое);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, целое);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
