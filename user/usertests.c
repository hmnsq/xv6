#ଅନ୍ତର୍ଭାଵ "kernel/param.h"
#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "kernel/syscall.h"
#ଅନ୍ତର୍ଭାଵ "kernel/memlayout.h"
#ଅନ୍ତର୍ଭାଵ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

ଅକ୍ଷର buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
ଶୂନ୍ୟ
copyin(ଅକ୍ଷର *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai = 0; ai < ଆକାର(addrs) / ଆକାର(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("copyin1", O_CREATE | O_WRONLY);
    ଯଦି (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ଲିଖ(fd, (ଶୂନ୍ୟ *)addr, 8192);
    ଯଦି (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (ଶୂନ୍ୟ *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = ଲିଖ(1, (ଅକ୍ଷର *)addr, 8192);
    ଯଦି (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (ଶୂନ୍ୟ *)addr, n);
      exit(1);
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[2];
    ଯଦି (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ଲିଖ(fds[1], (ଅକ୍ଷର *)addr, 8192);
    ଯଦି (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (ଶୂନ୍ୟ *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
ଶୂନ୍ୟ
copyout(ଅକ୍ଷର *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai = 0; ai < ଆକାର(addrs) / ଆକାର(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("README", 0);
    ଯଦି (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ପଠ(fd, (ଶୂନ୍ୟ *)addr, 8192);
    ଯଦି (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (ଶୂନ୍ୟ *)addr, n);
      exit(1);
    }
    close(fd);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[2];
    ଯଦି (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ଲିଖ(fds[1], "x", 1);
    ଯଦି (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = ପଠ(fds[0], (ଶୂନ୍ୟ *)addr, 8192);
    ଯଦି (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (ଶୂନ୍ୟ *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
ଶୂନ୍ୟ
copyinstr1(ଅକ୍ଷର *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai = 0; ai < ଆକାର(addrs) / ଆକାର(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open((ଅକ୍ଷର *)addr, O_CREATE | O_WRONLY);
    ଯଦି (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (ଶୂନ୍ୟ *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
ଶୂନ୍ୟ
copyinstr2(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର b[MAXPATH + 1];

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = unlink(b);
  ଯଦି (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(b, O_CREATE | O_WRONLY);
  ଯଦି (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ଯଦି (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ଅକ୍ଷର *args[] = {"xx", 0};
  ret = exec(b, args);
  ଯଦି (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
  ଯଦି (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
    ସ୍ଥିର ଅକ୍ଷର big[PGSIZE + 1];
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    ଅକ୍ଷର *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ଯଦି (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା st = 0;
  wait(&st);
  ଯଦି (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
ଶୂନ୍ୟ
copyinstr3(ଅକ୍ଷର *s)
{
  sbrk(8192);
  uint64 ଶୀର୍ଷ = (uint64)sbrk(0);
  ଯଦି ((ଶୀର୍ଷ % PGSIZE) != 0) {
    sbrk(PGSIZE - (ଶୀର୍ଷ % PGSIZE));
  }
  ଶୀର୍ଷ = (uint64)sbrk(0);
  ଯଦି (ଶୀର୍ଷ % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  ଅକ୍ଷର *b = (ଅକ୍ଷର *)(ଶୀର୍ଷ - 1);
  *b = 'x';

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = unlink(b);
  ଯଦି (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(b, O_CREATE | O_WRONLY);
  ଯଦି (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ଯଦି (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ଅକ୍ଷର *args[] = {"xx", 0};
  ret = exec(b, args);
  ଯଦି (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
ଶୂନ୍ୟ
rwsbrk(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, n;

  uint64 a = (uint64)sbrk(8192);

  ଯଦି (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  ଯଦି (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ଯଦି (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = ଲିଖ(fd, (ଶୂନ୍ୟ *)(a + PGSIZE), 1024);
  ଯଦି (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (ଶୂନ୍ୟ *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = ପଠ(fd, (ଶୂନ୍ୟ *)(a + PGSIZE), 10);
  ଯଦି (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (ଶୂନ୍ୟ *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
ଶୂନ୍ୟ
truncate1(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର buf[32];

  unlink("truncfile");
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  ଲିଖ(fd1, "abcd", 4);
  close(fd1);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd2 = open("truncfile", O_RDONLY);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ପଠ(fd2, buf, ଆକାର(buf));
  ଯଦି (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd3 = open("truncfile", O_RDONLY);
  n = ପଠ(fd3, buf, ଆକାର(buf));
  ଯଦି (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = ପଠ(fd2, buf, ଆକାର(buf));
  ଯଦି (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  ଲିଖ(fd1, "abcdef", 6);

  n = ପଠ(fd3, buf, ଆକାର(buf));
  ଯଦି (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = ପଠ(fd2, buf, ଆକାର(buf));
  ଯଦି (n != 2) {
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
ଶୂନ୍ୟ
truncate2(ଅକ୍ଷର *s)
{
  unlink("truncfile");

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  ଲିଖ(fd1, "abcd", 4);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ଲିଖ(fd1, "x", 1);
  ଯଦି (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

ଶୂନ୍ୟ
truncate3(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  ଯଦି (pid == 0) {
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 100; i++) {
      ଅକ୍ଷର buf[32];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("truncfile", O_WRONLY);
      ଯଦି (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ଲିଖ(fd, "1234567890", 10);
      ଯଦି (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      ପଠ(fd, buf, ଆକାର(buf));
      close(fd);
    }
    exit(0);
  }

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 150; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ଯଦି (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = ଲିଖ(fd, "xxx", 3);
    ଯଦି (n != 3) {
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
ଶୂନ୍ୟ
iputtest(ଅକ୍ଷର *s)
{
  ଯଦି (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
ଶୂନ୍ୟ
exitiputtest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, xstatus;

  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    ଯଦି (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    ଯଦି (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    ଯଦି (unlink("../iputdir") < 0) {
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
ଶୂନ୍ୟ
openiputtest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, xstatus;

  ଯଦି (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("oidir", O_RDWR);
    ଯଦି (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  ଯଦି (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

ଶୂନ୍ୟ
opentest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  fd = open("echo", 0);
  ଯଦି (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ଯଦି (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
writetest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଗଣନା { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  ପାଇଁ (i = 0; i < N; i++) {
    ଯଦି (ଲିଖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    ଯଦି (ଲିଖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = ପଠ(fd, buf, N * SZ * 2);
  ଯଦି (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
writebig(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  ପାଇଁ (i = 0; i < MAXFILE; i++) {
    ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)buf)[0] = i;
    ଯଦି (ଲିଖ(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  ପାଇଁ (;;) {
    i = ପଠ(fd, buf, BSIZE);
    ଯଦି (i == 0) {
      ଯଦି (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      ଭାଙ୍ଗ;
    } ଅନ୍ୟଥା ଯଦି (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  ଯଦି (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
ଶୂନ୍ୟ
createtest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, fd;
  ଗଣନା { N = 52 };

  ଅକ୍ଷର name[3];
  name[0] = 'a';
  name[2] = '\0';
  ପାଇଁ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ପାଇଁ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

ଶୂନ୍ୟ
dirtest(ଅକ୍ଷର *s)
{
  ଯଦି (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  ଯଦି (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  ଯଦି (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  ଯଦି (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
exectest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, xstatus, pid;
  ଅକ୍ଷର *echoargv[] = {"echo", "OK", 0};
  ଅକ୍ଷର buf[3];

  unlink("echo-ok");
  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା errfd = dup(1);
    ଯଦି (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ଯଦି (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    ଯଦି (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    ଯଦି (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  ଯଦି (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  ଯଦି (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ଯଦି (ପଠ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  ଯଦି (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  ଅନ୍ୟଥା {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

ଶୂନ୍ୟ
pipe1(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[2], pid, xstatus;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା seq, i, n, cc, total;
  ଗଣନା { N = 5, SZ = 1033 };

  ଯଦି (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  ଯଦି (pid == 0) {
    close(fds[0]);
    ପାଇଁ (n = 0; n < N; n++) {
      ପାଇଁ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ଯଦି (ଲିଖ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } ଅନ୍ୟଥା ଯଦି (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    ଯେତେବେଳେ ((n = ପଠ(fds[0], buf, cc)) > 0) {
      ପାଇଁ (i = 0; i < n; i++) {
        ଯଦି ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ଫେରନ୍ତୁ;
        }
      }
      total += n;
      cc = cc * 2;
      ଯଦି (cc > ଆକାର(buf))
        cc = ଆକାର(buf);
    }
    ଯଦି (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } ଅନ୍ୟଥା {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
ଶୂନ୍ୟ
killstatus(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xst;

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 100; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
    ଯଦି (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid1 == 0) {
      ଯେତେବେଳେ (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ଯଦି (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
ଶୂନ୍ୟ
killzero(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, xst;

  kill(0);

  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    exit(7);
  }
  ଯଦି (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  ଯଦି (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
ଶୂନ୍ୟ
preempt(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1, pid2, pid3;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pfds[2];

  pid1 = fork();
  ଯଦି (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ଯଦି (pid1 == 0)
    ପାଇଁ (;;)
      ;

  pid2 = fork();
  ଯଦି (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid2 == 0)
    ପାଇଁ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ଯଦି (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid3 == 0) {
    close(pfds[0]);
    ଯଦି (ଲିଖ(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    ପାଇଁ (;;)
      ;
  }

  close(pfds[1]);
  ଯଦି (ପଠ(pfds[0], buf, ଆକାର(buf)) != 1) {
    printf("%s: preempt read error", s);
    ଫେରନ୍ତୁ;
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
ଶୂନ୍ୟ
exitwait(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pid;

  ପାଇଁ (i = 0; i < 100; i++) {
    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstate;
      ଯଦି (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      ଯଦି (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } ଅନ୍ୟଥା {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
ଶୂନ୍ୟ
reparent(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା master_pid = getpid();
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 200; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid) {
      ଯଦି (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } ଅନ୍ୟଥା {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid2 = fork();
      ଯଦି (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
ଶୂନ୍ୟ
twochildren(ଅକ୍ଷର *s)
{
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 1000; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
    ଯଦି (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid1 == 0) {
      exit(0);
    } ଅନ୍ୟଥା {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid2 = fork();
      ଯଦି (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      ଯଦି (pid2 == 0) {
        exit(0);
      } ଅନ୍ୟଥା {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
ଶୂନ୍ୟ
forkfork(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 2 };

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < N; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    ଯଦି (pid == 0) {
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 200; j++) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
        ଯଦି (pid1 < 0) {
          exit(1);
        }
        ଯଦି (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < N; i++) {
    wait(&xstatus);
    ଯଦି (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

ଶୂନ୍ୟ
forkforkfork(ଅକ୍ଷର *s)
{
  unlink("stopforking");

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    ଯେତେବେଳେ (1) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("stopforking", 0);
      ଯଦି (fd >= 0) {
        exit(0);
      }
      ଯଦି (fork() < 0) {
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
ଶୂନ୍ୟ
reparent2(ଅକ୍ଷର *s)
{
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 800; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
    ଯଦି (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    ଯଦି (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
ଶୂନ୍ୟ
mem(ଅକ୍ଷର *s)
{
  ଶୂନ୍ୟ *m1, *m2;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;

  ଯଦି ((pid = fork()) == 0) {
    m1 = 0;
    ଯେତେବେଳେ ((m2 = malloc(10001)) != 0) {
      *(ଅକ୍ଷର **)m2 = m1;
      m1 = m2;
    }
    ଯେତେବେଳେ (m1) {
      m2 = *(ଅକ୍ଷର **)m1;
      ମୁକ୍ତ(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    ଯଦି (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    ମୁକ୍ତ(m1);
    exit(0);
  } ଅନ୍ୟଥା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
    wait(&xstatus);
    ଯଦି (xstatus == -1) {
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
ଶୂନ୍ୟ
sharedfd(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid, i, n, nc, np;
  ଗଣନା { N = 1000, SZ = 10 };
  ଅକ୍ଷର buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', ଆକାର(buf));
  ପାଇଁ (i = 0; i < N; i++) {
    ଯଦି (ଲିଖ(fd, buf, ଆକାର(buf)) != ଆକାର(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  ଯଦି (pid == 0) {
    exit(0);
  } ଅନ୍ୟଥା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
    wait(&xstatus);
    ଯଦି (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ଯଦି (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  ଯେତେବେଳେ ((n = ପଠ(fd, buf, ଆକାର(buf))) > 0) {
    ପାଇଁ (i = 0; i < ଆକାର(buf); i++) {
      ଯଦି (buf[i] == 'c')
        nc++;
      ଯଦି (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ଯଦି (nc == N * SZ && np == N * SZ) {
    exit(0);
  } ଅନ୍ୟଥା {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
ଶୂନ୍ୟ
fourfiles(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid, i, j, n, total, pi;
  ଅକ୍ଷର *names[] = {"f0", "f1", "f2", "f3"};
  ଅକ୍ଷର *fname;
  ଗଣନା { N = 12, NCHILD = 4, SZ = 500 };

  ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ଯଦି (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ଯଦି (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      ପାଇଁ (i = 0; i < N; i++) {
        ଯଦି ((n = ଲିଖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
  ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ଯଦି (xstatus != 0)
      exit(xstatus);
  }

  ପାଇଁ (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    ଯେତେବେଳେ ((n = ପଠ(fd, buf, ଆକାର(buf))) > 0) {
      ପାଇଁ (j = 0; j < n; j++) {
        ଯଦି (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    ଯଦି (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
ଶୂନ୍ୟ
createdelete(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 20, NCHILD = 4 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, i, fd, pi;
  ଅକ୍ଷର name[32];

  ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ଯଦି (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ପାଇଁ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ଯଦି (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        ଯଦି (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ଯଦି (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
  ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ଯଦି (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  ପାଇଁ (i = 0; i < N; i++) {
    ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ଯଦି ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } ଅନ୍ୟଥା ଯଦି ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      ଯଦି (fd >= 0)
        close(fd);
    }
  }

  ପାଇଁ (i = 0; i < N; i++) {
    ପାଇଁ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
ଶୂନ୍ୟ
unlinkread(ଅକ୍ଷର *s)
{
  ଗଣନା { SZ = 5 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  ଲିଖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  ଲିଖ(fd1, "yyy", 3);
  close(fd1);

  ଯଦି (ପଠ(fd, buf, ଆକାର(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  ଯଦି (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  ଯଦି (ଲିଖ(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

ଶୂନ୍ୟ
linktest(ଅକ୍ଷର *s)
{
  ଗଣନା { SZ = 5 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  ଯଦି (ଲିଖ(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  ଯଦି (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  ଯଦି (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  ଯଦି (ପଠ(fd, buf, ଆକାର(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  ଯଦି (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  ଯଦି (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
ଶୂନ୍ୟ
concreate(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 40 };
  ଅକ୍ଷର file[3];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pid, n, fd;
  ଅକ୍ଷର fa[N];
  ସଂରଚନା {
    ushort inum;
    ଅକ୍ଷର name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ପାଇଁ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ଯଦି (pid && (i % 3) == 1) {
      link("C0", file);
    } ଅନ୍ୟଥା ଯଦି (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ଅନ୍ୟଥା {
      fd = open(file, O_CREATE | O_RDWR);
      ଯଦି (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    ଯଦି (pid == 0) {
      exit(0);
    } ଅନ୍ୟଥା {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
      wait(&xstatus);
      ଯଦି (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, ଆକାର(fa));
  fd = open(".", 0);
  n = 0;
  ଯେତେବେଳେ (ପଠ(fd, &de, ଆକାର(de)) > 0) {
    ଯଦି (de.inum == 0)
      ଜାରି;
    ଯଦି (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ଯଦି (i < 0 || i >= ଆକାର(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      ଯଦି (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ଯଦି (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  ପାଇଁ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ଅନ୍ୟଥା {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ଯଦି (pid == 0)
      exit(0);
    ଅନ୍ୟଥା
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
ଶୂନ୍ୟ
linkunlink(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, i;

  unlink("x");
  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା x = (pid ? 1 : 97);
  ପାଇଁ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ଯଦି ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ଅନ୍ୟଥା ଯଦି ((x % 3) == 1) {
      link("cat", "x");
    } ଅନ୍ୟଥା {
      unlink("x");
    }
  }

  ଯଦି (pid)
    wait(0);
  ଅନ୍ୟଥା
    exit(0);
}

ଶୂନ୍ୟ
subdir(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, cc;

  unlink("ff");
  ଯଦି (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  ଲିଖ(fd, "ff", 2);
  close(fd);

  ଯଦି (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  ଯଦି (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  ଲିଖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ଯଦି (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = ପଠ(fd, buf, ଆକାର(buf));
  ଯଦି (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  ଯଦି (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ଯଦି (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  ଯଦି (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  ଯଦି (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  ଯଦି (ପଠ(fd, buf, ଆକାର(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  ଯଦି (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  ଯଦି (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  ଯଦି (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  ଯଦି (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  ଯଦି (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  ଯଦି (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
ଶୂନ୍ୟ
bigwrite(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, sz;

  unlink("bigwrite");
  ପାଇଁ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ଯଦି (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 0; i < 2; i++) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc = ଲିଖ(fd, buf, sz);
      ଯଦି (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

ଶୂନ୍ୟ
bigfile(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 20, SZ = 600 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  ପାଇଁ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ଯଦି (ଲିଖ(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ଯଦି (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  ପାଇଁ (i = 0;; i++) {
    cc = ପଠ(fd, buf, SZ / 2);
    ଯଦି (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    ଯଦି (cc == 0)
      ଭାଙ୍ଗ;
    ଯଦି (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    ଯଦି (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  ଯଦି (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

ଶୂନ୍ୟ
fourteen(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  // DIRSIZ is 14.

  ଯଦି (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  ଯଦି (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ଯଦି (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ଯଦି (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  ଯଦି (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  ଯଦି (mkdir("123456789012345/12345678901234") == 0) {
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

ଶୂନ୍ୟ
rmdot(ଅକ୍ଷର *s)
{
  ଯଦି (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  ଯଦି (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  ଯଦି (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  ଯଦି (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
dirfile(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  fd = open("dirfile", O_CREATE);
  ଯଦି (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  ଯଦି (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  ଯଦି (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ଯଦି (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ଯଦି (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  ଯଦି (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  ଯଦି (ଲିଖ(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
ଶୂନ୍ୟ
iref(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, fd;

  ପାଇଁ (i = 0; i < NINODE + 1; i++) {
    ଯଦି (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    ଯଦି (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ଯଦି (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ଯଦି (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ପାଇଁ (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
ଶୂନ୍ୟ
forktest(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 1000 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, pid;

  ପାଇଁ (n = 0; n < N; n++) {
    pid = fork();
    ଯଦି (pid < 0)
      ଭାଙ୍ଗ;
    ଯଦି (pid == 0)
      exit(0);
  }

  ଯଦି (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  ଯଦି (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  ପାଇଁ (; n > 0; n--) {
    ଯଦି (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  ଯଦି (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
sbrkbasic(ଅକ୍ଷର *s)
{
  ଗଣନା { TOOMUCH = 1024 * 1024 * 1024 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pid, xstatus;
  ଅକ୍ଷର *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ଯଦି (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
    a = sbrk(TOOMUCH);
    ଯଦି (a == (ଅକ୍ଷର *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    ପାଇଁ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  ଯଦି (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ପାଇଁ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ଯଦି (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ଯଦି (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  ଯଦି (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

ଶୂନ୍ୟ
sbrkmuch(ଅକ୍ଷର *s)
{
  ଗଣନା { BIG = 100 * 1024 * 1024 };
  ଅକ୍ଷର *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  ଯଦି (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (ଅକ୍ଷର *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ଯଦି (c == (ଅକ୍ଷର *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  ଯଦି (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ଯଦି (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  ଯଦି (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ଯଦି (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
ଶୂନ୍ୟ
kernmem(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *a;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;

  ପାଇଁ (a = (ଅକ୍ଷର *)(KERNBASE); a < (ଅକ୍ଷର *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
    wait(&xstatus);
    ଯଦି (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
ଶୂନ୍ୟ
MAXVAplus(ଅକ୍ଷର *s)
{
  ଅସ୍ଥିର uint64 a = MAXVA;
  ପାଇଁ (; a != 0; a <<= 1) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
    pid = fork();
    ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ଯଦି (pid == 0) {
      *(ଅକ୍ଷର *)a = 99;
      printf("%s: oops wrote %p\n", s, (ଶୂନ୍ୟ *)a);
      exit(1);
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
    wait(&xstatus);
    ଯଦି (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
ଶୂନ୍ୟ
sbrkfail(ଅକ୍ଷର *s)
{
  ଗଣନା { BIG = 100 * 1024 * 1024 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, xstatus;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[2];
  ଅକ୍ଷର scratch;
  ଅକ୍ଷର *c, *a;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pids[10];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା failed;

  failed = 0;
  ଯଦି (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  ପାଇଁ (i = 0; i < ଆକାର(pids) / ଆକାର(pids[0]); i++) {
    ଯଦି ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ଯଦି (sbrk(BIG - (uint64)sbrk(0)) == (ଅକ୍ଷର *)SBRK_ERROR)
        ଲିଖ(fds[1], "0", 1);
      ଅନ୍ୟଥା
        ଲିଖ(fds[1], "1", 1);
      // sit around until killed
      ପାଇଁ (;;)
        pause(1000);
    }
    ଯଦି (pids[i] != -1) {
      ପଠ(fds[0], &scratch, 1);
      ଯଦି (scratch == '0')
        failed = 1;
    }
  }
  ଯଦି (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ପାଇଁ (i = 0; i < ଆକାର(pids) / ଆକାର(pids[0]); i++) {
    ଯଦି (pids[i] == -1)
      ଜାରି;
    kill(pids[i]);
    wait(0);
  }
  ଯଦି (c == (ଅକ୍ଷର *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ଯଦି (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ଯଦି (a == (ଅକ୍ଷର *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  ଯଦି (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
ଶୂନ୍ୟ
sbrkarg(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *a;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ଯଦି (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  ଯଦି ((n = ଲିଖ(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ଯଦି (pipe((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

ଶୂନ୍ୟ
validatetest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା hi;
  uint64 p;

  hi = 1100 * 1024;
  ପାଇଁ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ଯଦି (link("nosuchfile", (ଅକ୍ଷର *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
ଅକ୍ଷର uninit[10000];
ଶୂନ୍ୟ
bsstest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ (i = 0; i < ଆକାର(uninit); i++) {
    ଯଦି (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
ଶୂନ୍ୟ
bigargtest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ଯଦି (pid == 0) {
    ସ୍ଥିର ଅକ୍ଷର *args[MAXARG];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅକ୍ଷର big[400];
    memset(big, ' ', ଆକାର(big));
    big[ଆକାର(big) - 1] = '\0';
    ପାଇଁ (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } ଅନ୍ୟଥା ଯଦି (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  ଯଦି (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  ଯଦି (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
ଶୂନ୍ୟ
fsfull()
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfiles;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsblocks = 0;

  printf("fsfull test\n");

  ପାଇଁ (nfiles = 0;; nfiles++) {
    ଅକ୍ଷର name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(name, O_CREATE | O_RDWR);
    ଯଦି (fd < 0) {
      printf("open %s failed\n", name);
      ଭାଙ୍ଗ;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା total = 0;
    ଯେତେବେଳେ (1) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc = ଲିଖ(fd, buf, BSIZE);
      ଯଦି (cc < BSIZE)
        ଭାଙ୍ଗ;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    ଯଦି (total == 0)
      ଭାଙ୍ଗ;
  }

  ଯେତେବେଳେ (nfiles >= 0) {
    ଅକ୍ଷର name[64];
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

ଶୂନ୍ୟ
argptest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  fd = open("init", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ପଠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
ଶୂନ୍ୟ
stacktest(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;

  pid = fork();
  ଯଦି (pid == 0) {
    ଅକ୍ଷର *sp = (ଅକ୍ଷର *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } ଅନ୍ୟଥା ଯଦି (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  ଯଦି (xstatus == -1) // kernel killed child?
    exit(0);
  ଅନ୍ୟଥା
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
ଶୂନ୍ୟ
nowrite(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai = 0; ai < ଆକାର(addrs) / ଆକାର(addrs[0]); ai++) {
    pid = fork();
    ଯଦି (pid == 0) {
      ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *addr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } ଅନ୍ୟଥା ଯଦି (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    ଯଦି (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
ଶୂନ୍ୟ *big = (ଶୂନ୍ୟ *)0xeaeb0b5b00002f5e;
ଶୂନ୍ୟ
pgbug(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
ଶୂନ୍ୟ
sbrkbugs(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
  ଯଦି (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  ଯଦି (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  ଯଦି (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
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
ଶୂନ୍ୟ
sbrklast(ଅକ୍ଷର *s)
{
  uint64 ଶୀର୍ଷ = (uint64)sbrk(0);
  ଯଦି ((ଶୀର୍ଷ % PGSIZE) != 0)
    sbrk(PGSIZE - (ଶୀର୍ଷ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  ଶୀର୍ଷ = (uint64)sbrk(0);
  ଅକ୍ଷର *p = (ଅକ୍ଷର *)(ଶୀର୍ଷ - 64);
  p[0] = 'x';
  p[1] = '\0';
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(p, O_RDWR | O_CREATE);
  ଲିଖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  ପଠ(fd, p, 1);
  ଯଦି (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
ଶୂନ୍ୟ
sbrk8000(ଅକ୍ଷର *s)
{
  sbrk(0x80000004);
  ଅସ୍ଥିର ଅକ୍ଷର *ଶୀର୍ଷ = sbrk(0);
  *(ଶୀର୍ଷ - 1) = *(ଶୀର୍ଷ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
ଶୂନ୍ୟ
badarg(ଅକ୍ଷର *s)
{
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 50000; i++) {
    ଅକ୍ଷର *argv[2];
    argv[0] = (ଅକ୍ଷର *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
ଶୂନ୍ୟ
lazy_alloc(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  ଯଦି (prev_end == (ଅକ୍ଷର *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ପାଇଁ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(ଅକ୍ଷର **)i = i;

  ପାଇଁ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    ଯଦି (*(ଅକ୍ଷର **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
ଶୂନ୍ୟ
lazy_unmap(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
  ଅକ୍ଷର *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  ଯଦି (prev_end == (ଅକ୍ଷର *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ପାଇଁ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(ଅକ୍ଷର **)i = i;

  ପାଇଁ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    ଯଦି (pid < 0) {
      printf("error forking\n");
      exit(1);
    } ଅନ୍ୟଥା ଯଦି (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(ଅକ୍ଷର **)i = i;
      exit(0);
    } ଅନ୍ୟଥା {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
      wait(&status);
      ଯଦି (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

ଶୂନ୍ୟ
lazy_copy(ଅକ୍ଷର *s)
{
  // copyinstr on lazy page
  {
    ଅକ୍ଷର *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    ଶୂନ୍ୟ *xx = sbrk(0);
    ଶୂନ୍ୟ *ret = sbrk(-(((uint64)xx) + 1));
    ଯଦି (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଆକାର(bad) / ଆକାର(bad[0]); i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("README", 0);
    ଯଦି (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    ଯଦି (ପଠ(fd, (ଅକ୍ଷର *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ଯଦି (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    ଯଦି (ଲିଖ(fd, (ଅକ୍ଷର *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

ଶୂନ୍ୟ
lazy_copyinstr(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ଯଦି ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(&p[4095], O_RDONLY);
  ଯଦି (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  ସଂରଚନା stat st;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = fstat(fd, &st);
  ଯଦି (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  ଯଦି (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

ଶୂନ୍ୟ
lazy_sbrk(ଅକ୍ଷର *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  ଅକ୍ଷର *p = sbrk(0);
  ଯେତେବେଳେ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ଯଦି (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = TRAPFRAME - PGSIZE - (uint64)p;

  ଅକ୍ଷର *p1 = sbrklazy(n);
  ଯଦି (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  ଯଦି (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  ଯଦି (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  ଯଦି ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  ଯଦି ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

ଶୂନ୍ୟ
partial_write(ଅକ୍ଷର *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("testfile", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc = ଲିଖ(fd, "A", 1);
  ଯଦି (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ଅକ୍ଷର *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ଯଦି ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = ଲିଖ(fd, p - 1, 2);
  ଯଦି (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ଅକ୍ଷର b;
  cc = ପଠ(fd, &b, 1);
  ଯଦି (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ଯଦି (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 64; i++) {
    ଅକ୍ଷର buf[1024];
    memset(buf, 0, ଆକାର(buf));
    cc = ଲିଖ(fd, buf, ଆକାର(buf));
    ଯଦି (cc != ଆକାର(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ଯଦି (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = ପଠ(fd, &b, 1);
  ଯଦି (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ଯଦି (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

ଶୂନ୍ୟ
unlinkcwd(ଅକ୍ଷର *s)
{
  ଯଦି (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  ଯଦି (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  ଯଦି (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  ଯଦି (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  ଯଦି (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  ଯଦି (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

ସଂରଚନା test {
  ଶୂନ୍ୟ (*f)(ଅକ୍ଷର *);
  ଅକ୍ଷର *s;
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
ଶୂନ୍ୟ
bigdir(ଅକ୍ଷର *s)
{
  ଗଣନା { N = 500 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, fd;
  ଅକ୍ଷର name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ଯଦି (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  ପାଇଁ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ଯଦି (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  ପାଇଁ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ଯଦି (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
ଶୂନ୍ୟ
manywrites(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nchildren = 4;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା howmany = 30; // increase to look for deadlock

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ci = 0; ci < nchildren; ci++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
    ଯଦି (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    ଯଦି (pid == 0) {
      ଅକ୍ଷର name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା iters = 0; iters < howmany; iters++) {
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ci + 1; i++) {
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(name, O_CREATE | O_RDWR);
          ଯଦି (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz = ଆକାର(buf);
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc = ଲିଖ(fd, buf, sz);
          ଯଦି (cc != sz) {
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

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ci = 0; ci < nchildren; ci++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା st = 0;
    wait(&st);
    ଯଦି (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
ଶୂନ୍ୟ
badwrite(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା assumed_free = 600;

  unlink("junk");
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < assumed_free; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("junk", O_CREATE | O_WRONLY);
    ଯଦି (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    ଲିଖ(fd, (ଅକ୍ଷର *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("junk", O_CREATE | O_WRONLY);
  ଯଦି (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  ଯଦି (ଲିଖ(fd, "x", 1) != 1) {
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
ଶୂନ୍ୟ
execout(ଅକ୍ଷର *s)
{
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା avail = 0; avail < 15; avail++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
    ଯଦି (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } ଅନ୍ୟଥା ଯଦି (pid == 0) {
      // allocate all of memory.
      ଯେତେବେଳେ (1) {
        ଅକ୍ଷର *a = sbrk(PGSIZE);
        ଯଦି (a == SBRK_ERROR)
          ଭାଙ୍ଗ;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      ଅକ୍ଷର *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } ଅନ୍ୟଥା {
      wait((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
ଶୂନ୍ୟ
diskfull(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fi;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = 0;

  unlink("diskfulldir");

  ପାଇଁ (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    ଅକ୍ଷର name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ଯଦି (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      ଭାଙ୍ଗ;
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAXFILE; i++) {
      ଅକ୍ଷର buf[BSIZE];
      ଯଦି (ଲିଖ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        ଭାଙ୍ଗ;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nzz = 128;
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nzz; i++) {
    ଅକ୍ଷର name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ଯଦି (fd < 0)
      ଭାଙ୍ଗ;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ଯଦି (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nzz; i++) {
    ଅକ୍ଷର name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; '0' + i < 0177; i++) {
    ଅକ୍ଷର name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

ଶୂନ୍ୟ
outofinodes(ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nzz = 32 * 32;
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nzz; i++) {
    ଅକ୍ଷର name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ଯଦି (fd < 0) {
      // failure is eventually expected.
      ଭାଙ୍ଗ;
    }
    close(fd);
  }

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nzz; i++) {
    ଅକ୍ଷର name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

ଶୂନ୍ୟ
linkoverflow(ଅକ୍ଷର *s)
{
  ଗଣନା { TARGET = 32768 };
  ଗଣନା { DIRS = 64 };
  ସଂରଚନା stat st;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  unlink("/lof");
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("/lof", O_CREATE | O_RDWR);
  ଯଦି (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  ପାଇଁ (i = 0; i < TARGET; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = i % DIRS;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = i / DIRS;

    ଅକ୍ଷର pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ଯଦି (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ଯଦି (link("/lof", pn) < 0) {
      ଯଦି (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      ଯଦି (st.nlink >= 32767) {
        // overflow check succeeded.
        ଭାଙ୍ଗ;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    ଯଦି (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ଯଦି (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  ଯଦି (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

ସଂରଚନା test slowtests[] = {
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
run(ଶୂନ୍ୟ f(ଅକ୍ଷର *), ଅକ୍ଷର *s)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;

  printf("test %s: ", s);
  ଯଦି ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  ଯଦି (pid == 0) {
    f(s);
    exit(0);
  } ଅନ୍ୟଥା {
    wait(&xstatus);
    ଯଦି (xstatus != 0)
      printf("FAILED\n");
    ଅନ୍ୟଥା
      printf("OK\n");
    ଫେରନ୍ତୁ xstatus == 0;
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
runtests(ସଂରଚନା test *tests, ଅକ୍ଷର *justone, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuous)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ntests = 0;
  ପାଇଁ (ସଂରଚନା test *t = tests; t->s != 0; t++) {
    ଯଦି ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ଯଦି (!run(t->f, t->s)) {
        ଯଦି (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ଫେରନ୍ତୁ -1;
        }
      }
    }
  }
  ଫେରନ୍ତୁ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
countfree()
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ଯେତେବେଳେ (1) {
    ଅକ୍ଷର *a = sbrk(PGSIZE);
    ଯଦି (a == SBRK_ERROR) {
      ଭାଙ୍ଗ;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
drivetests(ପୂର୍ଣ୍ଣସଂଖ୍ୟା quick, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuous, ଅକ୍ଷର *justone)
{
  କର {
    printf("usertests starting\n");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା free0 = countfree();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା free1 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ntests = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
    n = runtests(quicktests, justone, continuous);
    ଯଦି (n < 0) {
      ଯଦି (continuous != 2) {
        ଫେରନ୍ତୁ 1;
      }
    } ଅନ୍ୟଥା {
      ntests += n;
    }
    ଯଦି (!quick) {
      ଯଦି (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ଯଦି (n < 0) {
        ଯଦି (continuous != 2) {
          ଫେରନ୍ତୁ 1;
        }
      } ଅନ୍ୟଥା {
        ntests += n;
      }
    }
    ଯଦି ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ଯଦି (continuous != 2) {
        ଫେରନ୍ତୁ 1;
      }
    }
    ଯଦି (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ଫେରନ୍ତୁ 1;
    }
  } ଯେତେବେଳେ (continuous);
  ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuous = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା quick = 0;
  ଅକ୍ଷର *justone = 0;

  ଯଦି (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } ଅନ୍ୟଥା ଯଦି (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ଅନ୍ୟଥା ଯଦି (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ଅନ୍ୟଥା ଯଦି (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ଅନ୍ୟଥା ଯଦି (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  ଯଦି (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
