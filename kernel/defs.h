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
void            consoleintr(ціле);
void            consputc(ціле);

// exec.c
ціле             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
ціле             fileread(struct file*, uint64, ціле n);
ціле             filestat(struct file*, uint64 addr);
ціле             filewrite(struct file*, uint64, ціле n);

// fs.c
void            fsinit(ціле);
ціле             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
ціле             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
ціле             readi(struct inode*, ціле, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
ціле             writei(struct inode*, ціле, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(ціле);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(ціле, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
ціле             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, ціле);
ціле             piperead(struct pipe*, uint64, ціле);
ціле             pipewrite(struct pipe*, uint64, ціле);

// printk.c
ціле             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
ціле             cpuid(void);
void            kexit(ціле);
ціле             kfork(void);
ціле             growproc(ціле);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
ціле             kkill(ціле);
ціле             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
ціле             kwait(uint64);
void            wakeup(void*);
void            yield(void);
ціле             either_copyout(ціле user_dst, uint64 dst, void *src, uint64 len);
ціле             either_copyin(void *dst, ціле user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
ціле             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
ціле             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
ціле             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, ціле, uint);
char*           safestrcpy(char*, const char*, ціле);
ціле             strlen(const char*);
ціле             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, ціле);

// syscall.c
void            argint(ціле, ціле*);
ціле             argstr(ціле, char*, ціле);
void            argaddr(ціле, uint64 *);
ціле             fetchstr(uint64, char*, ціле);
ціле             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], ціле);
void            uartputc_sync(ціле);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, ціле);
ціле             mappages(pagetable_t, uint64, uint64, uint64, ціле);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, ціле);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
ціле             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, ціле);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, ціле);
uint64          walkaddr(pagetable_t, uint64);
ціле             copyout(pagetable_t, uint64, uint64, char *, uint64);
ціле             copyin(pagetable_t, uint64, char *, uint64, uint64);
ціле             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
ціле             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, ціле);

// plic.c
void            plicinit(void);
void            plicinithart(void);
ціле             plic_claim(void);
void            plic_complete(ціле);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, ціле);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
