#شامل "kernel/param.h"
#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"
#شامل "kernel/syscall.h"
#شامل "kernel/memlayout.h"
#شامل "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

نویسه buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
تهی
copyin(نویسه *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  برای (صحیح ai = 0; ai < اندازه(addrs) / اندازه(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open("copyin1", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    صحیح n = اكتب(fd, (تهی *)addr, 8192);
    اگر (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (تهی *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = اكتب(1, (نویسه *)addr, 8192);
    اگر (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (تهی *)addr, n);
      exit(1);
    }

    صحیح fds[2];
    اگر (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = اكتب(fds[1], (نویسه *)addr, 8192);
    اگر (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (تهی *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
تهی
copyout(نویسه *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  برای (صحیح ai = 0; ai < اندازه(addrs) / اندازه(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open("README", 0);
    اگر (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    صحیح n = اقرأ(fd, (تهی *)addr, 8192);
    اگر (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (تهی *)addr, n);
      exit(1);
    }
    close(fd);

    صحیح fds[2];
    اگر (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = اكتب(fds[1], "x", 1);
    اگر (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = اقرأ(fds[0], (تهی *)addr, 8192);
    اگر (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (تهی *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
تهی
copyinstr1(نویسه *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  برای (صحیح ai = 0; ai < اندازه(addrs) / اندازه(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open((نویسه *)addr, O_CREATE | O_WRONLY);
    اگر (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (تهی *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
تهی
copyinstr2(نویسه *s)
{
  نویسه b[MAXPATH + 1];

  برای (صحیح i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  صحیح ret = unlink(b);
  اگر (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  صحیح fd = open(b, O_CREATE | O_WRONLY);
  اگر (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  اگر (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  نویسه *args[] = {"xx", 0};
  ret = exec(b, args);
  اگر (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  صحیح pid = fork();
  اگر (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  اگر (pid == 0) {
    ایستا نویسه big[PGSIZE + 1];
    برای (صحیح i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    نویسه *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    اگر (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  صحیح st = 0;
  wait(&st);
  اگر (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
تهی
copyinstr3(نویسه *s)
{
  sbrk(8192);
  uint64 بالاترین = (uint64)sbrk(0);
  اگر ((بالاترین % PGSIZE) != 0) {
    sbrk(PGSIZE - (بالاترین % PGSIZE));
  }
  بالاترین = (uint64)sbrk(0);
  اگر (بالاترین % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  نویسه *b = (نویسه *)(بالاترین - 1);
  *b = 'x';

  صحیح ret = unlink(b);
  اگر (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  صحیح fd = open(b, O_CREATE | O_WRONLY);
  اگر (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  اگر (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  نویسه *args[] = {"xx", 0};
  ret = exec(b, args);
  اگر (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
تهی
rwsbrk(نویسه *s)
{
  صحیح fd, n;

  uint64 a = (uint64)sbrk(8192);

  اگر (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  اگر (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  اگر (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = اكتب(fd, (تهی *)(a + PGSIZE), 1024);
  اگر (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (تهی *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  اگر (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = اقرأ(fd, (تهی *)(a + PGSIZE), 10);
  اگر (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (تهی *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
تهی
truncate1(نویسه *s)
{
  نویسه buf[32];

  unlink("truncfile");
  صحیح fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  اكتب(fd1, "abcd", 4);
  close(fd1);

  صحیح fd2 = open("truncfile", O_RDONLY);
  صحیح n = اقرأ(fd2, buf, اندازه(buf));
  اگر (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  صحیح fd3 = open("truncfile", O_RDONLY);
  n = اقرأ(fd3, buf, اندازه(buf));
  اگر (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = اقرأ(fd2, buf, اندازه(buf));
  اگر (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  اكتب(fd1, "abcdef", 6);

  n = اقرأ(fd3, buf, اندازه(buf));
  اگر (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = اقرأ(fd2, buf, اندازه(buf));
  اگر (n != 2) {
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
تهی
truncate2(نویسه *s)
{
  unlink("truncfile");

  صحیح fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  اكتب(fd1, "abcd", 4);

  صحیح fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  صحیح n = اكتب(fd1, "x", 1);
  اگر (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

تهی
truncate3(نویسه *s)
{
  صحیح pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  اگر (pid == 0) {
    برای (صحیح i = 0; i < 100; i++) {
      نویسه buf[32];
      صحیح fd = open("truncfile", O_WRONLY);
      اگر (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      صحیح n = اكتب(fd, "1234567890", 10);
      اگر (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      اقرأ(fd, buf, اندازه(buf));
      close(fd);
    }
    exit(0);
  }

  برای (صحیح i = 0; i < 150; i++) {
    صحیح fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    اگر (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    صحیح n = اكتب(fd, "xxx", 3);
    اگر (n != 3) {
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
تهی
iputtest(نویسه *s)
{
  اگر (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  اگر (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  اگر (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  اگر (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
تهی
exitiputtest(نویسه *s)
{
  صحیح pid, xstatus;

  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid == 0) {
    اگر (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    اگر (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    اگر (unlink("../iputdir") < 0) {
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
تهی
openiputtest(نویسه *s)
{
  صحیح pid, xstatus;

  اگر (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid == 0) {
    صحیح fd = open("oidir", O_RDWR);
    اگر (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  اگر (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

تهی
opentest(نویسه *s)
{
  صحیح fd;

  fd = open("echo", 0);
  اگر (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  اگر (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

تهی
writetest(نویسه *s)
{
  صحیح fd;
  صحیح i;
  شمارش { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  برای (i = 0; i < N; i++) {
    اگر (اكتب(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    اگر (اكتب(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = اقرأ(fd, buf, N * SZ * 2);
  اگر (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  اگر (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

تهی
writebig(نویسه *s)
{
  صحیح i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  برای (i = 0; i < MAXFILE; i++) {
    ((صحیح *)buf)[0] = i;
    اگر (اكتب(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  برای (;;) {
    i = اقرأ(fd, buf, BSIZE);
    اگر (i == 0) {
      اگر (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      بشکن;
    } وگرنه اگر (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    اگر (((صحیح *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((صحیح *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  اگر (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
تهی
createtest(نویسه *s)
{
  صحیح i, fd;
  شمارش { N = 52 };

  نویسه name[3];
  name[0] = 'a';
  name[2] = '\0';
  برای (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  برای (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

تهی
dirtest(نویسه *s)
{
  اگر (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  اگر (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  اگر (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  اگر (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

تهی
exectest(نویسه *s)
{
  صحیح fd, xstatus, pid;
  نویسه *echoargv[] = {"echo", "OK", 0};
  نویسه buf[3];

  unlink("echo-ok");
  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid == 0) {
    صحیح errfd = dup(1);
    اگر (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    اگر (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    اگر (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  اگر (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  اگر (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  اگر (اقرأ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  اگر (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  وگرنه {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

تهی
pipe1(نویسه *s)
{
  صحیح fds[2], pid, xstatus;
  صحیح seq, i, n, cc, total;
  شمارش { N = 5, SZ = 1033 };

  اگر (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  اگر (pid == 0) {
    close(fds[0]);
    برای (n = 0; n < N; n++) {
      برای (i = 0; i < SZ; i++)
        buf[i] = seq++;
      اگر (اكتب(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } وگرنه اگر (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    وقتی ((n = اقرأ(fds[0], buf, cc)) > 0) {
      برای (i = 0; i < n; i++) {
        اگر ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          بازگشت;
        }
      }
      total += n;
      cc = cc * 2;
      اگر (cc > اندازه(buf))
        cc = اندازه(buf);
    }
    اگر (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } وگرنه {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
تهی
killstatus(نویسه *s)
{
  صحیح xst;

  برای (صحیح i = 0; i < 100; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid1 == 0) {
      وقتی (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    اگر (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
تهی
killzero(نویسه *s)
{
  صحیح pid, xst;

  kill(0);

  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid == 0) {
    exit(7);
  }
  اگر (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  اگر (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
تهی
preempt(نویسه *s)
{
  صحیح pid1, pid2, pid3;
  صحیح pfds[2];

  pid1 = fork();
  اگر (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  اگر (pid1 == 0)
    برای (;;)
      ;

  pid2 = fork();
  اگر (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid2 == 0)
    برای (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  اگر (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid3 == 0) {
    close(pfds[0]);
    اگر (اكتب(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    برای (;;)
      ;
  }

  close(pfds[1]);
  اگر (اقرأ(pfds[0], buf, اندازه(buf)) != 1) {
    printf("%s: preempt read error", s);
    بازگشت;
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
تهی
exitwait(نویسه *s)
{
  صحیح i, pid;

  برای (i = 0; i < 100; i++) {
    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid) {
      صحیح xstate;
      اگر (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      اگر (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } وگرنه {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
تهی
reparent(نویسه *s)
{
  صحیح master_pid = getpid();
  برای (صحیح i = 0; i < 200; i++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid) {
      اگر (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } وگرنه {
      صحیح pid2 = fork();
      اگر (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
تهی
twochildren(نویسه *s)
{
  برای (صحیح i = 0; i < 1000; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid1 == 0) {
      exit(0);
    } وگرنه {
      صحیح pid2 = fork();
      اگر (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      اگر (pid2 == 0) {
        exit(0);
      } وگرنه {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
تهی
forkfork(نویسه *s)
{
  شمارش { N = 2 };

  برای (صحیح i = 0; i < N; i++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    اگر (pid == 0) {
      برای (صحیح j = 0; j < 200; j++) {
        صحیح pid1 = fork();
        اگر (pid1 < 0) {
          exit(1);
        }
        اگر (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  صحیح xstatus;
  برای (صحیح i = 0; i < N; i++) {
    wait(&xstatus);
    اگر (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

تهی
forkforkfork(نویسه *s)
{
  unlink("stopforking");

  صحیح pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  اگر (pid == 0) {
    وقتی (1) {
      صحیح fd = open("stopforking", 0);
      اگر (fd >= 0) {
        exit(0);
      }
      اگر (fork() < 0) {
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
تهی
reparent2(نویسه *s)
{
  برای (صحیح i = 0; i < 800; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    اگر (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
تهی
mem(نویسه *s)
{
  تهی *m1, *m2;
  صحیح pid;

  اگر ((pid = fork()) == 0) {
    m1 = 0;
    وقتی ((m2 = malloc(10001)) != 0) {
      *(نویسه **)m2 = m1;
      m1 = m2;
    }
    وقتی (m1) {
      m2 = *(نویسه **)m1;
      آزاد(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    اگر (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    آزاد(m1);
    exit(0);
  } وگرنه {
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus == -1) {
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
تهی
sharedfd(نویسه *s)
{
  صحیح fd, pid, i, n, nc, np;
  شمارش { N = 1000, SZ = 10 };
  نویسه buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', اندازه(buf));
  برای (i = 0; i < N; i++) {
    اگر (اكتب(fd, buf, اندازه(buf)) != اندازه(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  اگر (pid == 0) {
    exit(0);
  } وگرنه {
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  اگر (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  وقتی ((n = اقرأ(fd, buf, اندازه(buf))) > 0) {
    برای (i = 0; i < اندازه(buf); i++) {
      اگر (buf[i] == 'c')
        nc++;
      اگر (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  اگر (nc == N * SZ && np == N * SZ) {
    exit(0);
  } وگرنه {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
تهی
fourfiles(نویسه *s)
{
  صحیح fd, pid, i, j, n, total, pi;
  نویسه *names[] = {"f0", "f1", "f2", "f3"};
  نویسه *fname;
  شمارش { N = 12, NCHILD = 4, SZ = 500 };

  برای (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    اگر (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      اگر (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      برای (i = 0; i < N; i++) {
        اگر ((n = اكتب(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  صحیح xstatus;
  برای (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      exit(xstatus);
  }

  برای (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    وقتی ((n = اقرأ(fd, buf, اندازه(buf))) > 0) {
      برای (j = 0; j < n; j++) {
        اگر (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    اگر (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
تهی
createdelete(نویسه *s)
{
  شمارش { N = 20, NCHILD = 4 };
  صحیح pid, i, fd, pi;
  نویسه name[32];

  برای (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    اگر (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      برای (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        اگر (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        اگر (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          اگر (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  صحیح xstatus;
  برای (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  برای (i = 0; i < N; i++) {
    برای (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      اگر ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } وگرنه اگر ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      اگر (fd >= 0)
        close(fd);
    }
  }

  برای (i = 0; i < N; i++) {
    برای (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
تهی
unlinkread(نویسه *s)
{
  شمارش { SZ = 5 };
  صحیح fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  اكتب(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  اگر (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  اگر (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  اكتب(fd1, "yyy", 3);
  close(fd1);

  اگر (اقرأ(fd, buf, اندازه(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  اگر (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  اگر (اكتب(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

تهی
linktest(نویسه *s)
{
  شمارش { SZ = 5 };
  صحیح fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  اگر (اكتب(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  اگر (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  اگر (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  اگر (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  اگر (اقرأ(fd, buf, اندازه(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  اگر (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  اگر (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  اگر (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
تهی
concreate(نویسه *s)
{
  شمارش { N = 40 };
  نویسه file[3];
  صحیح i, pid, n, fd;
  نویسه fa[N];
  ساختار {
    ushort inum;
    نویسه name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  برای (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    اگر (pid && (i % 3) == 1) {
      link("C0", file);
    } وگرنه اگر (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } وگرنه {
      fd = open(file, O_CREATE | O_RDWR);
      اگر (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    اگر (pid == 0) {
      exit(0);
    } وگرنه {
      صحیح xstatus;
      wait(&xstatus);
      اگر (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, اندازه(fa));
  fd = open(".", 0);
  n = 0;
  وقتی (اقرأ(fd, &de, اندازه(de)) > 0) {
    اگر (de.inum == 0)
      ادامه;
    اگر (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      اگر (i < 0 || i >= اندازه(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      اگر (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  اگر (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  برای (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } وگرنه {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    اگر (pid == 0)
      exit(0);
    وگرنه
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
تهی
linkunlink(نویسه *s)
{
  صحیح pid, i;

  unlink("x");
  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned صحیح x = (pid ? 1 : 97);
  برای (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    اگر ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } وگرنه اگر ((x % 3) == 1) {
      link("cat", "x");
    } وگرنه {
      unlink("x");
    }
  }

  اگر (pid)
    wait(0);
  وگرنه
    exit(0);
}

تهی
subdir(نویسه *s)
{
  صحیح fd, cc;

  unlink("ff");
  اگر (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  اكتب(fd, "ff", 2);
  close(fd);

  اگر (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  اگر (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  اكتب(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  اگر (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = اقرأ(fd, buf, اندازه(buf));
  اگر (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  اگر (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  اگر (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  اگر (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  اگر (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  اگر (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  اگر (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  اگر (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  اگر (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  اگر (اقرأ(fd, buf, اندازه(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  اگر (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  اگر (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  اگر (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  اگر (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  اگر (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  اگر (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  اگر (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  اگر (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  اگر (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  اگر (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  اگر (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  اگر (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  اگر (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  اگر (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  اگر (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  اگر (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  اگر (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  اگر (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  اگر (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  اگر (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  اگر (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
تهی
bigwrite(نویسه *s)
{
  صحیح fd, sz;

  unlink("bigwrite");
  برای (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    اگر (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    صحیح i;
    برای (i = 0; i < 2; i++) {
      صحیح cc = اكتب(fd, buf, sz);
      اگر (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

تهی
bigfile(نویسه *s)
{
  شمارش { N = 20, SZ = 600 };
  صحیح fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  برای (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    اگر (اكتب(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  اگر (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  برای (i = 0;; i++) {
    cc = اقرأ(fd, buf, SZ / 2);
    اگر (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    اگر (cc == 0)
      بشکن;
    اگر (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    اگر (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  اگر (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

تهی
fourteen(نویسه *s)
{
  صحیح fd;

  // DIRSIZ is 14.

  اگر (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  اگر (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  اگر (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  اگر (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  اگر (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  اگر (mkdir("123456789012345/12345678901234") == 0) {
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

تهی
rmdot(نویسه *s)
{
  اگر (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  اگر (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  اگر (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  اگر (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  اگر (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  اگر (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  اگر (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  اگر (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

تهی
dirfile(نویسه *s)
{
  صحیح fd;

  fd = open("dirfile", O_CREATE);
  اگر (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  اگر (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  اگر (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  اگر (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  اگر (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  اگر (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  اگر (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  اگر (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  اگر (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  اگر (اكتب(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
تهی
iref(نویسه *s)
{
  صحیح i, fd;

  برای (i = 0; i < NINODE + 1; i++) {
    اگر (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    اگر (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    اگر (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    اگر (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  برای (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
تهی
forktest(نویسه *s)
{
  شمارش { N = 1000 };
  صحیح n, pid;

  برای (n = 0; n < N; n++) {
    pid = fork();
    اگر (pid < 0)
      بشکن;
    اگر (pid == 0)
      exit(0);
  }

  اگر (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  اگر (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  برای (; n > 0; n--) {
    اگر (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  اگر (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

تهی
sbrkbasic(نویسه *s)
{
  شمارش { TOOMUCH = 1024 * 1024 * 1024 };
  صحیح i, pid, xstatus;
  نویسه *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  اگر (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  اگر (pid == 0) {
    a = sbrk(TOOMUCH);
    اگر (a == (نویسه *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    برای (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  اگر (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  برای (i = 0; i < 5000; i++) {
    b = sbrk(1);
    اگر (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  اگر (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  اگر (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  اگر (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

تهی
sbrkmuch(نویسه *s)
{
  شمارش { BIG = 100 * 1024 * 1024 };
  نویسه *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  اگر (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (نویسه *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  اگر (c == (نویسه *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  اگر (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  اگر (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  اگر (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  اگر (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
تهی
kernmem(نویسه *s)
{
  نویسه *a;
  صحیح pid;

  برای (a = (نویسه *)(KERNBASE); a < (نویسه *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
تهی
MAXVAplus(نویسه *s)
{
  فرار uint64 a = MAXVA;
  برای (; a != 0; a <<= 1) {
    صحیح pid;
    pid = fork();
    اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    اگر (pid == 0) {
      *(نویسه *)a = 99;
      printf("%s: oops wrote %p\n", s, (تهی *)a);
      exit(1);
    }
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
تهی
sbrkfail(نویسه *s)
{
  شمارش { BIG = 100 * 1024 * 1024 };
  صحیح i, xstatus;
  صحیح fds[2];
  نویسه scratch;
  نویسه *c, *a;
  صحیح pids[10];
  صحیح pid;
  صحیح failed;

  failed = 0;
  اگر (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  برای (i = 0; i < اندازه(pids) / اندازه(pids[0]); i++) {
    اگر ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      اگر (sbrk(BIG - (uint64)sbrk(0)) == (نویسه *)SBRK_ERROR)
        اكتب(fds[1], "0", 1);
      وگرنه
        اكتب(fds[1], "1", 1);
      // sit around until killed
      برای (;;)
        pause(1000);
    }
    اگر (pids[i] != -1) {
      اقرأ(fds[0], &scratch, 1);
      اگر (scratch == '0')
        failed = 1;
    }
  }
  اگر (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  برای (i = 0; i < اندازه(pids) / اندازه(pids[0]); i++) {
    اگر (pids[i] == -1)
      ادامه;
    kill(pids[i]);
    wait(0);
  }
  اگر (c == (نویسه *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  اگر (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    اگر (a == (نویسه *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  اگر (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
تهی
sbrkarg(نویسه *s)
{
  نویسه *a;
  صحیح fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  اگر (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  اگر ((n = اكتب(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  اگر (pipe((صحیح *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

تهی
validatetest(نویسه *s)
{
  صحیح hi;
  uint64 p;

  hi = 1100 * 1024;
  برای (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    اگر (link("nosuchfile", (نویسه *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
نویسه uninit[10000];
تهی
bsstest(نویسه *s)
{
  صحیح i;

  برای (i = 0; i < اندازه(uninit); i++) {
    اگر (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
تهی
bigargtest(نویسه *s)
{
  صحیح pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  اگر (pid == 0) {
    ایستا نویسه *args[MAXARG];
    صحیح i;
    نویسه big[400];
    memset(big, ' ', اندازه(big));
    big[اندازه(big) - 1] = '\0';
    برای (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } وگرنه اگر (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  اگر (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  اگر (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
تهی
fsfull()
{
  صحیح nfiles;
  صحیح fsblocks = 0;

  printf("fsfull test\n");

  برای (nfiles = 0;; nfiles++) {
    نویسه name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    صحیح fd = open(name, O_CREATE | O_RDWR);
    اگر (fd < 0) {
      printf("open %s failed\n", name);
      بشکن;
    }
    صحیح total = 0;
    وقتی (1) {
      صحیح cc = اكتب(fd, buf, BSIZE);
      اگر (cc < BSIZE)
        بشکن;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    اگر (total == 0)
      بشکن;
  }

  وقتی (nfiles >= 0) {
    نویسه name[64];
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

تهی
argptest(نویسه *s)
{
  صحیح fd;
  fd = open("init", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  اقرأ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
تهی
stacktest(نویسه *s)
{
  صحیح pid;
  صحیح xstatus;

  pid = fork();
  اگر (pid == 0) {
    نویسه *sp = (نویسه *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } وگرنه اگر (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  اگر (xstatus == -1) // kernel killed child?
    exit(0);
  وگرنه
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
تهی
nowrite(نویسه *s)
{
  صحیح pid;
  صحیح xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  برای (صحیح ai = 0; ai < اندازه(addrs) / اندازه(addrs[0]); ai++) {
    pid = fork();
    اگر (pid == 0) {
      فرار صحیح *addr = (صحیح *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } وگرنه اگر (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    اگر (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
تهی *big = (تهی *)0xeaeb0b5b00002f5e;
تهی
pgbug(نویسه *s)
{
  نویسه *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
تهی
sbrkbugs(نویسه *s)
{
  صحیح pid = fork();
  اگر (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  اگر (pid == 0) {
    صحیح sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  اگر (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  اگر (pid == 0) {
    صحیح sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  اگر (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  اگر (pid == 0) {
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
تهی
sbrklast(نویسه *s)
{
  uint64 بالاترین = (uint64)sbrk(0);
  اگر ((بالاترین % PGSIZE) != 0)
    sbrk(PGSIZE - (بالاترین % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  بالاترین = (uint64)sbrk(0);
  نویسه *p = (نویسه *)(بالاترین - 64);
  p[0] = 'x';
  p[1] = '\0';
  صحیح fd = open(p, O_RDWR | O_CREATE);
  اكتب(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  اقرأ(fd, p, 1);
  اگر (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
تهی
sbrk8000(نویسه *s)
{
  sbrk(0x80000004);
  فرار نویسه *بالاترین = sbrk(0);
  *(بالاترین - 1) = *(بالاترین - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
تهی
badarg(نویسه *s)
{
  برای (صحیح i = 0; i < 50000; i++) {
    نویسه *argv[2];
    argv[0] = (نویسه *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
تهی
lazy_alloc(نویسه *s)
{
  نویسه *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  اگر (prev_end == (نویسه *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  برای (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(نویسه **)i = i;

  برای (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    اگر (*(نویسه **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
تهی
lazy_unmap(نویسه *s)
{
  صحیح pid;
  نویسه *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  اگر (prev_end == (نویسه *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  برای (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(نویسه **)i = i;

  برای (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    اگر (pid < 0) {
      printf("error forking\n");
      exit(1);
    } وگرنه اگر (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(نویسه **)i = i;
      exit(0);
    } وگرنه {
      صحیح status;
      wait(&status);
      اگر (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

تهی
lazy_copy(نویسه *s)
{
  // copyinstr on lazy page
  {
    نویسه *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    تهی *xx = sbrk(0);
    تهی *ret = sbrk(-(((uint64)xx) + 1));
    اگر (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  برای (صحیح i = 0; i < اندازه(bad) / اندازه(bad[0]); i++) {
    صحیح fd = open("README", 0);
    اگر (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    اگر (اقرأ(fd, (نویسه *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    اگر (اكتب(fd, (نویسه *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

تهی
lazy_copyinstr(نویسه *s)
{
  نویسه *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  اگر ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  صحیح fd = open(&p[4095], O_RDONLY);
  اگر (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  ساختار stat st;
  صحیح r = fstat(fd, &st);
  اگر (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  اگر (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

تهی
lazy_sbrk(نویسه *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  نویسه *p = sbrk(0);
  وقتی ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    اگر (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  صحیح n = TRAPFRAME - PGSIZE - (uint64)p;

  نویسه *p1 = sbrklazy(n);
  اگر (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  اگر (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  اگر (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  اگر ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  اگر ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

تهی
partial_write(نویسه *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  صحیح fd = open("testfile", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  صحیح cc = اكتب(fd, "A", 1);
  اگر (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  اگر (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  نویسه *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  اگر ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = اكتب(fd, p - 1, 2);
  اگر (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  نویسه b;
  cc = اقرأ(fd, &b, 1);
  اگر (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  اگر (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  برای (صحیح i = 0; i < 64; i++) {
    نویسه buf[1024];
    memset(buf, 0, اندازه(buf));
    cc = اكتب(fd, buf, اندازه(buf));
    اگر (cc != اندازه(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  اگر (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = اقرأ(fd, &b, 1);
  اگر (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  اگر (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

تهی
unlinkcwd(نویسه *s)
{
  اگر (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  اگر (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  اگر (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  اگر (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  اگر (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  اگر (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  اگر (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

ساختار test {
  تهی (*f)(نویسه *);
  نویسه *s;
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
تهی
bigdir(نویسه *s)
{
  شمارش { N = 500 };
  صحیح i, fd;
  نویسه name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  اگر (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  برای (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    اگر (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  برای (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    اگر (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
تهی
manywrites(نویسه *s)
{
  صحیح nchildren = 4;
  صحیح howmany = 30; // increase to look for deadlock

  برای (صحیح ci = 0; ci < nchildren; ci++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    اگر (pid == 0) {
      نویسه name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      برای (صحیح iters = 0; iters < howmany; iters++) {
        برای (صحیح i = 0; i < ci + 1; i++) {
          صحیح fd = open(name, O_CREATE | O_RDWR);
          اگر (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          صحیح sz = اندازه(buf);
          صحیح cc = اكتب(fd, buf, sz);
          اگر (cc != sz) {
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

  برای (صحیح ci = 0; ci < nchildren; ci++) {
    صحیح st = 0;
    wait(&st);
    اگر (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
تهی
badwrite(نویسه *s)
{
  صحیح assumed_free = 600;

  unlink("junk");
  برای (صحیح i = 0; i < assumed_free; i++) {
    صحیح fd = open("junk", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    اكتب(fd, (نویسه *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  صحیح fd = open("junk", O_CREATE | O_WRONLY);
  اگر (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  اگر (اكتب(fd, "x", 1) != 1) {
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
تهی
execout(نویسه *s)
{
  برای (صحیح avail = 0; avail < 15; avail++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } وگرنه اگر (pid == 0) {
      // allocate all of memory.
      وقتی (1) {
        نویسه *a = sbrk(PGSIZE);
        اگر (a == SBRK_ERROR)
          بشکن;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      برای (صحیح i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      نویسه *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } وگرنه {
      wait((صحیح *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
تهی
diskfull(نویسه *s)
{
  صحیح fi;
  صحیح done = 0;

  unlink("diskfulldir");

  برای (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    نویسه name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      بشکن;
    }
    برای (صحیح i = 0; i < MAXFILE; i++) {
      نویسه buf[BSIZE];
      اگر (اكتب(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        بشکن;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  صحیح nzz = 128;
  برای (صحیح i = 0; i < nzz; i++) {
    نویسه name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0)
      بشکن;
    close(fd);
  }

  // this mkdir() is expected to fail.
  اگر (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  برای (صحیح i = 0; i < nzz; i++) {
    نویسه name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  برای (صحیح i = 0; '0' + i < 0177; i++) {
    نویسه name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

تهی
outofinodes(نویسه *s)
{
  صحیح nzz = 32 * 32;
  برای (صحیح i = 0; i < nzz; i++) {
    نویسه name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      // failure is eventually expected.
      بشکن;
    }
    close(fd);
  }

  برای (صحیح i = 0; i < nzz; i++) {
    نویسه name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

تهی
linkoverflow(نویسه *s)
{
  شمارش { TARGET = 32768 };
  شمارش { DIRS = 64 };
  ساختار stat st;
  صحیح i;

  unlink("/lof");
  صحیح fd = open("/lof", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  برای (i = 0; i < TARGET; i++) {
    صحیح d = i % DIRS;
    صحیح f = i / DIRS;

    نویسه pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    اگر (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    اگر (link("/lof", pn) < 0) {
      اگر (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      اگر (st.nlink >= 32767) {
        // overflow check succeeded.
        بشکن;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    اگر (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  اگر (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  اگر (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

ساختار test slowtests[] = {
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
صحیح
run(تهی f(نویسه *), نویسه *s)
{
  صحیح pid;
  صحیح xstatus;

  printf("test %s: ", s);
  اگر ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  اگر (pid == 0) {
    f(s);
    exit(0);
  } وگرنه {
    wait(&xstatus);
    اگر (xstatus != 0)
      printf("FAILED\n");
    وگرنه
      printf("OK\n");
    بازگشت xstatus == 0;
  }
}

صحیح
runtests(ساختار test *tests, نویسه *justone, صحیح continuous)
{
  صحیح ntests = 0;
  برای (ساختار test *t = tests; t->s != 0; t++) {
    اگر ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      اگر (!run(t->f, t->s)) {
        اگر (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          بازگشت -1;
        }
      }
    }
  }
  بازگشت ntests;
}

// use sbrk() to count how many free physical memory pages there are.
صحیح
countfree()
{
  صحیح n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  وقتی (1) {
    نویسه *a = sbrk(PGSIZE);
    اگر (a == SBRK_ERROR) {
      بشکن;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  بازگشت n;
}

صحیح
drivetests(صحیح quick, صحیح continuous, نویسه *justone)
{
  انجامبده {
    printf("usertests starting\n");
    صحیح free0 = countfree();
    صحیح free1 = 0;
    صحیح ntests = 0;
    صحیح n;
    n = runtests(quicktests, justone, continuous);
    اگر (n < 0) {
      اگر (continuous != 2) {
        بازگشت 1;
      }
    } وگرنه {
      ntests += n;
    }
    اگر (!quick) {
      اگر (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      اگر (n < 0) {
        اگر (continuous != 2) {
          بازگشت 1;
        }
      } وگرنه {
        ntests += n;
      }
    }
    اگر ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      اگر (continuous != 2) {
        بازگشت 1;
      }
    }
    اگر (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      بازگشت 1;
    }
  } وقتی (continuous);
  بازگشت 0;
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح continuous = 0;
  صحیح quick = 0;
  نویسه *justone = 0;

  اگر (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } وگرنه اگر (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } وگرنه اگر (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } وگرنه اگر (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } وگرنه اگر (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  اگر (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
