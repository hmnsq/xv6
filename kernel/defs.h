// clang-format off
काष्ठा buf;
काष्ठा context;
काष्ठा file;
काष्ठा inode;
काष्ठा pipe;
काष्ठा proc;
काष्ठा spinlock;
काष्ठा sleeplock;
काष्ठा stat;
काष्ठा superblock;

// bio.c
व्योम            binit(व्योम);
काष्ठा buf*     bread(uint, uint);
व्योम            brelse(काष्ठा buf*);
व्योम            bwrite(काष्ठा buf*);
व्योम            bpin(काष्ठा buf*);
व्योम            bunpin(काष्ठा buf*);

// console.c
व्योम            consoleinit(व्योम);
व्योम            consoleintr(पूर्णांक);
व्योम            consputc(पूर्णांक);

// exec.c
पूर्णांक             kexec(अक्षर*, अक्षर**);

// file.c
काष्ठा file*    filealloc(व्योम);
व्योम            fileclose(काष्ठा file*);
काष्ठा file*    filedup(काष्ठा file*);
व्योम            fileinit(व्योम);
पूर्णांक             fileread(काष्ठा file*, uint64, पूर्णांक n);
पूर्णांक             filestat(काष्ठा file*, uint64 addr);
पूर्णांक             filewrite(काष्ठा file*, uint64, पूर्णांक n);

// fs.c
व्योम            fsinit(पूर्णांक);
पूर्णांक             dirlink(काष्ठा inode*, अक्षर*, uint);
काष्ठा inode*   dirlookup(काष्ठा inode*, अक्षर*, uint*);
काष्ठा inode*   ialloc(uint, लघु);
काष्ठा inode*   idup(काष्ठा inode*);
व्योम            iinit();
व्योम            ilock(काष्ठा inode*);
व्योम            iput(काष्ठा inode*);
व्योम            iunlock(काष्ठा inode*);
व्योम            iunlockput(काष्ठा inode*);
व्योम            iupdate(काष्ठा inode*);
पूर्णांक             namecmp(स्थिर अक्षर*, स्थिर अक्षर*);
काष्ठा inode*   namei(अक्षर*);
काष्ठा inode*   nameiparent(अक्षर*, अक्षर*);
पूर्णांक             readi(काष्ठा inode*, पूर्णांक, uint64, uint, uint);
व्योम            stati(काष्ठा inode*, काष्ठा stat*);
पूर्णांक             writei(काष्ठा inode*, पूर्णांक, uint64, uint, uint);
व्योम            itrunc(काष्ठा inode*);
व्योम            ireclaim(पूर्णांक);

// kalloc.c
व्योम*           kalloc(व्योम);
व्योम            kfree(व्योम *);
व्योम            kinit(व्योम);

// log.c
व्योम            initlog(पूर्णांक, काष्ठा superblock*);
व्योम            log_write(काष्ठा buf*);
व्योम            begin_op(व्योम);
व्योम            end_op(व्योम);

// pipe.c
पूर्णांक             pipealloc(काष्ठा file**, काष्ठा file**);
व्योम            pipeclose(काष्ठा pipe*, पूर्णांक);
पूर्णांक             piperead(काष्ठा pipe*, uint64, पूर्णांक);
पूर्णांक             pipewrite(काष्ठा pipe*, uint64, पूर्णांक);

// printk.c
पूर्णांक             printk(अक्षर*, ...) __attribute__ ((format (म_लिखो, 1, 2)));
व्योम            panic(अक्षर*) __attribute__((noreturn));
व्योम            printkinit(व्योम);

// proc.c
पूर्णांक             cpuid(व्योम);
व्योम            kexit(पूर्णांक);
पूर्णांक             kfork(व्योम);
पूर्णांक             growproc(पूर्णांक);
व्योम            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(काष्ठा proc *);
व्योम            proc_freepagetable(pagetable_t, uint64);
पूर्णांक             kkill(पूर्णांक);
पूर्णांक             killed(काष्ठा proc*);
व्योम            setkilled(काष्ठा proc*);
काष्ठा cpu*     mycpu(व्योम);
काष्ठा proc*    myproc();
व्योम            procinit(व्योम);
व्योम            scheduler(व्योम) __attribute__((noreturn));
व्योम            sched(व्योम);
व्योम            sleep_prepare(व्योम*);
व्योम            sleep(व्योम);
व्योम            userinit(व्योम);
पूर्णांक             kwait(uint64);
व्योम            wakeup(व्योम*);
व्योम            yield(व्योम);
पूर्णांक             either_copyout(पूर्णांक user_dst, uint64 dst, व्योम *src, uint64 len);
पूर्णांक             either_copyin(व्योम *dst, पूर्णांक user_src, uint64 src, uint64 len);
व्योम            procdump(व्योम);

