// clang-format off
கட்டமைப்பு buf;
கட்டமைப்பு context;
கட்டமைப்பு file;
கட்டமைப்பு inode;
கட்டமைப்பு pipe;
கட்டமைப்பு proc;
கட்டமைப்பு spinlock;
கட்டமைப்பு sleeplock;
கட்டமைப்பு stat;
கட்டமைப்பு superblock;

// bio.c
வெறுமை            binit(வெறுமை);
கட்டமைப்பு buf*     bread(uint, uint);
வெறுமை            brelse(கட்டமைப்பு buf*);
வெறுமை            bwrite(கட்டமைப்பு buf*);
வெறுமை            bpin(கட்டமைப்பு buf*);
வெறுமை            bunpin(கட்டமைப்பு buf*);

// console.c
வெறுமை            consoleinit(வெறுமை);
வெறுமை            consoleintr(முழு);
வெறுமை            consputc(முழு);

// exec.c
முழு             kexec(எழுத்து*, எழுத்து**);

// file.c
கட்டமைப்பு file*    filealloc(வெறுமை);
வெறுமை            fileclose(கட்டமைப்பு file*);
கட்டமைப்பு file*    filedup(கட்டமைப்பு file*);
வெறுமை            fileinit(வெறுமை);
முழு             fileread(கட்டமைப்பு file*, uint64, முழு n);
முழு             filestat(கட்டமைப்பு file*, uint64 addr);
முழு             filewrite(கட்டமைப்பு file*, uint64, முழு n);

// fs.c
வெறுமை            fsinit(முழு);
முழு             dirlink(கட்டமைப்பு inode*, எழுத்து*, uint);
கட்டமைப்பு inode*   dirlookup(கட்டமைப்பு inode*, எழுத்து*, uint*);
கட்டமைப்பு inode*   ialloc(uint, short);
கட்டமைப்பு inode*   idup(கட்டமைப்பு inode*);
வெறுமை            iinit();
வெறுமை            ilock(கட்டமைப்பு inode*);
வெறுமை            iput(கட்டமைப்பு inode*);
வெறுமை            iunlock(கட்டமைப்பு inode*);
வெறுமை            iunlockput(கட்டமைப்பு inode*);
வெறுமை            iupdate(கட்டமைப்பு inode*);
முழு             namecmp(மாறிலி எழுத்து*, மாறிலி எழுத்து*);
கட்டமைப்பு inode*   namei(எழுத்து*);
கட்டமைப்பு inode*   nameiparent(எழுத்து*, எழுத்து*);
முழு             readi(கட்டமைப்பு inode*, முழு, uint64, uint, uint);
வெறுமை            stati(கட்டமைப்பு inode*, கட்டமைப்பு stat*);
முழு             writei(கட்டமைப்பு inode*, முழு, uint64, uint, uint);
வெறுமை            itrunc(கட்டமைப்பு inode*);
வெறுமை            ireclaim(முழு);

// kalloc.c
வெறுமை*           kalloc(வெறுமை);
வெறுமை            kfree(வெறுமை *);
வெறுமை            kinit(வெறுமை);

// log.c
வெறுமை            initlog(முழு, கட்டமைப்பு superblock*);
வெறுமை            log_write(கட்டமைப்பு buf*);
வெறுமை            begin_op(வெறுமை);
வெறுமை            end_op(வெறுமை);

// pipe.c
முழு             pipealloc(கட்டமைப்பு file**, கட்டமைப்பு file**);
வெறுமை            pipeclose(கட்டமைப்பு pipe*, முழு);
முழு             piperead(கட்டமைப்பு pipe*, uint64, முழு);
முழு             pipewrite(கட்டமைப்பு pipe*, uint64, முழு);

// printk.c
முழு             printk(எழுத்து*, ...) __attribute__ ((format (printf, 1, 2)));
வெறுமை            panic(எழுத்து*) __attribute__((noreturn));
வெறுமை            printkinit(வெறுமை);

// proc.c
முழு             cpuid(வெறுமை);
வெறுமை            kexit(முழு);
முழு             kfork(வெறுமை);
முழு             growproc(முழு);
வெறுமை            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(கட்டமைப்பு proc *);
வெறுமை            proc_freepagetable(pagetable_t, uint64);
முழு             kkill(முழு);
முழு             killed(கட்டமைப்பு proc*);
வெறுமை            setkilled(கட்டமைப்பு proc*);
கட்டமைப்பு cpu*     mycpu(வெறுமை);
கட்டமைப்பு proc*    myproc();
வெறுமை            procinit(வெறுமை);
வெறுமை            scheduler(வெறுமை) __attribute__((noreturn));
வெறுமை            sched(வெறுமை);
வெறுமை            sleep_prepare(வெறுமை*);
வெறுமை            sleep(வெறுமை);
வெறுமை            userinit(வெறுமை);
முழு             kwait(uint64);
வெறுமை            wakeup(வெறுமை*);
வெறுமை            yield(வெறுமை);
முழு             either_copyout(முழு user_dst, uint64 dst, வெறுமை *src, uint64 len);
முழு             either_copyin(வெறுமை *dst, முழு user_src, uint64 src, uint64 len);
வெறுமை            procdump(வெறுமை);

