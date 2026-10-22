// clang-format off
સંરચના buf;
સંરચના context;
સંરચના file;
સંરચના inode;
સંરચના pipe;
સંરચના proc;
સંરચના spinlock;
સંરચના sleeplock;
સંરચના stat;
સંરચના superblock;

// bio.c
શૂન્ય            binit(શૂન્ય);
સંરચના buf*     bread(uint, uint);
શૂન્ય            brelse(સંરચના buf*);
શૂન્ય            bwrite(સંરચના buf*);
શૂન્ય            bpin(સંરચના buf*);
શૂન્ય            bunpin(સંરચના buf*);

// console.c
શૂન્ય            consoleinit(શૂન્ય);
શૂન્ય            consoleintr(પૂર્ણાંક);
શૂન્ય            consputc(પૂર્ણાંક);

// exec.c
પૂર્ણાંક             kexec(અક્ષર*, અક્ષર**);

// file.c
સંરચના file*    filealloc(શૂન્ય);
શૂન્ય            fileclose(સંરચના file*);
સંરચના file*    filedup(સંરચના file*);
શૂન્ય            fileinit(શૂન્ય);
પૂર્ણાંક             fileread(સંરચના file*, uint64, પૂર્ણાંક n);
પૂર્ણાંક             filestat(સંરચના file*, uint64 addr);
પૂર્ણાંક             filewrite(સંરચના file*, uint64, પૂર્ણાંક n);

// fs.c
શૂન્ય            fsinit(પૂર્ણાંક);
પૂર્ણાંક             dirlink(સંરચના inode*, અક્ષર*, uint);
સંરચના inode*   dirlookup(સંરચના inode*, અક્ષર*, uint*);
સંરચના inode*   ialloc(uint, short);
સંરચના inode*   idup(સંરચના inode*);
શૂન્ય            iinit();
શૂન્ય            ilock(સંરચના inode*);
શૂન્ય            iput(સંરચના inode*);
શૂન્ય            iunlock(સંરચના inode*);
શૂન્ય            iunlockput(સંરચના inode*);
શૂન્ય            iupdate(સંરચના inode*);
પૂર્ણાંક             namecmp(અચલ અક્ષર*, અચલ અક્ષર*);
સંરચના inode*   namei(અક્ષર*);
સંરચના inode*   nameiparent(અક્ષર*, અક્ષર*);
પૂર્ણાંક             readi(સંરચના inode*, પૂર્ણાંક, uint64, uint, uint);
શૂન્ય            stati(સંરચના inode*, સંરચના stat*);
પૂર્ણાંક             writei(સંરચના inode*, પૂર્ણાંક, uint64, uint, uint);
શૂન્ય            itrunc(સંરચના inode*);
શૂન્ય            ireclaim(પૂર્ણાંક);

// kalloc.c
શૂન્ય*           kalloc(શૂન્ય);
શૂન્ય            kfree(શૂન્ય *);
શૂન્ય            kinit(શૂન્ય);

// log.c
શૂન્ય            initlog(પૂર્ણાંક, સંરચના superblock*);
શૂન્ય            log_write(સંરચના buf*);
શૂન્ય            begin_op(શૂન્ય);
શૂન્ય            end_op(શૂન્ય);

// pipe.c
પૂર્ણાંક             pipealloc(સંરચના file**, સંરચના file**);
શૂન્ય            pipeclose(સંરચના pipe*, પૂર્ણાંક);
પૂર્ણાંક             piperead(સંરચના pipe*, uint64, પૂર્ણાંક);
પૂર્ણાંક             pipewrite(સંરચના pipe*, uint64, પૂર્ણાંક);

// printk.c
પૂર્ણાંક             printk(અક્ષર*, ...) __attribute__ ((format (printf, 1, 2)));
શૂન્ય            panic(અક્ષર*) __attribute__((noreturn));
શૂન્ય            printkinit(શૂન્ય);

// proc.c
પૂર્ણાંક             cpuid(શૂન્ય);
શૂન્ય            kexit(પૂર્ણાંક);
પૂર્ણાંક             kfork(શૂન્ય);
પૂર્ણાંક             growproc(પૂર્ણાંક);
શૂન્ય            proc_mapstacks(pagetable_t);
pagetable_t     proc_pagetable(સંરચના proc *);
શૂન્ય            proc_freepagetable(pagetable_t, uint64);
પૂર્ણાંક             kkill(પૂર્ણાંક);
પૂર્ણાંક             killed(સંરચના proc*);
શૂન્ય            setkilled(સંરચના proc*);
સંરચના cpu*     mycpu(શૂન્ય);
સંરચના proc*    myproc();
શૂન્ય            procinit(શૂન્ય);
શૂન્ય            scheduler(શૂન્ય) __attribute__((noreturn));
શૂન્ય            sched(શૂન્ય);
શૂન્ય            sleep_prepare(શૂન્ય*);
શૂન્ય            sleep(શૂન્ય);
શૂન્ય            userinit(શૂન્ય);
પૂર્ણાંક             kwait(uint64);
શૂન્ય            wakeup(શૂન્ય*);
શૂન્ય            yield(શૂન્ય);
પૂર્ણાંક             either_copyout(પૂર્ણાંક user_dst, uint64 dst, શૂન્ય *src, uint64 len);
પૂર્ણાંક             either_copyin(શૂન્ય *dst, પૂર્ણાંક user_src, uint64 src, uint64 len);
શૂન્ય            procdump(શૂન્ય);

