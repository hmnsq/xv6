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
void            consoleintr(цео);
void            consputc(цео);

// exec.c
цео             kexec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
цео             fileread(struct file*, uint64, цео n);
цео             filestat(struct file*, uint64 addr);
цео             filewrite(struct file*, uint64, цео n);

// fs.c
void            fsinit(цео);
цео             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
цео             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
цео             readi(struct inode*, цео, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
цео             writei(struct inode*, цео, uint64, uint, uint);
void            itrunc(struct inode*);
void            ireclaim(цео);

// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);

// log.c
void            initlog(цео, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// pipe.c
цео             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, цео);
цео             piperead(struct pipe*, uint64, цео);
цео             pipewrite(struct pipe*, uint64, цео);

// printk.c
цео             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

// proc.c
цео             cpuid(void);
void            kexit(цео);
цео             kfork(void);
цео             growproc(цео);
void            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
цео             kkill(цео);
цео             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep_prepare(void*);
void            sleep(void);
void            userinit(void);
цео             kwait(uint64);
void            wakeup(void*);
void            yield(void);
цео             either_copyout(цео user_dst, uint64 dst, void *src, uint64 len);
цео             either_copyin(void *dst, цео user_src, uint64 src, uint64 len);
void            procdump(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
цео             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
цео             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
цео             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, цео, uint);
char*           safestrcpy(char*, const char*, цео);
цео             strlen(const char*);
цео             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, цео);

// syscall.c
void            argint(цео, цео*);
цео             argstr(цео, char*, цео);
void            argaddr(цео, uint64 *);
цео             fetchstr(uint64, char*, цео);
цео             fetchaddr(uint64, uint64*);
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
void            uartwrite(char [], цео);
void            uartputc_sync(цео);

// vm.c
void            kvminit(void);
void            kvminithart(void);
void            kvmmap(pagetable_t, uint64, uint64, uint64, цео);
цео             mappages(pagetable_t, uint64, uint64, uint64, цео);
pagetable_t     uvmcreate(void);
uint64          uvmalloc(pagetable_t, uint64, uint64, цео);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
цео             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, цео);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, цео);
uint64          walkaddr(pagetable_t, uint64);
цео             copyout(pagetable_t, uint64, uint64, char *, uint64);
цео             copyin(pagetable_t, uint64, char *, uint64, uint64);
цео             copyinstr(pagetable_t, uint64, char *, uint64, uint64);
цео             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, цео);

// plic.c
void            plicinit(void);
void            plicinithart(void);
цео             plic_claim(void);
void            plic_complete(цео);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, цео);
void            virtio_disk_intr(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