// swtch.S
வெறுமை            swtch(கட்டமைப்பு context*, கட்டமைப்பு context*);

// spinlock.c
வெறுமை            acquire(கட்டமைப்பு spinlock*);
முழு             holding(கட்டமைப்பு spinlock*);
வெறுமை            initlock(கட்டமைப்பு spinlock*, எழுத்து*);
வெறுமை            release(கட்டமைப்பு spinlock*);
வெறுமை            தள்ளு_off(வெறுமை);
வெறுமை            எடு_off(வெறுமை);

// sleeplock.c
வெறுமை            acquiresleep(கட்டமைப்பு sleeplock*);
வெறுமை            releasesleep(கட்டமைப்பு sleeplock*);
முழு             holdingsleep(கட்டமைப்பு sleeplock*);
வெறுமை            initsleeplock(கட்டமைப்பு sleeplock*, எழுத்து*);

// string.c
முழு             memcmp(மாறிலி வெறுமை*, மாறிலி வெறுமை*, uint);
வெறுமை*           memmove(வெறுமை*, மாறிலி வெறுமை*, uint);
வெறுமை*           memset(வெறுமை*, முழு, uint);
எழுத்து*           safestrcpy(எழுத்து*, மாறிலி எழுத்து*, முழு);
முழு             strlen(மாறிலி எழுத்து*);
முழு             strncmp(மாறிலி எழுத்து*, மாறிலி எழுத்து*, uint);
எழுத்து*           strncpy(எழுத்து*, மாறிலி எழுத்து*, முழு);

// syscall.c
வெறுமை            argint(முழு, முழு*);
முழு             argstr(முழு, எழுத்து*, முழு);
வெறுமை            argaddr(முழு, uint64 *);
முழு             fetchstr(uint64, எழுத்து*, முழு);
முழு             fetchaddr(uint64, uint64*);
வெறுமை            syscall();

// trap.c
வெளி uint     ticks;
வெறுமை            trapinit(வெறுமை);
வெறுமை            trapinithart(வெறுமை);
வெளி கட்டமைப்பு spinlock tickslock;
வெறுமை            prepare_return(வெறுமை);

// uart.c
வெறுமை            uartinit(வெறுமை);
வெறுமை            uartintr(வெறுமை);
வெறுமை            uartwrite(எழுத்து [], முழு);
வெறுமை            uartputc_sync(முழு);

// vm.c
வெறுமை            kvminit(வெறுமை);
வெறுமை            kvminithart(வெறுமை);
வெறுமை            kvmmap(pagetable_t, uint64, uint64, uint64, முழு);
முழு             mappages(pagetable_t, uint64, uint64, uint64, முழு);
pagetable_t     uvmcreate(வெறுமை);
uint64          uvmalloc(pagetable_t, uint64, uint64, முழு);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
முழு             uvmcopy(pagetable_t, pagetable_t, uint64);
வெறுமை            uvmfree(pagetable_t, uint64);
வெறுமை            uvmunmap(pagetable_t, uint64, uint64, முழு);
வெறுமை            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, முழு);
uint64          walkaddr(pagetable_t, uint64);
முழு             copyout(pagetable_t, uint64, uint64, எழுத்து *, uint64);
முழு             copyin(pagetable_t, uint64, எழுத்து *, uint64, uint64);
முழு             copyinstr(pagetable_t, uint64, எழுத்து *, uint64, uint64);
முழு             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, முழு);

// plic.c
வெறுமை            plicinit(வெறுமை);
வெறுமை            plicinithart(வெறுமை);
முழு             plic_claim(வெறுமை);
வெறுமை            plic_complete(முழு);

// virtio_disk.c
வெறுமை            virtio_disk_init(வெறுமை);
வெறுமை            virtio_disk_rw(கட்டமைப்பு buf *, முழு);
வெறுமை            virtio_disk_intr(வெறுமை);

// number of elements in fixed-size array
#define NELEM(x) (அளவு(x) / அளவு((x)[0]))