// swtch.S
શૂન્ય            swtch(સંરચના context*, સંરચના context*);

// spinlock.c
શૂન્ય            acquire(સંરચના spinlock*);
પૂર્ણાંક             holding(સંરચના spinlock*);
શૂન્ય            initlock(સંરચના spinlock*, અક્ષર*);
શૂન્ય            release(સંરચના spinlock*);
શૂન્ય            ધકેલો_off(શૂન્ય);
શૂન્ય            કાઢો_off(શૂન્ય);

// sleeplock.c
શૂન્ય            acquiresleep(સંરચના sleeplock*);
શૂન્ય            releasesleep(સંરચના sleeplock*);
પૂર્ણાંક             holdingsleep(સંરચના sleeplock*);
શૂન્ય            initsleeplock(સંરચના sleeplock*, અક્ષર*);

// string.c
પૂર્ણાંક             memcmp(અચલ શૂન્ય*, અચલ શૂન્ય*, uint);
શૂન્ય*           memmove(શૂન્ય*, અચલ શૂન્ય*, uint);
શૂન્ય*           memset(શૂન્ય*, પૂર્ણાંક, uint);
અક્ષર*           safestrcpy(અક્ષર*, અચલ અક્ષર*, પૂર્ણાંક);
પૂર્ણાંક             strlen(અચલ અક્ષર*);
પૂર્ણાંક             strncmp(અચલ અક્ષર*, અચલ અક્ષર*, uint);
અક્ષર*           strncpy(અક્ષર*, અચલ અક્ષર*, પૂર્ણાંક);

// syscall.c
શૂન્ય            argint(પૂર્ણાંક, પૂર્ણાંક*);
પૂર્ણાંક             argstr(પૂર્ણાંક, અક્ષર*, પૂર્ણાંક);
શૂન્ય            argaddr(પૂર્ણાંક, uint64 *);
પૂર્ણાંક             fetchstr(uint64, અક્ષર*, પૂર્ણાંક);
પૂર્ણાંક             fetchaddr(uint64, uint64*);
શૂન્ય            syscall();

// trap.c
બાહ્ય uint     ticks;
શૂન્ય            trapinit(શૂન્ય);
શૂન્ય            trapinithart(શૂન્ય);
બાહ્ય સંરચના spinlock tickslock;
શૂન્ય            prepare_return(શૂન્ય);

// uart.c
શૂન્ય            uartinit(શૂન્ય);
શૂન્ય            uartintr(શૂન્ય);
શૂન્ય            uartwrite(અક્ષર [], પૂર્ણાંક);
શૂન્ય            uartputc_sync(પૂર્ણાંક);

// vm.c
શૂન્ય            kvminit(શૂન્ય);
શૂન્ય            kvminithart(શૂન્ય);
શૂન્ય            kvmmap(pagetable_t, uint64, uint64, uint64, પૂર્ણાંક);
પૂર્ણાંક             mappages(pagetable_t, uint64, uint64, uint64, પૂર્ણાંક);
pagetable_t     uvmcreate(શૂન્ય);
uint64          uvmalloc(pagetable_t, uint64, uint64, પૂર્ણાંક);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
પૂર્ણાંક             uvmcopy(pagetable_t, pagetable_t, uint64);
શૂન્ય            uvmfree(pagetable_t, uint64);
શૂન્ય            uvmunmap(pagetable_t, uint64, uint64, પૂર્ણાંક);
શૂન્ય            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, પૂર્ણાંક);
uint64          walkaddr(pagetable_t, uint64);
પૂર્ણાંક             copyout(pagetable_t, uint64, uint64, અક્ષર *, uint64);
પૂર્ણાંક             copyin(pagetable_t, uint64, અક્ષર *, uint64, uint64);
પૂર્ણાંક             copyinstr(pagetable_t, uint64, અક્ષર *, uint64, uint64);
પૂર્ણાંક             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, uint64, પૂર્ણાંક);

// plic.c
શૂન્ય            plicinit(શૂન્ય);
શૂન્ય            plicinithart(શૂન્ય);
પૂર્ણાંક             plic_claim(શૂન્ય);
શૂન્ય            plic_complete(પૂર્ણાંક);

// virtio_disk.c
શૂન્ય            virtio_disk_init(શૂન્ય);
શૂન્ય            virtio_disk_rw(સંરચના buf *, પૂર્ણાંક);
શૂન્ય            virtio_disk_intr(શૂન્ય);

// number of elements in fixed-size array
#define NELEM(x) (કદ(x) / કદ((x)[0]))
