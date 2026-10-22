// clang-format off
संरचना buf;
संरचना context;
संरचना file;
संरचना inode;
संरचना pipe;
संरचना proc;
संरचना spinlock;
संरचना sleeplock;
संरचना stat;
संरचना superblock;

// bio.c
शून्य            binit(शून्य);
संरचना buf*     bread(uint, uint);
शून्य            brelse(संरचना buf*);
शून्य            bwrite(संरचना buf*);
शून्य            bpin(संरचना buf*);
शून्य            bunpin(संरचना buf*);

// console.c
शून्य            consoleinit(शून्य);
शून्य            consoleintr(पूर्णाङ्क);
शून्य            consputc(पूर्णाङ्क);

// exec.c
पूर्णाङ्क             kexec(वर्ण*, वर्ण**);

// file.c
संरचना file*    filealloc(शून्य);
शून्य            fileclose(संरचना file*);
संरचना file*    filedup(संरचना file*);
शून्य            fileinit(शून्य);
पूर्णाङ्क             fileread(संरचना file*, uint64, पूर्णाङ्क n);
पूर्णाङ्क             filestat(संरचना file*, uint64 addr);
पूर्णाङ्क             filewrite(संरचना file*, uint64, पूर्णाङ्क n);

// fs.c
शून्य            fsinit(पूर्णाङ्क);
पूर्णाङ्क             dirlink(संरचना inode*, वर्ण*, uint);
संरचना inode*   dirlookup(संरचना inode*, वर्ण*, uint*);
संरचना inode*   ialloc(uint, short);
संरचना inode*   idup(संरचना inode*);
शून्य            iinit();
शून्य            ilock(संरचना inode*);
शून्य            iput(संरचना inode*);
शून्य            iunlock(संरचना inode*);
शून्य            iunlockput(संरचना inode*);
शून्य            iupdate(संरचना inode*);
पूर्णाङ्क             namecmp(ध्रुव वर्ण*, ध्रुव वर्ण*);
संरचना inode*   namei(वर्ण*);
संरचना inode*   nameiparent(वर्ण*, वर्ण*);
पूर्णाङ्क             readi(संरचना inode*, पूर्णाङ्क, uint64, uint, uint);
शून्य            stati(संरचना inode*, संरचना stat*);
पूर्णाङ्क             writei(संरचना inode*, पूर्णाङ्क, uint64, uint, uint);
शून्य            itrunc(संरचना inode*);
शून्य            ireclaim(पूर्णाङ्क);

// kalloc.c
शून्य*           kalloc(शून्य);
शून्य            kfree(शून्य *);
शून्य            kinit(शून्य);

// log.c
शून्य            initlog(पूर्णाङ्क, संरचना superblock*);
शून्य            log_write(संरचना buf*);
शून्य            begin_op(शून्य);
शून्य            end_op(शून्य);

// pipe.c
पूर्णाङ्क             pipealloc(संरचना file**, संरचना file**);
शून्य            pipeclose(संरचना pipe*, पूर्णाङ्क);
पूर्णाङ्क             piperead(संरचना pipe*, uint64, पूर्णाङ्क);
पूर्णाङ्क             pipewrite(संरचना pipe*, uint64, पूर्णाङ्क);

// printk.c
पूर्णाङ्क             printk(वर्ण*, ...) __attribute__ ((format (printf, 1, 2)));
शून्य            panic(वर्ण*) __attribute__((noreturn));
शून्य            printkinit(शून्य);

// proc.c
पूर्णाङ्क             cpuid(शून्य);
शून्य            kexit(पूर्णाङ्क);
पूर्णाङ्क             kfork(शून्य);
पूर्णाङ्क             growproc(पूर्णाङ्क);
शून्य            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(संरचना proc *);
शून्य            proc_freepagetable(pagetable_t, uint64);
पूर्णाङ्क             kkill(पूर्णाङ्क);
पूर्णाङ्क             killed(संरचना proc*);
शून्य            setkilled(संरचना proc*);
संरचना cpu*     mycpu(शून्य);
संरचना proc*    myproc();
शून्य            procinit(शून्य);
शून्य            scheduler(शून्य) __attribute__((noreturn));
शून्य            sched(शून्य);
शून्य            sleep_prepare(शून्य*);
शून्य            sleep(शून्य);
शून्य            userinit(शून्य);
पूर्णाङ्क             kwait(uint64);
शून्य            wakeup(शून्य*);
शून्य            yield(शून्य);
पूर्णाङ्क             either_copyout(पूर्णाङ्क user_dst, uint64 dst, शून्य *src, uint64 len);
पूर्णाङ्क             either_copyin(शून्य *dst, पूर्णाङ्क user_src, uint64 src, uint64 len);
शून्य            procdump(शून्य);