// swtch.S
व्योम            swtch(काष्ठा context*, काष्ठा context*);

// spinlock.c
व्योम            acquire(काष्ठा spinlock*);
पूर्णांक             holding(काष्ठा spinlock*);
व्योम            initlock(काष्ठा spinlock*, अक्षर*);
व्योम            release(काष्ठा spinlock*);
व्योम            धकेलो_off(व्योम);
व्योम            निकालो_off(व्योम);

// sleeplock.c
व्योम            acquiresleep(काष्ठा sleeplock*);
व्योम            releasesleep(काष्ठा sleeplock*);
पूर्णांक             holdingsleep(काष्ठा sleeplock*);
व्योम            initsleeplock(काष्ठा sleeplock*, अक्षर*);

// string.c
पूर्णांक             स_भेद(स्थिर व्योम*, स्थिर व्योम*, uint);
व्योम*           स_हटाओ(व्योम*, स्थिर व्योम*, uint);
व्योम*           स_रखो(व्योम*, पूर्णांक, uint);
अक्षर*           safestrcpy(अक्षर*, स्थिर अक्षर*, पूर्णांक);
पूर्णांक             म_माप(स्थिर अक्षर*);
पूर्णांक             म_भेदन(स्थिर अक्षर*, स्थिर अक्षर*, uint);
अक्षर*           म_नकलन(अक्षर*, स्थिर अक्षर*, पूर्णांक);

// syscall.c
व्योम            argint(पूर्णांक, पूर्णांक*);
पूर्णांक             argstr(पूर्णांक, अक्षर*, पूर्णांक);
व्योम            argaddr(पूर्णांक, uint64 *);
पूर्णांक             fetchstr(uint64, अक्षर*, पूर्णांक);
पूर्णांक             fetchaddr(uint64, uint64*);
व्योम            syscall();

// trap.c
बाह्य uint     ticks;
व्योम            trapinit(व्योम);
व्योम            trapinithart(व्योम);
बाह्य काष्ठा spinlock tickslock;
व्योम            prepare_return(व्योम);

// uart.c
व्योम            uartinit(व्योम);
व्योम            uartintr(व्योम);
व्योम            uartwrite(अक्षर [], पूर्णांक);
व्योम            uartputc_sync(पूर्णांक);

// vm.c
व्योम            kvminit(व्योम);
व्योम            kvminithart(व्योम);
व्योम            kvmmap(pagetable_t, uint64, uint64, uint64, पूर्णांक);
पूर्णांक             mappages(pagetable_t, uint64, uint64, uint64, पूर्णांक);
pagetable_t     uvmcreate(व्योम);
uint64          uvmalloc(pagetable_t, uint64, uint64, पूर्णांक);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
पूर्णांक             uvmcopy(pagetable_t, pagetable_t, uint64);
व्योम            uvmfree(pagetable_t, uint64);
व्योम            uvmunmap(pagetable_t, uint64, uint64, पूर्णांक);
व्योम            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, पूर्णांक);
uint64          walkaddr(pagetable_t, uint64);
पूर्णांक             copyout(pagetable_t, uint64, uint64, अक्षर *, uint64);
पूर्णांक             copyin(pagetable_t, uint64, अक्षर *, uint64, uint64);
पूर्णांक             copyinstr(pagetable_t, uint64, अक्षर *, uint64, uint64);
पूर्णांक             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, पूर्णांक);

// plic.c
व्योम            plicinit(व्योम);
व्योम            plicinithart(व्योम);
पूर्णांक             plic_claim(व्योम);
व्योम            plic_complete(पूर्णांक);

// virtio_disk.c
व्योम            virtio_disk_init(व्योम);
व्योम            virtio_disk_rw(काष्ठा buf *, पूर्णांक);
व्योम            virtio_disk_intr(व्योम);

// number of elements in fixed-size array
#घोषणा NELEM(x) (माप(x) / माप((x)[0]))
