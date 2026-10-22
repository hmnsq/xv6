// clang-format off
ഘടന buf;
ഘടന context;
ഘടന file;
ഘടന inode;
ഘടന pipe;
ഘടന proc;
ഘടന spinlock;
ഘടന sleeplock;
ഘടന stat;
ഘടന superblock;

// bio.c
ശൂന്യം            binit(ശൂന്യം);
ഘടന buf*     bread(uint, uint);
ശൂന്യം            brelse(ഘടന buf*);
ശൂന്യം            bwrite(ഘടന buf*);
ശൂന്യം            bpin(ഘടന buf*);
ശൂന്യം            bunpin(ഘടന buf*);

// console.c
ശൂന്യം            consoleinit(ശൂന്യം);
ശൂന്യം            consoleintr(പൂർണ്ണസംഖ്യ);
ശൂന്യം            consputc(പൂർണ്ണസംഖ്യ);

// exec.c
പൂർണ്ണസംഖ്യ             kexec(അക്ഷരം*, അക്ഷരം**);

// file.c
ഘടന file*    filealloc(ശൂന്യം);
ശൂന്യം            fileclose(ഘടന file*);
ഘടന file*    filedup(ഘടന file*);
ശൂന്യം            fileinit(ശൂന്യം);
പൂർണ്ണസംഖ്യ             fileread(ഘടന file*, uint64, പൂർണ്ണസംഖ്യ n);
പൂർണ്ണസംഖ്യ             filestat(ഘടന file*, uint64 addr);
പൂർണ്ണസംഖ്യ             filewrite(ഘടന file*, uint64, പൂർണ്ണസംഖ്യ n);