// swtch.S
शून्य            swtch(संरचना context*, संरचना context*);

// spinlock.c
शून्य            acquire(संरचना spinlock*);
पूर्णाङ्क             holding(संरचना spinlock*);
शून्य            initlock(संरचना spinlock*, वर्ण*);
शून्य            release(संरचना spinlock*);
शून्य            प्रक्षिप_off(शून्य);
शून्य            निष्कासय_off(शून्य);

// sleeplock.c
शून्य            acquiresleep(संरचना sleeplock*);
शून्य            releasesleep(संरचना sleeplock*);
पूर्णाङ्क             holdingsleep(संरचना sleeplock*);
शून्य            initsleeplock(संरचना sleeplock*, वर्ण*);

// string.c
पूर्णाङ्क             memcmp(ध्रुव शून्य*, ध्रुव शून्य*, uint);
शून्य*           memmove(शून्य*, ध्रुव शून्य*, uint);
शून्य*           memset(शून्य*, पूर्णाङ्क, uint);
वर्ण*           safestrcpy(वर्ण*, ध्रुव वर्ण*, पूर्णाङ्क);
पूर्णाङ्क             strlen(ध्रुव वर्ण*);
पूर्णाङ्क             strncmp(ध्रुव वर्ण*, ध्रुव वर्ण*, uint);
वर्ण*           strncpy(वर्ण*, ध्रुव वर्ण*, पूर्णाङ्क);

// syscall.c
शून्य            argint(पूर्णाङ्क, पूर्णाङ्क*);
पूर्णाङ्क             argstr(पूर्णाङ्क, वर्ण*, पूर्णाङ्क);
शून्य            argaddr(पूर्णाङ्क, uint64 *);
पूर्णाङ्क             fetchstr(uint64, वर्ण*, पूर्णाङ्क);
पूर्णाङ्क             fetchaddr(uint64, uint64*);
शून्य            syscall();

// trap.c
बाह्य uint     ticks;
शून्य            trapinit(शून्य);
शून्य            trapinithart(शून्य);
बाह्य संरचना spinlock tickslock;
शून्य            prepare_return(शून्य);

// uart.c
शून्य            uartinit(शून्य);
शून्य            uartintr(शून्य);
शून्य            uartwrite(वर्ण [], पूर्णाङ्क);
शून्य            uartputc_sync(पूर्णाङ्क);

// vm.c
शून्य            kvminit(शून्य);
शून्य            kvminithart(शून्य);
शून्य            kvmmap(pagetable_t, uint64, uint64, uint64, पूर्णाङ्क);
पूर्णाङ्क             mappages(pagetable_t, uint64, uint64, uint64, पूर्णाङ्क);
pagetable_t     uvmcreate(शून्य);
uint64          uvmalloc(pagetable_t, uint64, uint64, पूर्णाङ्क);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
पूर्णाङ्क             uvmcopy(pagetable_t, pagetable_t, uint64);
शून्य            uvmfree(pagetable_t, uint64);
शून्य            uvmunmap(pagetable_t, uint64, uint64, पूर्णाङ्क);
शून्य            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, पूर्णाङ्क);
uint64          walkaddr(pagetable_t, uint64);
पूर्णाङ्क             copyout(pagetable_t, uint64, uint64, वर्ण *, uint64);
पूर्णाङ्क             copyin(pagetable_t, uint64, वर्ण *, uint64, uint64);
पूर्णाङ्क             copyinstr(pagetable_t, uint64, वर्ण *, uint64, uint64);
पूर्णाङ्क             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, पूर्णाङ्क);

// plic.c
शून्य            plicinit(शून्य);
शून्य            plicinithart(शून्य);
पूर्णाङ्क             plic_claim(शून्य);
शून्य            plic_complete(पूर्णाङ्क);

// virtio_disk.c
शून्य            virtio_disk_init(शून्य);
शून्य            virtio_disk_rw(संरचना buf *, पूर्णाङ्क);
शून्य            virtio_disk_intr(शून्य);

// number of elements in fixed-size array
#define NELEM(x) (परिमाण(x) / परिमाण((x)[0]))
