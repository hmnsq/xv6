#ಅನ್ತರ್ಭಾವ "kernel/param.h"
#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "kernel/syscall.h"
#ಅನ್ತರ್ಭಾವ "kernel/memlayout.h"
#ಅನ್ತರ್ಭಾವ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

ವರ್ಣ buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
ಶೂನ್ಯ
copyin(ವರ್ಣ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ai = 0; ai < ಪರಿಮಾಣ(addrs) / ಪರಿಮಾಣ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಙ್ಕ fd = open("copyin1", O_CREATE | O_WRONLY);
    ಯದಿ (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ n = ಲಿಖ(fd, (ಶೂನ್ಯ *)addr, 8192);
    ಯದಿ (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (ಶೂನ್ಯ *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = ಲಿಖ(1, (ವರ್ಣ *)addr, 8192);
    ಯದಿ (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr, n);
      exit(1);
    }

    ಪೂರ್ಣಾಙ್ಕ fds[2];
    ಯದಿ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ಲಿಖ(fds[1], (ವರ್ಣ *)addr, 8192);
    ಯದಿ (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
ಶೂನ್ಯ
copyout(ವರ್ಣ *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ai = 0; ai < ಪರಿಮಾಣ(addrs) / ಪರಿಮಾಣ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಙ್ಕ fd = open("README", 0);
    ಯದಿ (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ n = ಪಠ(fd, (ಶೂನ್ಯ *)addr, 8192);
    ಯದಿ (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr, n);
      exit(1);
    }
    close(fd);

    ಪೂರ್ಣಾಙ್ಕ fds[2];
    ಯದಿ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = ಲಿಖ(fds[1], "x", 1);
    ಯದಿ (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = ಪಠ(fds[0], (ಶೂನ್ಯ *)addr, 8192);
    ಯದಿ (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
ಶೂನ್ಯ
copyinstr1(ವರ್ಣ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ai = 0; ai < ಪರಿಮಾಣ(addrs) / ಪರಿಮಾಣ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಙ್ಕ fd = open((ವರ್ಣ *)addr, O_CREATE | O_WRONLY);
    ಯದಿ (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (ಶೂನ್ಯ *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
ಶೂನ್ಯ
copyinstr2(ವರ್ಣ *s)
{
  ವರ್ಣ b[MAXPATH + 1];

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ಪೂರ್ಣಾಙ್ಕ ret = unlink(b);
  ಯದಿ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ಪೂರ್ಣಾಙ್ಕ fd = open(b, O_CREATE | O_WRONLY);
  ಯದಿ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ಯದಿ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ವರ್ಣ *args[] = {"xx", 0};
  ret = exec(b, args);
  ಯದಿ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ಪೂರ್ಣಾಙ್ಕ pid = fork();
  ಯದಿ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಸ್ಥಿರ ವರ್ಣ big[PGSIZE + 1];
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    ವರ್ಣ *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ಯದಿ (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  ಪೂರ್ಣಾಙ್ಕ st = 0;
  wait(&st);
  ಯದಿ (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
ಶೂನ್ಯ
copyinstr3(ವರ್ಣ *s)
{
  sbrk(8192);
  uint64 ತುದಿ = (uint64)sbrk(0);
  ಯದಿ ((ತುದಿ % PGSIZE) != 0) {
    sbrk(PGSIZE - (ತುದಿ % PGSIZE));
  }
  ತುದಿ = (uint64)sbrk(0);
  ಯದಿ (ತುದಿ % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  ವರ್ಣ *b = (ವರ್ಣ *)(ತುದಿ - 1);
  *b = 'x';

  ಪೂರ್ಣಾಙ್ಕ ret = unlink(b);
  ಯದಿ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ಪೂರ್ಣಾಙ್ಕ fd = open(b, O_CREATE | O_WRONLY);
  ಯದಿ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ಯದಿ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ವರ್ಣ *args[] = {"xx", 0};
  ret = exec(b, args);
  ಯದಿ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
ಶೂನ್ಯ
rwsbrk(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, n;

  uint64 a = (uint64)sbrk(8192);

  ಯದಿ (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  ಯದಿ (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ಯದಿ (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = ಲಿಖ(fd, (ಶೂನ್ಯ *)(a + PGSIZE), 1024);
  ಯದಿ (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (ಶೂನ್ಯ *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = ಪಠ(fd, (ಶೂನ್ಯ *)(a + PGSIZE), 10);
  ಯದಿ (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (ಶೂನ್ಯ *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
ಶೂನ್ಯ
truncate1(ವರ್ಣ *s)
{
  ವರ್ಣ buf[32];

  unlink("truncfile");
  ಪೂರ್ಣಾಙ್ಕ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  ಲಿಖ(fd1, "abcd", 4);
  close(fd1);

  ಪೂರ್ಣಾಙ್ಕ fd2 = open("truncfile", O_RDONLY);
  ಪೂರ್ಣಾಙ್ಕ n = ಪಠ(fd2, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ಪೂರ್ಣಾಙ್ಕ fd3 = open("truncfile", O_RDONLY);
  n = ಪಠ(fd3, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = ಪಠ(fd2, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  ಲಿಖ(fd1, "abcdef", 6);

  n = ಪಠ(fd3, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = ಪಠ(fd2, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (n != 2) {
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
ಶೂನ್ಯ
truncate2(ವರ್ಣ *s)
{
  unlink("truncfile");

  ಪೂರ್ಣಾಙ್ಕ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  ಲಿಖ(fd1, "abcd", 4);

  ಪೂರ್ಣಾಙ್ಕ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ಪೂರ್ಣಾಙ್ಕ n = ಲಿಖ(fd1, "x", 1);
  ಯದಿ (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

ಶೂನ್ಯ
truncate3(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  ಯದಿ (pid == 0) {
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 100; i++) {
      ವರ್ಣ buf[32];
      ಪೂರ್ಣಾಙ್ಕ fd = open("truncfile", O_WRONLY);
      ಯದಿ (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      ಪೂರ್ಣಾಙ್ಕ n = ಲಿಖ(fd, "1234567890", 10);
      ಯದಿ (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      ಪಠ(fd, buf, ಪರಿಮಾಣ(buf));
      close(fd);
    }
    exit(0);
  }

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 150; i++) {
    ಪೂರ್ಣಾಙ್ಕ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ಯದಿ (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ n = ಲಿಖ(fd, "xxx", 3);
    ಯದಿ (n != 3) {
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
ಶೂನ್ಯ
iputtest(ವರ್ಣ *s)
{
  ಯದಿ (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
ಶೂನ್ಯ
exitiputtest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, xstatus;

  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಯದಿ (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    ಯದಿ (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    ಯದಿ (unlink("../iputdir") < 0) {
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
ಶೂನ್ಯ
openiputtest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, xstatus;

  ಯದಿ (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಪೂರ್ಣಾಙ್ಕ fd = open("oidir", O_RDWR);
    ಯದಿ (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  ಯದಿ (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

ಶೂನ್ಯ
opentest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd;

  fd = open("echo", 0);
  ಯದಿ (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ಯದಿ (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
writetest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಪೂರ್ಣಾಙ್ಕ i;
  ಗಣನಾ { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  ಪ್ರತಿ (i = 0; i < N; i++) {
    ಯದಿ (ಲಿಖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    ಯದಿ (ಲಿಖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = ಪಠ(fd, buf, N * SZ * 2);
  ಯದಿ (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
writebig(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  ಪ್ರತಿ (i = 0; i < MAXFILE; i++) {
    ((ಪೂರ್ಣಾಙ್ಕ *)buf)[0] = i;
    ಯದಿ (ಲಿಖ(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  ಪ್ರತಿ (;;) {
    i = ಪಠ(fd, buf, BSIZE);
    ಯದಿ (i == 0) {
      ಯದಿ (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      ಭಙ್ಗ;
    } ಅನ್ಯಥಾ ಯದಿ (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((ಪೂರ್ಣಾಙ್ಕ *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  ಯದಿ (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
ಶೂನ್ಯ
createtest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ i, fd;
  ಗಣನಾ { N = 52 };

  ವರ್ಣ name[3];
  name[0] = 'a';
  name[2] = '\0';
  ಪ್ರತಿ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ಪ್ರತಿ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

ಶೂನ್ಯ
dirtest(ವರ್ಣ *s)
{
  ಯದಿ (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  ಯದಿ (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  ಯದಿ (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  ಯದಿ (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
exectest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, xstatus, pid;
  ವರ್ಣ *echoargv[] = {"echo", "OK", 0};
  ವರ್ಣ buf[3];

  unlink("echo-ok");
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಪೂರ್ಣಾಙ್ಕ errfd = dup(1);
    ಯದಿ (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ಯದಿ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    ಯದಿ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    ಯದಿ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  ಯದಿ (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  ಯದಿ (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ಯದಿ (ಪಠ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  ಯದಿ (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  ಅನ್ಯಥಾ {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

ಶೂನ್ಯ
pipe1(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fds[2], pid, xstatus;
  ಪೂರ್ಣಾಙ್ಕ seq, i, n, cc, total;
  ಗಣನಾ { N = 5, SZ = 1033 };

  ಯದಿ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  ಯದಿ (pid == 0) {
    close(fds[0]);
    ಪ್ರತಿ (n = 0; n < N; n++) {
      ಪ್ರತಿ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ಯದಿ (ಲಿಖ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } ಅನ್ಯಥಾ ಯದಿ (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    ಯಾವತ್ ((n = ಪಠ(fds[0], buf, cc)) > 0) {
      ಪ್ರತಿ (i = 0; i < n; i++) {
        ಯದಿ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ಪ್ರತ್ಯಾಗಮ;
        }
      }
      total += n;
      cc = cc * 2;
      ಯದಿ (cc > ಪರಿಮಾಣ(buf))
        cc = ಪರಿಮಾಣ(buf);
    }
    ಯದಿ (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } ಅನ್ಯಥಾ {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
ಶೂನ್ಯ
killstatus(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ xst;

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 100; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
    ಯದಿ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid1 == 0) {
      ಯಾವತ್ (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ಯದಿ (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
ಶೂನ್ಯ
killzero(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, xst;

  kill(0);

  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    exit(7);
  }
  ಯದಿ (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  ಯದಿ (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
ಶೂನ್ಯ
preempt(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid1, pid2, pid3;
  ಪೂರ್ಣಾಙ್ಕ pfds[2];

  pid1 = fork();
  ಯದಿ (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ಯದಿ (pid1 == 0)
    ಪ್ರತಿ (;;)
      ;

  pid2 = fork();
  ಯದಿ (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid2 == 0)
    ಪ್ರತಿ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ಯದಿ (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid3 == 0) {
    close(pfds[0]);
    ಯದಿ (ಲಿಖ(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    ಪ್ರತಿ (;;)
      ;
  }

  close(pfds[1]);
  ಯದಿ (ಪಠ(pfds[0], buf, ಪರಿಮಾಣ(buf)) != 1) {
    printf("%s: preempt read error", s);
    ಪ್ರತ್ಯಾಗಮ;
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
ಶೂನ್ಯ
exitwait(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ i, pid;

  ಪ್ರತಿ (i = 0; i < 100; i++) {
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid) {
      ಪೂರ್ಣಾಙ್ಕ xstate;
      ಯದಿ (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      ಯದಿ (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } ಅನ್ಯಥಾ {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
ಶೂನ್ಯ
reparent(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ master_pid = getpid();
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 200; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid) {
      ಯದಿ (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } ಅನ್ಯಥಾ {
      ಪೂರ್ಣಾಙ್ಕ pid2 = fork();
      ಯದಿ (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
ಶೂನ್ಯ
twochildren(ವರ್ಣ *s)
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 1000; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
    ಯದಿ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid1 == 0) {
      exit(0);
    } ಅನ್ಯಥಾ {
      ಪೂರ್ಣಾಙ್ಕ pid2 = fork();
      ಯದಿ (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      ಯದಿ (pid2 == 0) {
        exit(0);
      } ಅನ್ಯಥಾ {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
ಶೂನ್ಯ
forkfork(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 2 };

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < N; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    ಯದಿ (pid == 0) {
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < 200; j++) {
        ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
        ಯದಿ (pid1 < 0) {
          exit(1);
        }
        ಯದಿ (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  ಪೂರ್ಣಾಙ್ಕ xstatus;
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < N; i++) {
    wait(&xstatus);
    ಯದಿ (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

ಶೂನ್ಯ
forkforkfork(ವರ್ಣ *s)
{
  unlink("stopforking");

  ಪೂರ್ಣಾಙ್ಕ pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಯಾವತ್ (1) {
      ಪೂರ್ಣಾಙ್ಕ fd = open("stopforking", 0);
      ಯದಿ (fd >= 0) {
        exit(0);
      }
      ಯದಿ (fork() < 0) {
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
ಶೂನ್ಯ
reparent2(ವರ್ಣ *s)
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 800; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
    ಯದಿ (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    ಯದಿ (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
ಶೂನ್ಯ
mem(ವರ್ಣ *s)
{
  ಶೂನ್ಯ *m1, *m2;
  ಪೂರ್ಣಾಙ್ಕ pid;

  ಯದಿ ((pid = fork()) == 0) {
    m1 = 0;
    ಯಾವತ್ ((m2 = malloc(10001)) != 0) {
      *(ವರ್ಣ **)m2 = m1;
      m1 = m2;
    }
    ಯಾವತ್ (m1) {
      m2 = *(ವರ್ಣ **)m1;
      ಮುಕ್ತ(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    ಯದಿ (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    ಮುಕ್ತ(m1);
    exit(0);
  } ಅನ್ಯಥಾ {
    ಪೂರ್ಣಾಙ್ಕ xstatus;
    wait(&xstatus);
    ಯದಿ (xstatus == -1) {
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
ಶೂನ್ಯ
sharedfd(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, pid, i, n, nc, np;
  ಗಣನಾ { N = 1000, SZ = 10 };
  ವರ್ಣ buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', ಪರಿಮಾಣ(buf));
  ಪ್ರತಿ (i = 0; i < N; i++) {
    ಯದಿ (ಲಿಖ(fd, buf, ಪರಿಮಾಣ(buf)) != ಪರಿಮಾಣ(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  ಯದಿ (pid == 0) {
    exit(0);
  } ಅನ್ಯಥಾ {
    ಪೂರ್ಣಾಙ್ಕ xstatus;
    wait(&xstatus);
    ಯದಿ (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ಯದಿ (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  ಯಾವತ್ ((n = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf))) > 0) {
    ಪ್ರತಿ (i = 0; i < ಪರಿಮಾಣ(buf); i++) {
      ಯದಿ (buf[i] == 'c')
        nc++;
      ಯದಿ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ಯದಿ (nc == N * SZ && np == N * SZ) {
    exit(0);
  } ಅನ್ಯಥಾ {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
ಶೂನ್ಯ
fourfiles(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, pid, i, j, n, total, pi;
  ವರ್ಣ *names[] = {"f0", "f1", "f2", "f3"};
  ವರ್ಣ *fname;
  ಗಣನಾ { N = 12, NCHILD = 4, SZ = 500 };

  ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ಯದಿ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ಯದಿ (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      ಪ್ರತಿ (i = 0; i < N; i++) {
        ಯದಿ ((n = ಲಿಖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  ಪೂರ್ಣಾಙ್ಕ xstatus;
  ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ಯದಿ (xstatus != 0)
      exit(xstatus);
  }

  ಪ್ರತಿ (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    ಯಾವತ್ ((n = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf))) > 0) {
      ಪ್ರತಿ (j = 0; j < n; j++) {
        ಯದಿ (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    ಯದಿ (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
ಶೂನ್ಯ
createdelete(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 20, NCHILD = 4 };
  ಪೂರ್ಣಾಙ್ಕ pid, i, fd, pi;
  ವರ್ಣ name[32];

  ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ಯದಿ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ಪ್ರತಿ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ಯದಿ (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        ಯದಿ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ಯದಿ (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  ಪೂರ್ಣಾಙ್ಕ xstatus;
  ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ಯದಿ (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  ಪ್ರತಿ (i = 0; i < N; i++) {
    ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ಯದಿ ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } ಅನ್ಯಥಾ ಯದಿ ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      ಯದಿ (fd >= 0)
        close(fd);
    }
  }

  ಪ್ರತಿ (i = 0; i < N; i++) {
    ಪ್ರತಿ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
ಶೂನ್ಯ
unlinkread(ವರ್ಣ *s)
{
  ಗಣನಾ { SZ = 5 };
  ಪೂರ್ಣಾಙ್ಕ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  ಲಿಖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  ಲಿಖ(fd1, "yyy", 3);
  close(fd1);

  ಯದಿ (ಪಠ(fd, buf, ಪರಿಮಾಣ(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  ಯದಿ (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  ಯದಿ (ಲಿಖ(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

ಶೂನ್ಯ
linktest(ವರ್ಣ *s)
{
  ಗಣನಾ { SZ = 5 };
  ಪೂರ್ಣಾಙ್ಕ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  ಯದಿ (ಲಿಖ(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  ಯದಿ (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  ಯದಿ (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  ಯದಿ (ಪಠ(fd, buf, ಪರಿಮಾಣ(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  ಯದಿ (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  ಯದಿ (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
ಶೂನ್ಯ
concreate(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 40 };
  ವರ್ಣ file[3];
  ಪೂರ್ಣಾಙ್ಕ i, pid, n, fd;
  ವರ್ಣ fa[N];
  ಸಂರಚನಾ {
    ushort inum;
    ವರ್ಣ name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ಪ್ರತಿ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ಯದಿ (pid && (i % 3) == 1) {
      link("C0", file);
    } ಅನ್ಯಥಾ ಯದಿ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ಅನ್ಯಥಾ {
      fd = open(file, O_CREATE | O_RDWR);
      ಯದಿ (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    ಯದಿ (pid == 0) {
      exit(0);
    } ಅನ್ಯಥಾ {
      ಪೂರ್ಣಾಙ್ಕ xstatus;
      wait(&xstatus);
      ಯದಿ (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, ಪರಿಮಾಣ(fa));
  fd = open(".", 0);
  n = 0;
  ಯಾವತ್ (ಪಠ(fd, &de, ಪರಿಮಾಣ(de)) > 0) {
    ಯದಿ (de.inum == 0)
      ಅನುವರ್ತ;
    ಯದಿ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ಯದಿ (i < 0 || i >= ಪರಿಮಾಣ(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      ಯದಿ (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ಯದಿ (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  ಪ್ರತಿ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ಅನ್ಯಥಾ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ಯದಿ (pid == 0)
      exit(0);
    ಅನ್ಯಥಾ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
ಶೂನ್ಯ
linkunlink(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, i;

  unlink("x");
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned ಪೂರ್ಣಾಙ್ಕ x = (pid ? 1 : 97);
  ಪ್ರತಿ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ಯದಿ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ಅನ್ಯಥಾ ಯದಿ ((x % 3) == 1) {
      link("cat", "x");
    } ಅನ್ಯಥಾ {
      unlink("x");
    }
  }

  ಯದಿ (pid)
    wait(0);
  ಅನ್ಯಥಾ
    exit(0);
}

ಶೂನ್ಯ
subdir(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, cc;

  unlink("ff");
  ಯದಿ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  ಲಿಖ(fd, "ff", 2);
  close(fd);

  ಯದಿ (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  ಯದಿ (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  ಲಿಖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ಯದಿ (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf));
  ಯದಿ (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  ಯದಿ (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ಯದಿ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  ಯದಿ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  ಯದಿ (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  ಯದಿ (ಪಠ(fd, buf, ಪರಿಮಾಣ(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  ಯದಿ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  ಯದಿ (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
ಶೂನ್ಯ
bigwrite(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd, sz;

  unlink("bigwrite");
  ಪ್ರತಿ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ಯದಿ (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 0; i < 2; i++) {
      ಪೂರ್ಣಾಙ್ಕ cc = ಲಿಖ(fd, buf, sz);
      ಯದಿ (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

ಶೂನ್ಯ
bigfile(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 20, SZ = 600 };
  ಪೂರ್ಣಾಙ್ಕ fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  ಪ್ರತಿ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ಯದಿ (ಲಿಖ(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ಯದಿ (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  ಪ್ರತಿ (i = 0;; i++) {
    cc = ಪಠ(fd, buf, SZ / 2);
    ಯದಿ (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    ಯದಿ (cc == 0)
      ಭಙ್ಗ;
    ಯದಿ (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    ಯದಿ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  ಯದಿ (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

ಶೂನ್ಯ
fourteen(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd;

  // DIRSIZ is 14.

  ಯದಿ (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ಯದಿ (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ಯದಿ (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  ಯದಿ (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("123456789012345/12345678901234") == 0) {
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

ಶೂನ್ಯ
rmdot(ವರ್ಣ *s)
{
  ಯದಿ (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  ಯದಿ (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
dirfile(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd;

  fd = open("dirfile", O_CREATE);
  ಯದಿ (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  ಯದಿ (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  ಯದಿ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ಯದಿ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ಯದಿ (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  ಯದಿ (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  ಯದಿ (ಲಿಖ(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
ಶೂನ್ಯ
iref(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ i, fd;

  ಪ್ರತಿ (i = 0; i < NINODE + 1; i++) {
    ಯದಿ (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    ಯದಿ (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ಯದಿ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ಯದಿ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ಪ್ರತಿ (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
ಶೂನ್ಯ
forktest(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 1000 };
  ಪೂರ್ಣಾಙ್ಕ n, pid;

  ಪ್ರತಿ (n = 0; n < N; n++) {
    pid = fork();
    ಯದಿ (pid < 0)
      ಭಙ್ಗ;
    ಯದಿ (pid == 0)
      exit(0);
  }

  ಯದಿ (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  ಯದಿ (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  ಪ್ರತಿ (; n > 0; n--) {
    ಯದಿ (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  ಯದಿ (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
sbrkbasic(ವರ್ಣ *s)
{
  ಗಣನಾ { TOOMUCH = 1024 * 1024 * 1024 };
  ಪೂರ್ಣಾಙ್ಕ i, pid, xstatus;
  ವರ್ಣ *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
    a = sbrk(TOOMUCH);
    ಯದಿ (a == (ವರ್ಣ *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    ಪ್ರತಿ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  ಯದಿ (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ಪ್ರತಿ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ಯದಿ (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ಯದಿ (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

ಶೂನ್ಯ
sbrkmuch(ವರ್ಣ *s)
{
  ಗಣನಾ { BIG = 100 * 1024 * 1024 };
  ವರ್ಣ *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  ಯದಿ (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (ವರ್ಣ *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ಯದಿ (c == (ವರ್ಣ *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  ಯದಿ (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ಯದಿ (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  ಯದಿ (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ಯದಿ (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
ಶೂನ್ಯ
kernmem(ವರ್ಣ *s)
{
  ವರ್ಣ *a;
  ಪೂರ್ಣಾಙ್ಕ pid;

  ಪ್ರತಿ (a = (ವರ್ಣ *)(KERNBASE); a < (ವರ್ಣ *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ xstatus;
    wait(&xstatus);
    ಯದಿ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
ಶೂನ್ಯ
MAXVAplus(ವರ್ಣ *s)
{
  ಚಞ್ಚಲ uint64 a = MAXVA;
  ಪ್ರತಿ (; a != 0; a <<= 1) {
    ಪೂರ್ಣಾಙ್ಕ pid;
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ಯದಿ (pid == 0) {
      *(ವರ್ಣ *)a = 99;
      printf("%s: oops wrote %p\n", s, (ಶೂನ್ಯ *)a);
      exit(1);
    }
    ಪೂರ್ಣಾಙ್ಕ xstatus;
    wait(&xstatus);
    ಯದಿ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
ಶೂನ್ಯ
sbrkfail(ವರ್ಣ *s)
{
  ಗಣನಾ { BIG = 100 * 1024 * 1024 };
  ಪೂರ್ಣಾಙ್ಕ i, xstatus;
  ಪೂರ್ಣಾಙ್ಕ fds[2];
  ವರ್ಣ scratch;
  ವರ್ಣ *c, *a;
  ಪೂರ್ಣಾಙ್ಕ pids[10];
  ಪೂರ್ಣಾಙ್ಕ pid;
  ಪೂರ್ಣಾಙ್ಕ failed;

  failed = 0;
  ಯದಿ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  ಪ್ರತಿ (i = 0; i < ಪರಿಮಾಣ(pids) / ಪರಿಮಾಣ(pids[0]); i++) {
    ಯದಿ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ಯದಿ (sbrk(BIG - (uint64)sbrk(0)) == (ವರ್ಣ *)SBRK_ERROR)
        ಲಿಖ(fds[1], "0", 1);
      ಅನ್ಯಥಾ
        ಲಿಖ(fds[1], "1", 1);
      // sit around until killed
      ಪ್ರತಿ (;;)
        pause(1000);
    }
    ಯದಿ (pids[i] != -1) {
      ಪಠ(fds[0], &scratch, 1);
      ಯದಿ (scratch == '0')
        failed = 1;
    }
  }
  ಯದಿ (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ಪ್ರತಿ (i = 0; i < ಪರಿಮಾಣ(pids) / ಪರಿಮಾಣ(pids[0]); i++) {
    ಯದಿ (pids[i] == -1)
      ಅನುವರ್ತ;
    kill(pids[i]);
    wait(0);
  }
  ಯದಿ (c == (ವರ್ಣ *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ಯದಿ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ಯದಿ (a == (ವರ್ಣ *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  ಯದಿ (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
ಶೂನ್ಯ
sbrkarg(ವರ್ಣ *s)
{
  ವರ್ಣ *a;
  ಪೂರ್ಣಾಙ್ಕ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ಯದಿ (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  ಯದಿ ((n = ಲಿಖ(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ಯದಿ (pipe((ಪೂರ್ಣಾಙ್ಕ *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

ಶೂನ್ಯ
validatetest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ hi;
  uint64 p;

  hi = 1100 * 1024;
  ಪ್ರತಿ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ಯದಿ (link("nosuchfile", (ವರ್ಣ *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
ವರ್ಣ uninit[10000];
ಶೂನ್ಯ
bsstest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ (i = 0; i < ಪರಿಮಾಣ(uninit); i++) {
    ಯದಿ (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
ಶೂನ್ಯ
bigargtest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ಯದಿ (pid == 0) {
    ಸ್ಥಿರ ವರ್ಣ *args[MAXARG];
    ಪೂರ್ಣಾಙ್ಕ i;
    ವರ್ಣ big[400];
    memset(big, ' ', ಪರಿಮಾಣ(big));
    big[ಪರಿಮಾಣ(big) - 1] = '\0';
    ಪ್ರತಿ (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  ಯದಿ (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  ಯದಿ (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
ಶೂನ್ಯ
fsfull()
{
  ಪೂರ್ಣಾಙ್ಕ nfiles;
  ಪೂರ್ಣಾಙ್ಕ fsblocks = 0;

  printf("fsfull test\n");

  ಪ್ರತಿ (nfiles = 0;; nfiles++) {
    ವರ್ಣ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    ಪೂರ್ಣಾಙ್ಕ fd = open(name, O_CREATE | O_RDWR);
    ಯದಿ (fd < 0) {
      printf("open %s failed\n", name);
      ಭಙ್ಗ;
    }
    ಪೂರ್ಣಾಙ್ಕ total = 0;
    ಯಾವತ್ (1) {
      ಪೂರ್ಣಾಙ್ಕ cc = ಲಿಖ(fd, buf, BSIZE);
      ಯದಿ (cc < BSIZE)
        ಭಙ್ಗ;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    ಯದಿ (total == 0)
      ಭಙ್ಗ;
  }

  ಯಾವತ್ (nfiles >= 0) {
    ವರ್ಣ name[64];
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

ಶೂನ್ಯ
argptest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  fd = open("init", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ಪಠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
ಶೂನ್ಯ
stacktest(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid;
  ಪೂರ್ಣಾಙ್ಕ xstatus;

  pid = fork();
  ಯದಿ (pid == 0) {
    ವರ್ಣ *sp = (ವರ್ಣ *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  ಯದಿ (xstatus == -1) // kernel killed child?
    exit(0);
  ಅನ್ಯಥಾ
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
ಶೂನ್ಯ
nowrite(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid;
  ಪೂರ್ಣಾಙ್ಕ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ai = 0; ai < ಪರಿಮಾಣ(addrs) / ಪರಿಮಾಣ(addrs[0]); ai++) {
    pid = fork();
    ಯದಿ (pid == 0) {
      ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ *addr = (ಪೂರ್ಣಾಙ್ಕ *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    ಯದಿ (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
ಶೂನ್ಯ *big = (ಶೂನ್ಯ *)0xeaeb0b5b00002f5e;
ಶೂನ್ಯ
pgbug(ವರ್ಣ *s)
{
  ವರ್ಣ *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
ಶೂನ್ಯ
sbrkbugs(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid = fork();
  ಯದಿ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಪೂರ್ಣಾಙ್ಕ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  ಯದಿ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
    ಪೂರ್ಣಾಙ್ಕ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  ಯದಿ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
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
ಶೂನ್ಯ
sbrklast(ವರ್ಣ *s)
{
  uint64 ತುದಿ = (uint64)sbrk(0);
  ಯದಿ ((ತುದಿ % PGSIZE) != 0)
    sbrk(PGSIZE - (ತುದಿ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  ತುದಿ = (uint64)sbrk(0);
  ವರ್ಣ *p = (ವರ್ಣ *)(ತುದಿ - 64);
  p[0] = 'x';
  p[1] = '\0';
  ಪೂರ್ಣಾಙ್ಕ fd = open(p, O_RDWR | O_CREATE);
  ಲಿಖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  ಪಠ(fd, p, 1);
  ಯದಿ (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
ಶೂನ್ಯ
sbrk8000(ವರ್ಣ *s)
{
  sbrk(0x80000004);
  ಚಞ್ಚಲ ವರ್ಣ *ತುದಿ = sbrk(0);
  *(ತುದಿ - 1) = *(ತುದಿ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
ಶೂನ್ಯ
badarg(ವರ್ಣ *s)
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 50000; i++) {
    ವರ್ಣ *argv[2];
    argv[0] = (ವರ್ಣ *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
ಶೂನ್ಯ
lazy_alloc(ವರ್ಣ *s)
{
  ವರ್ಣ *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  ಯದಿ (prev_end == (ವರ್ಣ *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ಪ್ರತಿ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(ವರ್ಣ **)i = i;

  ಪ್ರತಿ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    ಯದಿ (*(ವರ್ಣ **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
ಶೂನ್ಯ
lazy_unmap(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid;
  ವರ್ಣ *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  ಯದಿ (prev_end == (ವರ್ಣ *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ಪ್ರತಿ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(ವರ್ಣ **)i = i;

  ಪ್ರತಿ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("error forking\n");
      exit(1);
    } ಅನ್ಯಥಾ ಯದಿ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(ವರ್ಣ **)i = i;
      exit(0);
    } ಅನ್ಯಥಾ {
      ಪೂರ್ಣಾಙ್ಕ status;
      wait(&status);
      ಯದಿ (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

ಶೂನ್ಯ
lazy_copy(ವರ್ಣ *s)
{
  // copyinstr on lazy page
  {
    ವರ್ಣ *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    ಶೂನ್ಯ *xx = sbrk(0);
    ಶೂನ್ಯ *ret = sbrk(-(((uint64)xx) + 1));
    ಯದಿ (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಪರಿಮಾಣ(bad) / ಪರಿಮಾಣ(bad[0]); i++) {
    ಪೂರ್ಣಾಙ್ಕ fd = open("README", 0);
    ಯದಿ (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    ಯದಿ (ಪಠ(fd, (ವರ್ಣ *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ಯದಿ (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    ಯದಿ (ಲಿಖ(fd, (ವರ್ಣ *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

ಶೂನ್ಯ
lazy_copyinstr(ವರ್ಣ *s)
{
  ವರ್ಣ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ಯದಿ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ಪೂರ್ಣಾಙ್ಕ fd = open(&p[4095], O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  ಸಂರಚನಾ stat st;
  ಪೂರ್ಣಾಙ್ಕ r = fstat(fd, &st);
  ಯದಿ (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  ಯದಿ (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

ಶೂನ್ಯ
lazy_sbrk(ವರ್ಣ *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  ವರ್ಣ *p = sbrk(0);
  ಯಾವತ್ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ಯದಿ (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  ಪೂರ್ಣಾಙ್ಕ n = TRAPFRAME - PGSIZE - (uint64)p;

  ವರ್ಣ *p1 = sbrklazy(n);
  ಯದಿ (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  ಯದಿ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  ಯದಿ (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  ಯದಿ ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  ಯದಿ ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

ಶೂನ್ಯ
partial_write(ವರ್ಣ *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ಪೂರ್ಣಾಙ್ಕ fd = open("testfile", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  ಪೂರ್ಣಾಙ್ಕ cc = ಲಿಖ(fd, "A", 1);
  ಯದಿ (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ವರ್ಣ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ಯದಿ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = ಲಿಖ(fd, p - 1, 2);
  ಯದಿ (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ವರ್ಣ b;
  cc = ಪಠ(fd, &b, 1);
  ಯದಿ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ಯದಿ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 64; i++) {
    ವರ್ಣ buf[1024];
    memset(buf, 0, ಪರಿಮಾಣ(buf));
    cc = ಲಿಖ(fd, buf, ಪರಿಮಾಣ(buf));
    ಯದಿ (cc != ಪರಿಮಾಣ(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ಯದಿ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = ಪಠ(fd, &b, 1);
  ಯದಿ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ಯದಿ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

ಶೂನ್ಯ
unlinkcwd(ವರ್ಣ *s)
{
  ಯದಿ (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  ಯದಿ (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  ಯದಿ (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  ಯದಿ (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  ಯದಿ (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  ಯದಿ (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

ಸಂರಚನಾ test {
  ಶೂನ್ಯ (*f)(ವರ್ಣ *);
  ವರ್ಣ *s;
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
ಶೂನ್ಯ
bigdir(ವರ್ಣ *s)
{
  ಗಣನಾ { N = 500 };
  ಪೂರ್ಣಾಙ್ಕ i, fd;
  ವರ್ಣ name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ಯದಿ (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  ಪ್ರತಿ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ಯದಿ (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  ಪ್ರತಿ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ಯದಿ (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
ಶೂನ್ಯ
manywrites(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ nchildren = 4;
  ಪೂರ್ಣಾಙ್ಕ howmany = 30; // increase to look for deadlock

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ci = 0; ci < nchildren; ci++) {
    ಪೂರ್ಣಾಙ್ಕ pid = fork();
    ಯದಿ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    ಯದಿ (pid == 0) {
      ವರ್ಣ name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ iters = 0; iters < howmany; iters++) {
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ci + 1; i++) {
          ಪೂರ್ಣಾಙ್ಕ fd = open(name, O_CREATE | O_RDWR);
          ಯದಿ (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          ಪೂರ್ಣಾಙ್ಕ sz = ಪರಿಮಾಣ(buf);
          ಪೂರ್ಣಾಙ್ಕ cc = ಲಿಖ(fd, buf, sz);
          ಯದಿ (cc != sz) {
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

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ci = 0; ci < nchildren; ci++) {
    ಪೂರ್ಣಾಙ್ಕ st = 0;
    wait(&st);
    ಯದಿ (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
ಶೂನ್ಯ
badwrite(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ assumed_free = 600;

  unlink("junk");
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < assumed_free; i++) {
    ಪೂರ್ಣಾಙ್ಕ fd = open("junk", O_CREATE | O_WRONLY);
    ಯದಿ (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    ಲಿಖ(fd, (ವರ್ಣ *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ಪೂರ್ಣಾಙ್ಕ fd = open("junk", O_CREATE | O_WRONLY);
  ಯದಿ (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  ಯದಿ (ಲಿಖ(fd, "x", 1) != 1) {
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
ಶೂನ್ಯ
execout(ವರ್ಣ *s)
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ avail = 0; avail < 15; avail++) {
    ಪೂರ್ಣಾಙ್ಕ pid = fork();
    ಯದಿ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } ಅನ್ಯಥಾ ಯದಿ (pid == 0) {
      // allocate all of memory.
      ಯಾವತ್ (1) {
        ವರ್ಣ *a = sbrk(PGSIZE);
        ಯದಿ (a == SBRK_ERROR)
          ಭಙ್ಗ;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      ವರ್ಣ *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } ಅನ್ಯಥಾ {
      wait((ಪೂರ್ಣಾಙ್ಕ *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
ಶೂನ್ಯ
diskfull(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ fi;
  ಪೂರ್ಣಾಙ್ಕ done = 0;

  unlink("diskfulldir");

  ಪ್ರತಿ (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    ವರ್ಣ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಙ್ಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಯದಿ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      ಭಙ್ಗ;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < MAXFILE; i++) {
      ವರ್ಣ buf[BSIZE];
      ಯದಿ (ಲಿಖ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        ಭಙ್ಗ;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ಪೂರ್ಣಾಙ್ಕ nzz = 128;
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nzz; i++) {
    ವರ್ಣ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಙ್ಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಯದಿ (fd < 0)
      ಭಙ್ಗ;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ಯದಿ (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nzz; i++) {
    ವರ್ಣ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; '0' + i < 0177; i++) {
    ವರ್ಣ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

ಶೂನ್ಯ
outofinodes(ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ nzz = 32 * 32;
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nzz; i++) {
    ವರ್ಣ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಙ್ಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಯದಿ (fd < 0) {
      // failure is eventually expected.
      ಭಙ್ಗ;
    }
    close(fd);
  }

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nzz; i++) {
    ವರ್ಣ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

ಶೂನ್ಯ
linkoverflow(ವರ್ಣ *s)
{
  ಗಣನಾ { TARGET = 32768 };
  ಗಣನಾ { DIRS = 64 };
  ಸಂರಚನಾ stat st;
  ಪೂರ್ಣಾಙ್ಕ i;

  unlink("/lof");
  ಪೂರ್ಣಾಙ್ಕ fd = open("/lof", O_CREATE | O_RDWR);
  ಯದಿ (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  ಪ್ರತಿ (i = 0; i < TARGET; i++) {
    ಪೂರ್ಣಾಙ್ಕ d = i % DIRS;
    ಪೂರ್ಣಾಙ್ಕ f = i / DIRS;

    ವರ್ಣ pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ಯದಿ (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ಯದಿ (link("/lof", pn) < 0) {
      ಯದಿ (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      ಯದಿ (st.nlink >= 32767) {
        // overflow check succeeded.
        ಭಙ್ಗ;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    ಯದಿ (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ಯದಿ (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  ಯದಿ (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

ಸಂರಚನಾ test slowtests[] = {
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
ಪೂರ್ಣಾಙ್ಕ
run(ಶೂನ್ಯ f(ವರ್ಣ *), ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ pid;
  ಪೂರ್ಣಾಙ್ಕ xstatus;

  printf("test %s: ", s);
  ಯದಿ ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  ಯದಿ (pid == 0) {
    f(s);
    exit(0);
  } ಅನ್ಯಥಾ {
    wait(&xstatus);
    ಯದಿ (xstatus != 0)
      printf("FAILED\n");
    ಅನ್ಯಥಾ
      printf("OK\n");
    ಪ್ರತ್ಯಾಗಮ xstatus == 0;
  }
}

ಪೂರ್ಣಾಙ್ಕ
runtests(ಸಂರಚನಾ test *tests, ವರ್ಣ *justone, ಪೂರ್ಣಾಙ್ಕ continuous)
{
  ಪೂರ್ಣಾಙ್ಕ ntests = 0;
  ಪ್ರತಿ (ಸಂರಚನಾ test *t = tests; t->s != 0; t++) {
    ಯದಿ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ಯದಿ (!run(t->f, t->s)) {
        ಯದಿ (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ಪ್ರತ್ಯಾಗಮ -1;
        }
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ಪೂರ್ಣಾಙ್ಕ
countfree()
{
  ಪೂರ್ಣಾಙ್ಕ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ಯಾವತ್ (1) {
    ವರ್ಣ *a = sbrk(PGSIZE);
    ಯದಿ (a == SBRK_ERROR) {
      ಭಙ್ಗ;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ
drivetests(ಪೂರ್ಣಾಙ್ಕ quick, ಪೂರ್ಣಾಙ್ಕ continuous, ವರ್ಣ *justone)
{
  ಕುರು {
    printf("usertests starting\n");
    ಪೂರ್ಣಾಙ್ಕ free0 = countfree();
    ಪೂರ್ಣಾಙ್ಕ free1 = 0;
    ಪೂರ್ಣಾಙ್ಕ ntests = 0;
    ಪೂರ್ಣಾಙ್ಕ n;
    n = runtests(quicktests, justone, continuous);
    ಯದಿ (n < 0) {
      ಯದಿ (continuous != 2) {
        ಪ್ರತ್ಯಾಗಮ 1;
      }
    } ಅನ್ಯಥಾ {
      ntests += n;
    }
    ಯದಿ (!quick) {
      ಯದಿ (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ಯದಿ (n < 0) {
        ಯದಿ (continuous != 2) {
          ಪ್ರತ್ಯಾಗಮ 1;
        }
      } ಅನ್ಯಥಾ {
        ntests += n;
      }
    }
    ಯದಿ ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ಯದಿ (continuous != 2) {
        ಪ್ರತ್ಯಾಗಮ 1;
      }
    }
    ಯದಿ (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ಪ್ರತ್ಯಾಗಮ 1;
    }
  } ಯಾವತ್ (continuous);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ continuous = 0;
  ಪೂರ್ಣಾಙ್ಕ quick = 0;
  ವರ್ಣ *justone = 0;

  ಯದಿ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } ಅನ್ಯಥಾ ಯದಿ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ಅನ್ಯಥಾ ಯದಿ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ಅನ್ಯಥಾ ಯದಿ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ಅನ್ಯಥಾ ಯದಿ (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  ಯದಿ (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
