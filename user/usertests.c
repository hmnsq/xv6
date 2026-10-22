#അന്തര്ഭാവ "kernel/param.h"
#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "kernel/syscall.h"
#അന്തര്ഭാവ "kernel/memlayout.h"
#അന്തര്ഭാവ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

അക്ഷരം buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
ശൂന്യം
copyin(അക്ഷരം *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  വേണ്ടി (പൂർണ്ണസംഖ്യ ai = 0; ai < വലുപ്പം(addrs) / വലുപ്പം(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    പൂർണ്ണസംഖ്യ fd = open("copyin1", O_CREATE | O_WRONLY);
    എങ്കിൽ (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    പൂർണ്ണസംഖ്യ n = ലിഖ(fd, (ശൂന്യം *)addr, 8192);
    എങ്കിൽ (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (ശൂന്യം *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = ലിഖ(1, (അക്ഷരം *)addr, 8192);
    എങ്കിൽ (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (ശൂന്യം *)addr, n);
      exit(1);
    }

    പൂർണ്ണസംഖ്യ fds[2];
    എങ്കിൽ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ലിഖ(fds[1], (അക്ഷരം *)addr, 8192);
    എങ്കിൽ (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (ശൂന്യം *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
ശൂന്യം
copyout(അക്ഷരം *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  വേണ്ടി (പൂർണ്ണസംഖ്യ ai = 0; ai < വലുപ്പം(addrs) / വലുപ്പം(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    പൂർണ്ണസംഖ്യ fd = open("README", 0);
    എങ്കിൽ (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    പൂർണ്ണസംഖ്യ n = പഠ(fd, (ശൂന്യം *)addr, 8192);
    എങ്കിൽ (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (ശൂന്യം *)addr, n);
      exit(1);
    }
    close(fd);

    പൂർണ്ണസംഖ്യ fds[2];
    എങ്കിൽ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ലിഖ(fds[1], "x", 1);
    എങ്കിൽ (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = പഠ(fds[0], (ശൂന്യം *)addr, 8192);
    എങ്കിൽ (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (ശൂന്യം *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
ശൂന്യം
copyinstr1(അക്ഷരം *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  വേണ്ടി (പൂർണ്ണസംഖ്യ ai = 0; ai < വലുപ്പം(addrs) / വലുപ്പം(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    പൂർണ്ണസംഖ്യ fd = open((അക്ഷരം *)addr, O_CREATE | O_WRONLY);
    എങ്കിൽ (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (ശൂന്യം *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
ശൂന്യം
copyinstr2(അക്ഷരം *s)
{
  അക്ഷരം b[MAXPATH + 1];

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  പൂർണ്ണസംഖ്യ ret = unlink(b);
  എങ്കിൽ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  പൂർണ്ണസംഖ്യ fd = open(b, O_CREATE | O_WRONLY);
  എങ്കിൽ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  എങ്കിൽ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  അക്ഷരം *args[] = {"xx", 0};
  ret = exec(b, args);
  എങ്കിൽ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  പൂർണ്ണസംഖ്യ pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    സ്ഥിതം അക്ഷരം big[PGSIZE + 1];
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    അക്ഷരം *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    എങ്കിൽ (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  പൂർണ്ണസംഖ്യ st = 0;
  wait(&st);
  എങ്കിൽ (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
ശൂന്യം
copyinstr3(അക്ഷരം *s)
{
  sbrk(8192);
  uint64 മുകൾ = (uint64)sbrk(0);
  എങ്കിൽ ((മുകൾ % PGSIZE) != 0) {
    sbrk(PGSIZE - (മുകൾ % PGSIZE));
  }
  മുകൾ = (uint64)sbrk(0);
  എങ്കിൽ (മുകൾ % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  അക്ഷരം *b = (അക്ഷരം *)(മുകൾ - 1);
  *b = 'x';

  പൂർണ്ണസംഖ്യ ret = unlink(b);
  എങ്കിൽ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  പൂർണ്ണസംഖ്യ fd = open(b, O_CREATE | O_WRONLY);
  എങ്കിൽ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  എങ്കിൽ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  അക്ഷരം *args[] = {"xx", 0};
  ret = exec(b, args);
  എങ്കിൽ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
ശൂന്യം
rwsbrk(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, n;

  uint64 a = (uint64)sbrk(8192);

  എങ്കിൽ (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  എങ്കിൽ (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  എങ്കിൽ (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = ലിഖ(fd, (ശൂന്യം *)(a + PGSIZE), 1024);
  എങ്കിൽ (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (ശൂന്യം *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = പഠ(fd, (ശൂന്യം *)(a + PGSIZE), 10);
  എങ്കിൽ (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (ശൂന്യം *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
ശൂന്യം
truncate1(അക്ഷരം *s)
{
  അക്ഷരം buf[32];

  unlink("truncfile");
  പൂർണ്ണസംഖ്യ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  ലിഖ(fd1, "abcd", 4);
  close(fd1);

  പൂർണ്ണസംഖ്യ fd2 = open("truncfile", O_RDONLY);
  പൂർണ്ണസംഖ്യ n = പഠ(fd2, buf, വലുപ്പം(buf));
  എങ്കിൽ (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  പൂർണ്ണസംഖ്യ fd3 = open("truncfile", O_RDONLY);
  n = പഠ(fd3, buf, വലുപ്പം(buf));
  എങ്കിൽ (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = പഠ(fd2, buf, വലുപ്പം(buf));
  എങ്കിൽ (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  ലിഖ(fd1, "abcdef", 6);

  n = പഠ(fd3, buf, വലുപ്പം(buf));
  എങ്കിൽ (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = പഠ(fd2, buf, വലുപ്പം(buf));
  എങ്കിൽ (n != 2) {
    printf("%s: read %d bytes, wanted 2\n", s, n);
    exit(1);
  }

  unlink("truncfile");

  close(fd1);
  close(fd2);
  close(fd3);
}

// write to an open FD whose file has just been truncated.
// this causes a write at an offset beyond the end of the file.
// such writes fail on xv6 (unlike POSIX) but at least
// they don't crash.
ശൂന്യം
truncate2(അക്ഷരം *s)
{
  unlink("truncfile");

  പൂർണ്ണസംഖ്യ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  ലിഖ(fd1, "abcd", 4);

  പൂർണ്ണസംഖ്യ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  പൂർണ്ണസംഖ്യ n = ലിഖ(fd1, "x", 1);
  എങ്കിൽ (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

ശൂന്യം
truncate3(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  എങ്കിൽ (pid == 0) {
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 100; i++) {
      അക്ഷരം buf[32];
      പൂർണ്ണസംഖ്യ fd = open("truncfile", O_WRONLY);
      എങ്കിൽ (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      പൂർണ്ണസംഖ്യ n = ലിഖ(fd, "1234567890", 10);
      എങ്കിൽ (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      പഠ(fd, buf, വലുപ്പം(buf));
      close(fd);
    }
    exit(0);
  }

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 150; i++) {
    പൂർണ്ണസംഖ്യ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    എങ്കിൽ (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    പൂർണ്ണസംഖ്യ n = ലിഖ(fd, "xxx", 3);
    എങ്കിൽ (n != 3) {
      printf("%s: write got %d, expected 3\n", s, n);
      exit(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  exit(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
ശൂന്യം
iputtest(അക്ഷരം *s)
{
  എങ്കിൽ (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
ശൂന്യം
exitiputtest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, xstatus;

  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    എങ്കിൽ (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    എങ്കിൽ (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    എങ്കിൽ (unlink("../iputdir") < 0) {
      printf("%s: unlink ../iputdir failed\n", s);
      exit(1);
    }
    exit(0);
  }
  wait(&xstatus);
  exit(xstatus);
}

// does the error path in open() for attempt to write a
// directory call iput() in a transaction?
// needs a hacked kernel that pauses just after the namei()
// call in sys_open():
//    if((ip = namei(path)) == 0)
//      return -1;
//    {
//      int i;
//      for(i = 0; i < 10000; i++)
//        yield();
//    }
ശൂന്യം
openiputtest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, xstatus;

  എങ്കിൽ (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    പൂർണ്ണസംഖ്യ fd = open("oidir", O_RDWR);
    എങ്കിൽ (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  എങ്കിൽ (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

ശൂന്യം
opentest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd;

  fd = open("echo", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  എങ്കിൽ (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

ശൂന്യം
writetest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd;
  പൂർണ്ണസംഖ്യ i;
  എണ്ണൽ { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  വേണ്ടി (i = 0; i < N; i++) {
    എങ്കിൽ (ലിഖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    എങ്കിൽ (ലിഖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = പഠ(fd, buf, N * SZ * 2);
  എങ്കിൽ (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

ശൂന്യം
writebig(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  വേണ്ടി (i = 0; i < MAXFILE; i++) {
    ((പൂർണ്ണസംഖ്യ *)buf)[0] = i;
    എങ്കിൽ (ലിഖ(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  വേണ്ടി (;;) {
    i = പഠ(fd, buf, BSIZE);
    എങ്കിൽ (i == 0) {
      എങ്കിൽ (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      നിർത്തുക;
    } അല്ലെങ്കിൽ എങ്കിൽ (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    എങ്കിൽ (((പൂർണ്ണസംഖ്യ *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((പൂർണ്ണസംഖ്യ *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  എങ്കിൽ (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
ശൂന്യം
createtest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ i, fd;
  എണ്ണൽ { N = 52 };

  അക്ഷരം name[3];
  name[0] = 'a';
  name[2] = '\0';
  വേണ്ടി (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  വേണ്ടി (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

ശൂന്യം
dirtest(അക്ഷരം *s)
{
  എങ്കിൽ (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  എങ്കിൽ (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  എങ്കിൽ (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  എങ്കിൽ (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

ശൂന്യം
exectest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, xstatus, pid;
  അക്ഷരം *echoargv[] = {"echo", "OK", 0};
  അക്ഷരം buf[3];

  unlink("echo-ok");
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    പൂർണ്ണസംഖ്യ errfd = dup(1);
    എങ്കിൽ (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    എങ്കിൽ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    എങ്കിൽ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    എങ്കിൽ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  എങ്കിൽ (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  എങ്കിൽ (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  എങ്കിൽ (പഠ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  എങ്കിൽ (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  അല്ലെങ്കിൽ {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

ശൂന്യം
pipe1(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fds[2], pid, xstatus;
  പൂർണ്ണസംഖ്യ seq, i, n, cc, total;
  എണ്ണൽ { N = 5, SZ = 1033 };

  എങ്കിൽ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  എങ്കിൽ (pid == 0) {
    close(fds[0]);
    വേണ്ടി (n = 0; n < N; n++) {
      വേണ്ടി (i = 0; i < SZ; i++)
        buf[i] = seq++;
      എങ്കിൽ (ലിഖ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } അല്ലെങ്കിൽ എങ്കിൽ (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    ആകുമ്പോൾ ((n = പഠ(fds[0], buf, cc)) > 0) {
      വേണ്ടി (i = 0; i < n; i++) {
        എങ്കിൽ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          തിരികെ;
        }
      }
      total += n;
      cc = cc * 2;
      എങ്കിൽ (cc > വലുപ്പം(buf))
        cc = വലുപ്പം(buf);
    }
    എങ്കിൽ (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } അല്ലെങ്കിൽ {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
ശൂന്യം
killstatus(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ xst;

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 100; i++) {
    പൂർണ്ണസംഖ്യ pid1 = fork();
    എങ്കിൽ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid1 == 0) {
      ആകുമ്പോൾ (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    എങ്കിൽ (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
ശൂന്യം
killzero(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, xst;

  kill(0);

  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    exit(7);
  }
  എങ്കിൽ (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  എങ്കിൽ (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
ശൂന്യം
preempt(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid1, pid2, pid3;
  പൂർണ്ണസംഖ്യ pfds[2];

  pid1 = fork();
  എങ്കിൽ (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  എങ്കിൽ (pid1 == 0)
    വേണ്ടി (;;)
      ;

  pid2 = fork();
  എങ്കിൽ (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid2 == 0)
    വേണ്ടി (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  എങ്കിൽ (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid3 == 0) {
    close(pfds[0]);
    എങ്കിൽ (ലിഖ(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    വേണ്ടി (;;)
      ;
  }

  close(pfds[1]);
  എങ്കിൽ (പഠ(pfds[0], buf, വലുപ്പം(buf)) != 1) {
    printf("%s: preempt read error", s);
    തിരികെ;
  }
  close(pfds[0]);
  printf("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  printf("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
ശൂന്യം
exitwait(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ i, pid;

  വേണ്ടി (i = 0; i < 100; i++) {
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid) {
      പൂർണ്ണസംഖ്യ xstate;
      എങ്കിൽ (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      എങ്കിൽ (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } അല്ലെങ്കിൽ {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
ശൂന്യം
reparent(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ master_pid = getpid();
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 200; i++) {
    പൂർണ്ണസംഖ്യ pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid) {
      എങ്കിൽ (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } അല്ലെങ്കിൽ {
      പൂർണ്ണസംഖ്യ pid2 = fork();
      എങ്കിൽ (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
ശൂന്യം
twochildren(അക്ഷരം *s)
{
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 1000; i++) {
    പൂർണ്ണസംഖ്യ pid1 = fork();
    എങ്കിൽ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid1 == 0) {
      exit(0);
    } അല്ലെങ്കിൽ {
      പൂർണ്ണസംഖ്യ pid2 = fork();
      എങ്കിൽ (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      എങ്കിൽ (pid2 == 0) {
        exit(0);
      } അല്ലെങ്കിൽ {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
ശൂന്യം
forkfork(അക്ഷരം *s)
{
  എണ്ണൽ { N = 2 };

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < N; i++) {
    പൂർണ്ണസംഖ്യ pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    എങ്കിൽ (pid == 0) {
      വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < 200; j++) {
        പൂർണ്ണസംഖ്യ pid1 = fork();
        എങ്കിൽ (pid1 < 0) {
          exit(1);
        }
        എങ്കിൽ (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  പൂർണ്ണസംഖ്യ xstatus;
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < N; i++) {
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

ശൂന്യം
forkforkfork(അക്ഷരം *s)
{
  unlink("stopforking");

  പൂർണ്ണസംഖ്യ pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    ആകുമ്പോൾ (1) {
      പൂർണ്ണസംഖ്യ fd = open("stopforking", 0);
      എങ്കിൽ (fd >= 0) {
        exit(0);
      }
      എങ്കിൽ (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    exit(0);
  }

  pause(20); // two seconds
  close(open("stopforking", O_CREATE | O_RDWR));
  wait(0);
  pause(10); // one second
}

// regression test. does reparent() violate the parent-then-child
// locking order when giving away a child to init, so that exit()
// deadlocks against init's wait()? also used to trigger a "panic:
// release" due to exit() releasing a different p->parent->lock than
// it acquired.
ശൂന്യം
reparent2(അക്ഷരം *s)
{
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 800; i++) {
    പൂർണ്ണസംഖ്യ pid1 = fork();
    എങ്കിൽ (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    എങ്കിൽ (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
ശൂന്യം
mem(അക്ഷരം *s)
{
  ശൂന്യം *m1, *m2;
  പൂർണ്ണസംഖ്യ pid;

  എങ്കിൽ ((pid = fork()) == 0) {
    m1 = 0;
    ആകുമ്പോൾ ((m2 = malloc(10001)) != 0) {
      *(അക്ഷരം **)m2 = m1;
      m1 = m2;
    }
    ആകുമ്പോൾ (m1) {
      m2 = *(അക്ഷരം **)m1;
      സ്വതന്ത്രം(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    എങ്കിൽ (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    സ്വതന്ത്രം(m1);
    exit(0);
  } അല്ലെങ്കിൽ {
    പൂർണ്ണസംഖ്യ xstatus;
    wait(&xstatus);
    എങ്കിൽ (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      exit(0);
    }
    exit(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
ശൂന്യം
sharedfd(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, pid, i, n, nc, np;
  എണ്ണൽ { N = 1000, SZ = 10 };
  അക്ഷരം buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', വലുപ്പം(buf));
  വേണ്ടി (i = 0; i < N; i++) {
    എങ്കിൽ (ലിഖ(fd, buf, വലുപ്പം(buf)) != വലുപ്പം(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  എങ്കിൽ (pid == 0) {
    exit(0);
  } അല്ലെങ്കിൽ {
    പൂർണ്ണസംഖ്യ xstatus;
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  ആകുമ്പോൾ ((n = പഠ(fd, buf, വലുപ്പം(buf))) > 0) {
    വേണ്ടി (i = 0; i < വലുപ്പം(buf); i++) {
      എങ്കിൽ (buf[i] == 'c')
        nc++;
      എങ്കിൽ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  എങ്കിൽ (nc == N * SZ && np == N * SZ) {
    exit(0);
  } അല്ലെങ്കിൽ {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
ശൂന്യം
fourfiles(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, pid, i, j, n, total, pi;
  അക്ഷരം *names[] = {"f0", "f1", "f2", "f3"};
  അക്ഷരം *fname;
  എണ്ണൽ { N = 12, NCHILD = 4, SZ = 500 };

  വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    എങ്കിൽ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      എങ്കിൽ (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      വേണ്ടി (i = 0; i < N; i++) {
        എങ്കിൽ ((n = ലിഖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  പൂർണ്ണസംഖ്യ xstatus;
  വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0)
      exit(xstatus);
  }

  വേണ്ടി (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    ആകുമ്പോൾ ((n = പഠ(fd, buf, വലുപ്പം(buf))) > 0) {
      വേണ്ടി (j = 0; j < n; j++) {
        എങ്കിൽ (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    എങ്കിൽ (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
ശൂന്യം
createdelete(അക്ഷരം *s)
{
  എണ്ണൽ { N = 20, NCHILD = 4 };
  പൂർണ്ണസംഖ്യ pid, i, fd, pi;
  അക്ഷരം name[32];

  വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    എങ്കിൽ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      വേണ്ടി (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        എങ്കിൽ (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        എങ്കിൽ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          എങ്കിൽ (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  പൂർണ്ണസംഖ്യ xstatus;
  വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  വേണ്ടി (i = 0; i < N; i++) {
    വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      എങ്കിൽ ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } അല്ലെങ്കിൽ എങ്കിൽ ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      എങ്കിൽ (fd >= 0)
        close(fd);
    }
  }

  വേണ്ടി (i = 0; i < N; i++) {
    വേണ്ടി (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
ശൂന്യം
unlinkread(അക്ഷരം *s)
{
  എണ്ണൽ { SZ = 5 };
  പൂർണ്ണസംഖ്യ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  ലിഖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  ലിഖ(fd1, "yyy", 3);
  close(fd1);

  എങ്കിൽ (പഠ(fd, buf, വലുപ്പം(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  എങ്കിൽ (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  എങ്കിൽ (ലിഖ(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

ശൂന്യം
linktest(അക്ഷരം *s)
{
  എണ്ണൽ { SZ = 5 };
  പൂർണ്ണസംഖ്യ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  എങ്കിൽ (ലിഖ(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  എങ്കിൽ (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  എങ്കിൽ (പഠ(fd, buf, വലുപ്പം(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  എങ്കിൽ (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  എങ്കിൽ (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
ശൂന്യം
concreate(അക്ഷരം *s)
{
  എണ്ണൽ { N = 40 };
  അക്ഷരം file[3];
  പൂർണ്ണസംഖ്യ i, pid, n, fd;
  അക്ഷരം fa[N];
  ഘടന {
    ushort inum;
    അക്ഷരം name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  വേണ്ടി (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    എങ്കിൽ (pid && (i % 3) == 1) {
      link("C0", file);
    } അല്ലെങ്കിൽ എങ്കിൽ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } അല്ലെങ്കിൽ {
      fd = open(file, O_CREATE | O_RDWR);
      എങ്കിൽ (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    എങ്കിൽ (pid == 0) {
      exit(0);
    } അല്ലെങ്കിൽ {
      പൂർണ്ണസംഖ്യ xstatus;
      wait(&xstatus);
      എങ്കിൽ (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, വലുപ്പം(fa));
  fd = open(".", 0);
  n = 0;
  ആകുമ്പോൾ (പഠ(fd, &de, വലുപ്പം(de)) > 0) {
    എങ്കിൽ (de.inum == 0)
      തുടരുക;
    എങ്കിൽ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      എങ്കിൽ (i < 0 || i >= വലുപ്പം(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      എങ്കിൽ (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  എങ്കിൽ (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  വേണ്ടി (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } അല്ലെങ്കിൽ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    എങ്കിൽ (pid == 0)
      exit(0);
    അല്ലെങ്കിൽ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
ശൂന്യം
linkunlink(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, i;

  unlink("x");
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned പൂർണ്ണസംഖ്യ x = (pid ? 1 : 97);
  വേണ്ടി (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    എങ്കിൽ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } അല്ലെങ്കിൽ എങ്കിൽ ((x % 3) == 1) {
      link("cat", "x");
    } അല്ലെങ്കിൽ {
      unlink("x");
    }
  }

  എങ്കിൽ (pid)
    wait(0);
  അല്ലെങ്കിൽ
    exit(0);
}

ശൂന്യം
subdir(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, cc;

  unlink("ff");
  എങ്കിൽ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  ലിഖ(fd, "ff", 2);
  close(fd);

  എങ്കിൽ (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  എങ്കിൽ (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  ലിഖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = പഠ(fd, buf, വലുപ്പം(buf));
  എങ്കിൽ (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  എങ്കിൽ (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  എങ്കിൽ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  എങ്കിൽ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  എങ്കിൽ (പഠ(fd, buf, വലുപ്പം(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  എങ്കിൽ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  എങ്കിൽ (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
ശൂന്യം
bigwrite(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd, sz;

  unlink("bigwrite");
  വേണ്ടി (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    എങ്കിൽ (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 0; i < 2; i++) {
      പൂർണ്ണസംഖ്യ cc = ലിഖ(fd, buf, sz);
      എങ്കിൽ (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

ശൂന്യം
bigfile(അക്ഷരം *s)
{
  എണ്ണൽ { N = 20, SZ = 600 };
  പൂർണ്ണസംഖ്യ fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  വേണ്ടി (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    എങ്കിൽ (ലിഖ(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  വേണ്ടി (i = 0;; i++) {
    cc = പഠ(fd, buf, SZ / 2);
    എങ്കിൽ (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    എങ്കിൽ (cc == 0)
      നിർത്തുക;
    എങ്കിൽ (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    എങ്കിൽ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  എങ്കിൽ (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

ശൂന്യം
fourteen(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd;

  // DIRSIZ is 14.

  എങ്കിൽ (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  എങ്കിൽ (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  എങ്കിൽ (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("123456789012345/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    exit(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

ശൂന്യം
rmdot(അക്ഷരം *s)
{
  എങ്കിൽ (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

ശൂന്യം
dirfile(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd;

  fd = open("dirfile", O_CREATE);
  എങ്കിൽ (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  എങ്കിൽ (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  എങ്കിൽ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  എങ്കിൽ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  എങ്കിൽ (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  എങ്കിൽ (ലിഖ(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
ശൂന്യം
iref(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ i, fd;

  വേണ്ടി (i = 0; i < NINODE + 1; i++) {
    എങ്കിൽ (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    എങ്കിൽ (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    എങ്കിൽ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    എങ്കിൽ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  വേണ്ടി (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
ശൂന്യം
forktest(അക്ഷരം *s)
{
  എണ്ണൽ { N = 1000 };
  പൂർണ്ണസംഖ്യ n, pid;

  വേണ്ടി (n = 0; n < N; n++) {
    pid = fork();
    എങ്കിൽ (pid < 0)
      നിർത്തുക;
    എങ്കിൽ (pid == 0)
      exit(0);
  }

  എങ്കിൽ (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  എങ്കിൽ (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  വേണ്ടി (; n > 0; n--) {
    എങ്കിൽ (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  എങ്കിൽ (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

ശൂന്യം
sbrkbasic(അക്ഷരം *s)
{
  എണ്ണൽ { TOOMUCH = 1024 * 1024 * 1024 };
  പൂർണ്ണസംഖ്യ i, pid, xstatus;
  അക്ഷരം *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    a = sbrk(TOOMUCH);
    എങ്കിൽ (a == (അക്ഷരം *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    വേണ്ടി (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  എങ്കിൽ (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  വേണ്ടി (i = 0; i < 5000; i++) {
    b = sbrk(1);
    എങ്കിൽ (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  എങ്കിൽ (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

ശൂന്യം
sbrkmuch(അക്ഷരം *s)
{
  എണ്ണൽ { BIG = 100 * 1024 * 1024 };
  അക്ഷരം *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  എങ്കിൽ (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (അക്ഷരം *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  എങ്കിൽ (c == (അക്ഷരം *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  എങ്കിൽ (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  എങ്കിൽ (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  എങ്കിൽ (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  എങ്കിൽ (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
ശൂന്യം
kernmem(അക്ഷരം *s)
{
  അക്ഷരം *a;
  പൂർണ്ണസംഖ്യ pid;

  വേണ്ടി (a = (അക്ഷരം *)(KERNBASE); a < (അക്ഷരം *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    പൂർണ്ണസംഖ്യ xstatus;
    wait(&xstatus);
    എങ്കിൽ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
ശൂന്യം
MAXVAplus(അക്ഷരം *s)
{
  അസ്ഥിര uint64 a = MAXVA;
  വേണ്ടി (; a != 0; a <<= 1) {
    പൂർണ്ണസംഖ്യ pid;
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    എങ്കിൽ (pid == 0) {
      *(അക്ഷരം *)a = 99;
      printf("%s: oops wrote %p\n", s, (ശൂന്യം *)a);
      exit(1);
    }
    പൂർണ്ണസംഖ്യ xstatus;
    wait(&xstatus);
    എങ്കിൽ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
ശൂന്യം
sbrkfail(അക്ഷരം *s)
{
  എണ്ണൽ { BIG = 100 * 1024 * 1024 };
  പൂർണ്ണസംഖ്യ i, xstatus;
  പൂർണ്ണസംഖ്യ fds[2];
  അക്ഷരം scratch;
  അക്ഷരം *c, *a;
  പൂർണ്ണസംഖ്യ pids[10];
  പൂർണ്ണസംഖ്യ pid;
  പൂർണ്ണസംഖ്യ failed;

  failed = 0;
  എങ്കിൽ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  വേണ്ടി (i = 0; i < വലുപ്പം(pids) / വലുപ്പം(pids[0]); i++) {
    എങ്കിൽ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      എങ്കിൽ (sbrk(BIG - (uint64)sbrk(0)) == (അക്ഷരം *)SBRK_ERROR)
        ലിഖ(fds[1], "0", 1);
      അല്ലെങ്കിൽ
        ലിഖ(fds[1], "1", 1);
      // sit around until killed
      വേണ്ടി (;;)
        pause(1000);
    }
    എങ്കിൽ (pids[i] != -1) {
      പഠ(fds[0], &scratch, 1);
      എങ്കിൽ (scratch == '0')
        failed = 1;
    }
  }
  എങ്കിൽ (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  വേണ്ടി (i = 0; i < വലുപ്പം(pids) / വലുപ്പം(pids[0]); i++) {
    എങ്കിൽ (pids[i] == -1)
      തുടരുക;
    kill(pids[i]);
    wait(0);
  }
  എങ്കിൽ (c == (അക്ഷരം *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    എങ്കിൽ (a == (അക്ഷരം *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  എങ്കിൽ (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
ശൂന്യം
sbrkarg(അക്ഷരം *s)
{
  അക്ഷരം *a;
  പൂർണ്ണസംഖ്യ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  എങ്കിൽ (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  എങ്കിൽ ((n = ലിഖ(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  എങ്കിൽ (pipe((പൂർണ്ണസംഖ്യ *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

ശൂന്യം
validatetest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ hi;
  uint64 p;

  hi = 1100 * 1024;
  വേണ്ടി (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    എങ്കിൽ (link("nosuchfile", (അക്ഷരം *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
അക്ഷരം uninit[10000];
ശൂന്യം
bsstest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി (i = 0; i < വലുപ്പം(uninit); i++) {
    എങ്കിൽ (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
ശൂന്യം
bigargtest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  എങ്കിൽ (pid == 0) {
    സ്ഥിതം അക്ഷരം *args[MAXARG];
    പൂർണ്ണസംഖ്യ i;
    അക്ഷരം big[400];
    memset(big, ' ', വലുപ്പം(big));
    big[വലുപ്പം(big) - 1] = '\0';
    വേണ്ടി (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  എങ്കിൽ (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  എങ്കിൽ (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
ശൂന്യം
fsfull()
{
  പൂർണ്ണസംഖ്യ nfiles;
  പൂർണ്ണസംഖ്യ fsblocks = 0;

  printf("fsfull test\n");

  വേണ്ടി (nfiles = 0;; nfiles++) {
    അക്ഷരം name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    പൂർണ്ണസംഖ്യ fd = open(name, O_CREATE | O_RDWR);
    എങ്കിൽ (fd < 0) {
      printf("open %s failed\n", name);
      നിർത്തുക;
    }
    പൂർണ്ണസംഖ്യ total = 0;
    ആകുമ്പോൾ (1) {
      പൂർണ്ണസംഖ്യ cc = ലിഖ(fd, buf, BSIZE);
      എങ്കിൽ (cc < BSIZE)
        നിർത്തുക;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    എങ്കിൽ (total == 0)
      നിർത്തുക;
  }

  ആകുമ്പോൾ (nfiles >= 0) {
    അക്ഷരം name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  printf("fsfull test finished, %d blocks\n", fsblocks);
}

ശൂന്യം
argptest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fd;
  fd = open("init", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  പഠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
ശൂന്യം
stacktest(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid;
  പൂർണ്ണസംഖ്യ xstatus;

  pid = fork();
  എങ്കിൽ (pid == 0) {
    അക്ഷരം *sp = (അക്ഷരം *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  എങ്കിൽ (xstatus == -1) // kernel killed child?
    exit(0);
  അല്ലെങ്കിൽ
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
ശൂന്യം
nowrite(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid;
  പൂർണ്ണസംഖ്യ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  വേണ്ടി (പൂർണ്ണസംഖ്യ ai = 0; ai < വലുപ്പം(addrs) / വലുപ്പം(addrs[0]); ai++) {
    pid = fork();
    എങ്കിൽ (pid == 0) {
      അസ്ഥിര പൂർണ്ണസംഖ്യ *addr = (പൂർണ്ണസംഖ്യ *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    എങ്കിൽ (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
ശൂന്യം *big = (ശൂന്യം *)0xeaeb0b5b00002f5e;
ശൂന്യം
pgbug(അക്ഷരം *s)
{
  അക്ഷരം *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
ശൂന്യം
sbrkbugs(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    പൂർണ്ണസംഖ്യ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    പൂർണ്ണസംഖ്യ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  എങ്കിൽ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    exit(0);
  }
  wait(0);

  exit(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
ശൂന്യം
sbrklast(അക്ഷരം *s)
{
  uint64 മുകൾ = (uint64)sbrk(0);
  എങ്കിൽ ((മുകൾ % PGSIZE) != 0)
    sbrk(PGSIZE - (മുകൾ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  മുകൾ = (uint64)sbrk(0);
  അക്ഷരം *p = (അക്ഷരം *)(മുകൾ - 64);
  p[0] = 'x';
  p[1] = '\0';
  പൂർണ്ണസംഖ്യ fd = open(p, O_RDWR | O_CREATE);
  ലിഖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  പഠ(fd, p, 1);
  എങ്കിൽ (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
ശൂന്യം
sbrk8000(അക്ഷരം *s)
{
  sbrk(0x80000004);
  അസ്ഥിര അക്ഷരം *മുകൾ = sbrk(0);
  *(മുകൾ - 1) = *(മുകൾ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
ശൂന്യം
badarg(അക്ഷരം *s)
{
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 50000; i++) {
    അക്ഷരം *argv[2];
    argv[0] = (അക്ഷരം *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
ശൂന്യം
lazy_alloc(അക്ഷരം *s)
{
  അക്ഷരം *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  എങ്കിൽ (prev_end == (അക്ഷരം *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  വേണ്ടി (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(അക്ഷരം **)i = i;

  വേണ്ടി (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    എങ്കിൽ (*(അക്ഷരം **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
ശൂന്യം
lazy_unmap(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid;
  അക്ഷരം *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  എങ്കിൽ (prev_end == (അക്ഷരം *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  വേണ്ടി (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(അക്ഷരം **)i = i;

  വേണ്ടി (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("error forking\n");
      exit(1);
    } അല്ലെങ്കിൽ എങ്കിൽ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(അക്ഷരം **)i = i;
      exit(0);
    } അല്ലെങ്കിൽ {
      പൂർണ്ണസംഖ്യ status;
      wait(&status);
      എങ്കിൽ (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

ശൂന്യം
lazy_copy(അക്ഷരം *s)
{
  // copyinstr on lazy page
  {
    അക്ഷരം *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    ശൂന്യം *xx = sbrk(0);
    ശൂന്യം *ret = sbrk(-(((uint64)xx) + 1));
    എങ്കിൽ (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < വലുപ്പം(bad) / വലുപ്പം(bad[0]); i++) {
    പൂർണ്ണസംഖ്യ fd = open("README", 0);
    എങ്കിൽ (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    എങ്കിൽ (പഠ(fd, (അക്ഷരം *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    എങ്കിൽ (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    എങ്കിൽ (ലിഖ(fd, (അക്ഷരം *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

ശൂന്യം
lazy_copyinstr(അക്ഷരം *s)
{
  അക്ഷരം *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  എങ്കിൽ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  പൂർണ്ണസംഖ്യ fd = open(&p[4095], O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  ഘടന stat st;
  പൂർണ്ണസംഖ്യ r = fstat(fd, &st);
  എങ്കിൽ (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  എങ്കിൽ (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

ശൂന്യം
lazy_sbrk(അക്ഷരം *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  അക്ഷരം *p = sbrk(0);
  ആകുമ്പോൾ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    എങ്കിൽ (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  പൂർണ്ണസംഖ്യ n = TRAPFRAME - PGSIZE - (uint64)p;

  അക്ഷരം *p1 = sbrklazy(n);
  എങ്കിൽ (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  എങ്കിൽ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  എങ്കിൽ (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  എങ്കിൽ ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  എങ്കിൽ ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

ശൂന്യം
partial_write(അക്ഷരം *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  പൂർണ്ണസംഖ്യ fd = open("testfile", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  പൂർണ്ണസംഖ്യ cc = ലിഖ(fd, "A", 1);
  എങ്കിൽ (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  അക്ഷരം *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  എങ്കിൽ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = ലിഖ(fd, p - 1, 2);
  എങ്കിൽ (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  അക്ഷരം b;
  cc = പഠ(fd, &b, 1);
  എങ്കിൽ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  എങ്കിൽ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 64; i++) {
    അക്ഷരം buf[1024];
    memset(buf, 0, വലുപ്പം(buf));
    cc = ലിഖ(fd, buf, വലുപ്പം(buf));
    എങ്കിൽ (cc != വലുപ്പം(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = പഠ(fd, &b, 1);
  എങ്കിൽ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  എങ്കിൽ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

ശൂന്യം
unlinkcwd(അക്ഷരം *s)
{
  എങ്കിൽ (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  എങ്കിൽ (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  എങ്കിൽ (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  എങ്കിൽ (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  എങ്കിൽ (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  എങ്കിൽ (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

ഘടന test {
  ശൂന്യം (*f)(അക്ഷരം *);
  അക്ഷരം *s;
} quicktests[] = {
  {copyin, "copyin"},
  {copyout, "copyout"},
  {copyinstr1, "copyinstr1"},
  {copyinstr2, "copyinstr2"},
  {copyinstr3, "copyinstr3"},
  {rwsbrk, "rwsbrk"},
  {truncate1, "truncate1"},
  {truncate2, "truncate2"},
  {truncate3, "truncate3"},
  {openiputtest, "openiput"},
  {exitiputtest, "exitiput"},
  {iputtest, "iput"},
  {opentest, "opentest"},
  {writetest, "writetest"},
  {writebig, "writebig"},
  {createtest, "createtest"},
  {dirtest, "dirtest"},
  {exectest, "exectest"},
  {pipe1, "pipe1"},
  {killstatus, "killstatus"},
  {killzero, "killzero"},
  {preempt, "preempt"},
  {exitwait, "exitwait"},
  {reparent, "reparent"},
  {twochildren, "twochildren"},
  {forkfork, "forkfork"},
  {forkforkfork, "forkforkfork"},
  {reparent2, "reparent2"},
  {mem, "mem"},
  {sharedfd, "sharedfd"},
  {fourfiles, "fourfiles"},
  {createdelete, "createdelete"},
  {unlinkread, "unlinkread"},
  {linktest, "linktest"},
  {concreate, "concreate"},
  {linkunlink, "linkunlink"},
  {subdir, "subdir"},
  {bigwrite, "bigwrite"},
  {bigfile, "bigfile"},
  {fourteen, "fourteen"},
  {rmdot, "rmdot"},
  {dirfile, "dirfile"},
  {iref, "iref"},
  {forktest, "forktest"},
  {sbrkbasic, "sbrkbasic"},
  {sbrkmuch, "sbrkmuch"},
  {kernmem, "kernmem"},
  {MAXVAplus, "MAXVAplus"},
  {sbrkfail, "sbrkfail"},
  {sbrkarg, "sbrkarg"},
  {validatetest, "validatetest"},
  {bsstest, "bsstest"},
  {bigargtest, "bigargtest"},
  {argptest, "argptest"},
  {stacktest, "stacktest"},
  {nowrite, "nowrite"},
  {pgbug, "pgbug"},
  {sbrkbugs, "sbrkbugs"},
  {sbrklast, "sbrklast"},
  {sbrk8000, "sbrk8000"},
  {badarg, "badarg"},
  {lazy_alloc, "lazy_alloc"},
  {lazy_unmap, "lazy_unmap"},
  {lazy_copy, "lazy_copy"},
  {lazy_copyinstr, "lazy_copyinstr"},
  {lazy_sbrk, "lazy_sbrk"},
  {partial_write, "partial_write"},
  {unlinkcwd, "unlinkcwd"},
  {0, 0},
};

//
// Section with tests that take a fair bit of time
//

// directory that uses indirect blocks
ശൂന്യം
bigdir(അക്ഷരം *s)
{
  എണ്ണൽ { N = 500 };
  പൂർണ്ണസംഖ്യ i, fd;
  അക്ഷരം name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  എങ്കിൽ (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  വേണ്ടി (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    എങ്കിൽ (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  വേണ്ടി (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    എങ്കിൽ (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
ശൂന്യം
manywrites(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ nchildren = 4;
  പൂർണ്ണസംഖ്യ howmany = 30; // increase to look for deadlock

  വേണ്ടി (പൂർണ്ണസംഖ്യ ci = 0; ci < nchildren; ci++) {
    പൂർണ്ണസംഖ്യ pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    എങ്കിൽ (pid == 0) {
      അക്ഷരം name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      വേണ്ടി (പൂർണ്ണസംഖ്യ iters = 0; iters < howmany; iters++) {
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < ci + 1; i++) {
          പൂർണ്ണസംഖ്യ fd = open(name, O_CREATE | O_RDWR);
          എങ്കിൽ (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          പൂർണ്ണസംഖ്യ sz = വലുപ്പം(buf);
          പൂർണ്ണസംഖ്യ cc = ലിഖ(fd, buf, sz);
          എങ്കിൽ (cc != sz) {
            printf("%s: write(%d) ret %d\n", s, sz, cc);
            exit(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      exit(0);
    }
  }

  വേണ്ടി (പൂർണ്ണസംഖ്യ ci = 0; ci < nchildren; ci++) {
    പൂർണ്ണസംഖ്യ st = 0;
    wait(&st);
    എങ്കിൽ (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
ശൂന്യം
badwrite(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ assumed_free = 600;

  unlink("junk");
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < assumed_free; i++) {
    പൂർണ്ണസംഖ്യ fd = open("junk", O_CREATE | O_WRONLY);
    എങ്കിൽ (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    ലിഖ(fd, (അക്ഷരം *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  പൂർണ്ണസംഖ്യ fd = open("junk", O_CREATE | O_WRONLY);
  എങ്കിൽ (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  എങ്കിൽ (ലിഖ(fd, "x", 1) != 1) {
    printf("write failed\n");
    exit(1);
  }
  close(fd);
  unlink("junk");

  exit(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
ശൂന്യം
execout(അക്ഷരം *s)
{
  വേണ്ടി (പൂർണ്ണസംഖ്യ avail = 0; avail < 15; avail++) {
    പൂർണ്ണസംഖ്യ pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } അല്ലെങ്കിൽ എങ്കിൽ (pid == 0) {
      // allocate all of memory.
      ആകുമ്പോൾ (1) {
        അക്ഷരം *a = sbrk(PGSIZE);
        എങ്കിൽ (a == SBRK_ERROR)
          നിർത്തുക;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      അക്ഷരം *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } അല്ലെങ്കിൽ {
      wait((പൂർണ്ണസംഖ്യ *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
ശൂന്യം
diskfull(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ fi;
  പൂർണ്ണസംഖ്യ done = 0;

  unlink("diskfulldir");

  വേണ്ടി (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    അക്ഷരം name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    പൂർണ്ണസംഖ്യ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    എങ്കിൽ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      നിർത്തുക;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAXFILE; i++) {
      അക്ഷരം buf[BSIZE];
      എങ്കിൽ (ലിഖ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        നിർത്തുക;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  പൂർണ്ണസംഖ്യ nzz = 128;
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < nzz; i++) {
    അക്ഷരം name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    പൂർണ്ണസംഖ്യ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    എങ്കിൽ (fd < 0)
      നിർത്തുക;
    close(fd);
  }

  // this mkdir() is expected to fail.
  എങ്കിൽ (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < nzz; i++) {
    അക്ഷരം name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; '0' + i < 0177; i++) {
    അക്ഷരം name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

ശൂന്യം
outofinodes(അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ nzz = 32 * 32;
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < nzz; i++) {
    അക്ഷരം name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    പൂർണ്ണസംഖ്യ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    എങ്കിൽ (fd < 0) {
      // failure is eventually expected.
      നിർത്തുക;
    }
    close(fd);
  }

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < nzz; i++) {
    അക്ഷരം name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

ശൂന്യം
linkoverflow(അക്ഷരം *s)
{
  എണ്ണൽ { TARGET = 32768 };
  എണ്ണൽ { DIRS = 64 };
  ഘടന stat st;
  പൂർണ്ണസംഖ്യ i;

  unlink("/lof");
  പൂർണ്ണസംഖ്യ fd = open("/lof", O_CREATE | O_RDWR);
  എങ്കിൽ (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  വേണ്ടി (i = 0; i < TARGET; i++) {
    പൂർണ്ണസംഖ്യ d = i % DIRS;
    പൂർണ്ണസംഖ്യ f = i / DIRS;

    അക്ഷരം pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    എങ്കിൽ (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    എങ്കിൽ (link("/lof", pn) < 0) {
      എങ്കിൽ (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      എങ്കിൽ (st.nlink >= 32767) {
        // overflow check succeeded.
        നിർത്തുക;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    എങ്കിൽ (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  എങ്കിൽ (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  എങ്കിൽ (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

ഘടന test slowtests[] = {
  {bigdir, "bigdir"},
  {manywrites, "manywrites"},
  {badwrite, "badwrite"},
  {execout, "execout"},
  {diskfull, "diskfull"},
  {outofinodes, "outofinodes"},
  // {linkoverflow, "linkoverflow"},

  {0, 0},
};

//
// drive tests
//

// run each test in its own process. run returns 1 if child's exit()
// indicates success.
പൂർണ്ണസംഖ്യ
run(ശൂന്യം f(അക്ഷരം *), അക്ഷരം *s)
{
  പൂർണ്ണസംഖ്യ pid;
  പൂർണ്ണസംഖ്യ xstatus;

  printf("test %s: ", s);
  എങ്കിൽ ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  എങ്കിൽ (pid == 0) {
    f(s);
    exit(0);
  } അല്ലെങ്കിൽ {
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0)
      printf("FAILED\n");
    അല്ലെങ്കിൽ
      printf("OK\n");
    തിരികെ xstatus == 0;
  }
}

പൂർണ്ണസംഖ്യ
runtests(ഘടന test *tests, അക്ഷരം *justone, പൂർണ്ണസംഖ്യ continuous)
{
  പൂർണ്ണസംഖ്യ ntests = 0;
  വേണ്ടി (ഘടന test *t = tests; t->s != 0; t++) {
    എങ്കിൽ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      എങ്കിൽ (!run(t->f, t->s)) {
        എങ്കിൽ (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          തിരികെ -1;
        }
      }
    }
  }
  തിരികെ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
പൂർണ്ണസംഖ്യ
countfree()
{
  പൂർണ്ണസംഖ്യ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ആകുമ്പോൾ (1) {
    അക്ഷരം *a = sbrk(PGSIZE);
    എങ്കിൽ (a == SBRK_ERROR) {
      നിർത്തുക;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  തിരികെ n;
}

പൂർണ്ണസംഖ്യ
drivetests(പൂർണ്ണസംഖ്യ quick, പൂർണ്ണസംഖ്യ continuous, അക്ഷരം *justone)
{
  ചെയ്യുക {
    printf("usertests starting\n");
    പൂർണ്ണസംഖ്യ free0 = countfree();
    പൂർണ്ണസംഖ്യ free1 = 0;
    പൂർണ്ണസംഖ്യ ntests = 0;
    പൂർണ്ണസംഖ്യ n;
    n = runtests(quicktests, justone, continuous);
    എങ്കിൽ (n < 0) {
      എങ്കിൽ (continuous != 2) {
        തിരികെ 1;
      }
    } അല്ലെങ്കിൽ {
      ntests += n;
    }
    എങ്കിൽ (!quick) {
      എങ്കിൽ (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      എങ്കിൽ (n < 0) {
        എങ്കിൽ (continuous != 2) {
          തിരികെ 1;
        }
      } അല്ലെങ്കിൽ {
        ntests += n;
      }
    }
    എങ്കിൽ ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      എങ്കിൽ (continuous != 2) {
        തിരികെ 1;
      }
    }
    എങ്കിൽ (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      തിരികെ 1;
    }
  } ആകുമ്പോൾ (continuous);
  തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ continuous = 0;
  പൂർണ്ണസംഖ്യ quick = 0;
  അക്ഷരം *justone = 0;

  എങ്കിൽ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } അല്ലെങ്കിൽ എങ്കിൽ (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  എങ്കിൽ (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