// fs.c
ശൂന്യം            fsinit(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             dirlink(ഘടന inode*, അക്ഷരം*, uint);
ഘടന inode*   dirlookup(ഘടന inode*, അക്ഷരം*, uint*);
ഘടന inode*   ialloc(uint, short);
ഘടന inode*   idup(ഘടന inode*);
ശൂന്യം            iinit();
ശൂന്യം            ilock(ഘടന inode*);
ശൂന്യം            iput(ഘടന inode*);
ശൂന്യം            iunlock(ഘടന inode*);
ശൂന്യം            iunlockput(ഘടന inode*);
ശൂന്യം            iupdate(ഘടന inode*);
പൂർണ്ണസംഖ്യ             namecmp(സ്ഥിരം അക്ഷരം*, സ്ഥിരം അക്ഷരം*);
ഘടന inode*   namei(അക്ഷരം*);
ഘടന inode*   nameiparent(അക്ഷരം*, അക്ഷരം*);
പൂർണ്ണസംഖ്യ             readi(ഘടന inode*, പൂർണ്ണസംഖ്യ, uint64, uint, uint);
ശൂന്യം            stati(ഘടന inode*, ഘടന stat*);
പൂർണ്ണസംഖ്യ             writei(ഘടന inode*, പൂർണ്ണസംഖ്യ, uint64, uint, uint);
ശൂന്യം            itrunc(ഘടന inode*);
ശൂന്യം            ireclaim(പൂർണ്ണസംഖ്യ);

// kalloc.c
ശൂന്യം*           kalloc(ശൂന്യം);
ശൂന്യം            kfree(ശൂന്യം *);
ശൂന്യം            kinit(ശൂന്യം);

// log.c
ശൂന്യം            initlog(പൂർണ്ണസംഖ്യ, ഘടന superblock*);
ശൂന്യം            log_write(ഘടന buf*);
ശൂന്യം            begin_op(ശൂന്യം);
ശൂന്യം            end_op(ശൂന്യം);

// pipe.c
പൂർണ്ണസംഖ്യ             pipealloc(ഘടന file**, ഘടന file**);
ശൂന്യം            pipeclose(ഘടന pipe*, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             piperead(ഘടന pipe*, uint64, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             pipewrite(ഘടന pipe*, uint64, പൂർണ്ണസംഖ്യ);

// printk.c
പൂർണ്ണസംഖ്യ             printk(അക്ഷരം*, ...) __attribute__ ((format (printf, 1, 2)));
ശൂന്യം            panic(അക്ഷരം*) __attribute__((noreturn));
ശൂന്യം            printkinit(ശൂന്യം);

// proc.c
പൂർണ്ണസംഖ്യ             cpuid(ശൂന്യം);
ശൂന്യം            kexit(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             kfork(ശൂന്യം);
പൂർണ്ണസംഖ്യ             growproc(പൂർണ്ണസംഖ്യ);
ശൂന്യം            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(ഘടന proc *);
ശൂന്യം            proc_freepagetable(pagetable_t, uint64);
പൂർണ്ണസംഖ്യ             kkill(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             killed(ഘടന proc*);
ശൂന്യം            setkilled(ഘടന proc*);
ഘടന cpu*     mycpu(ശൂന്യം);
ഘടന proc*    myproc();
ശൂന്യം            procinit(ശൂന്യം);
ശൂന്യം            scheduler(ശൂന്യം) __attribute__((noreturn));
ശൂന്യം            sched(ശൂന്യം);
ശൂന്യം            sleep_prepare(ശൂന്യം*);
ശൂന്യം            sleep(ശൂന്യം);
ശൂന്യം            userinit(ശൂന്യം);
പൂർണ്ണസംഖ്യ             kwait(uint64);
ശൂന്യം            wakeup(ശൂന്യം*);
ശൂന്യം            yield(ശൂന്യം);
പൂർണ്ണസംഖ്യ             either_copyout(പൂർണ്ണസംഖ്യ user_dst, uint64 dst, ശൂന്യം *src, uint64 len);
പൂർണ്ണസംഖ്യ             either_copyin(ശൂന്യം *dst, പൂർണ്ണസംഖ്യ user_src, uint64 src, uint64 len);
ശൂന്യം            procdump(ശൂന്യം);

// swtch.S
ശൂന്യം            swtch(ഘടന context*, ഘടന context*);

// spinlock.c
ശൂന്യം            acquire(ഘടന spinlock*);
പൂർണ്ണസംഖ്യ             holding(ഘടന spinlock*);
ശൂന്യം            initlock(ഘടന spinlock*, അക്ഷരം*);
ശൂന്യം            release(ഘടന spinlock*);
ശൂന്യം            തള്ളുക_off(ശൂന്യം);
ശൂന്യം            എടുക്കുക_off(ശൂന്യം);

// sleeplock.c
ശൂന്യം            acquiresleep(ഘടന sleeplock*);
ശൂന്യം            releasesleep(ഘടന sleeplock*);
പൂർണ്ണസംഖ്യ             holdingsleep(ഘടന sleeplock*);
ശൂന്യം            initsleeplock(ഘടന sleeplock*, അക്ഷരം*);

// string.c
പൂർണ്ണസംഖ്യ             memcmp(സ്ഥിരം ശൂന്യം*, സ്ഥിരം ശൂന്യം*, uint);
ശൂന്യം*           memmove(ശൂന്യം*, സ്ഥിരം ശൂന്യം*, uint);
ശൂന്യം*           memset(ശൂന്യം*, പൂർണ്ണസംഖ്യ, uint);
അക്ഷരം*           safestrcpy(അക്ഷരം*, സ്ഥിരം അക്ഷരം*, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             strlen(സ്ഥിരം അക്ഷരം*);
പൂർണ്ണസംഖ്യ             strncmp(സ്ഥിരം അക്ഷരം*, സ്ഥിരം അക്ഷരം*, uint);
അക്ഷരം*           strncpy(അക്ഷരം*, സ്ഥിരം അക്ഷരം*, പൂർണ്ണസംഖ്യ);

// syscall.c
ശൂന്യം            argint(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ*);
പൂർണ്ണസംഖ്യ             argstr(പൂർണ്ണസംഖ്യ, അക്ഷരം*, പൂർണ്ണസംഖ്യ);
ശൂന്യം            argaddr(പൂർണ്ണസംഖ്യ, uint64 *);
പൂർണ്ണസംഖ്യ             fetchstr(uint64, അക്ഷരം*, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             fetchaddr(uint64, uint64*);
ശൂന്യം            syscall();

// trap.c
ബാഹ്യ uint     ticks;
ശൂന്യം            trapinit(ശൂന്യം);
ശൂന്യം            trapinithart(ശൂന്യം);
ബാഹ്യ ഘടന spinlock tickslock;
ശൂന്യം            prepare_return(ശൂന്യം);

// uart.c
ശൂന്യം            uartinit(ശൂന്യം);
ശൂന്യം            uartintr(ശൂന്യം);
ശൂന്യം            uartwrite(അക്ഷരം [], പൂർണ്ണസംഖ്യ);
ശൂന്യം            uartputc_sync(പൂർണ്ണസംഖ്യ);

// vm.c
ശൂന്യം            kvminit(ശൂന്യം);
ശൂന്യം            kvminithart(ശൂന്യം);
ശൂന്യം            kvmmap(pagetable_t, uint64, uint64, uint64, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ             mappages(pagetable_t, uint64, uint64, uint64, പൂർണ്ണസംഖ്യ);
pagetable_t     uvmcreate(ശൂന്യം);
uint64          uvmalloc(pagetable_t, uint64, uint64, പൂർണ്ണസംഖ്യ);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
പൂർണ്ണസംഖ്യ             uvmcopy(pagetable_t, pagetable_t, uint64);
ശൂന്യം            uvmfree(pagetable_t, uint64);
ശൂന്യം            uvmunmap(pagetable_t, uint64, uint64, പൂർണ്ണസംഖ്യ);
ശൂന്യം            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, പൂർണ്ണസംഖ്യ);
uint64          walkaddr(pagetable_t, uint64);
പൂർണ്ണസംഖ്യ             copyout(pagetable_t, uint64, uint64, അക്ഷരം *, uint64);
പൂർണ്ണസംഖ്യ             copyin(pagetable_t, uint64, അക്ഷരം *, uint64, uint64);
പൂർണ്ണസംഖ്യ             copyinstr(pagetable_t, uint64, അക്ഷരം *, uint64, uint64);
പൂർണ്ണസംഖ്യ             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, പൂർണ്ണസംഖ്യ);

// plic.c
ശൂന്യം            plicinit(ശൂന്യം);
ശൂന്യം            plicinithart(ശൂന്യം);
പൂർണ്ണസംഖ്യ             plic_claim(ശൂന്യം);
ശൂന്യം            plic_complete(പൂർണ്ണസംഖ്യ);

// virtio_disk.c
ശൂന്യം            virtio_disk_init(ശൂന്യം);
ശൂന്യം            virtio_disk_rw(ഘടന buf *, പൂർണ്ണസംഖ്യ);
ശൂന്യം            virtio_disk_intr(ശൂന്യം);

// number of elements in fixed-size array
#define NELEM(x) (വലുപ്പം(x) / വലുപ്പം((x)[0]))
