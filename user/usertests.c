#ضمن "kernel/param.h"
#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"
#ضمن "kernel/syscall.h"
#ضمن "kernel/memlayout.h"
#ضمن "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#تعریف_کړه BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

حرف buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
فراغ
copyin(حرف *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  لكل (صحيح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحيح fd = open("copyin1", O_CREATE | O_WRONLY);
    إذا (fd < 0) {
      چاپ_کړه("open(copyin1) failed\n");
      وتل(1);
    }
    صحيح n = اكتب(fd, (فراغ *)addr, 8192);
    إذا (n >= 0) {
      چاپ_کړه("write(fd, %p, 8192) returned %d, not -1\n", (فراغ *)addr, n);
      وتل(1);
    }
    close(fd);
    unlink("copyin1");

    n = اكتب(1, (حرف *)addr, 8192);
    إذا (n > 0) {
      چاپ_کړه("write(1, %p, 8192) returned %d, not -1 or 0\n", (فراغ *)addr, n);
      وتل(1);
    }

    صحيح fds[2];
    إذا (pipe(fds) < 0) {
      چاپ_کړه("pipe() failed\n");
      وتل(1);
    }
    n = اكتب(fds[1], (حرف *)addr, 8192);
    إذا (n > 0) {
      چاپ_کړه("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (فراغ *)addr,
             n);
      وتل(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
فراغ
copyout(حرف *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  لكل (صحيح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحيح fd = open("README", 0);
    إذا (fd < 0) {
      چاپ_کړه("open(README) failed\n");
      وتل(1);
    }
    صحيح n = اقرأ(fd, (فراغ *)addr, 8192);
    إذا (n > 0) {
      چاپ_کړه("read(fd, %p, 8192) returned %d, not -1 or 0\n", (فراغ *)addr, n);
      وتل(1);
    }
    close(fd);

    صحيح fds[2];
    إذا (pipe(fds) < 0) {
      چاپ_کړه("pipe() failed\n");
      وتل(1);
    }
    n = اكتب(fds[1], "x", 1);
    إذا (n != 1) {
      چاپ_کړه("pipe write failed\n");
      وتل(1);
    }
    n = اقرأ(fds[0], (فراغ *)addr, 8192);
    إذا (n > 0) {
      چاپ_کړه("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (فراغ *)addr,
             n);
      وتل(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
فراغ
copyinstr1(حرف *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  لكل (صحيح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحيح fd = open((حرف *)addr, O_CREATE | O_WRONLY);
    إذا (fd >= 0) {
      چاپ_کړه("open(%p) returned %d, not -1\n", (فراغ *)addr, fd);
      وتل(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
فراغ
copyinstr2(حرف *s)
{
  حرف b[MAXPATH + 1];

  لكل (صحيح i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  صحيح ret = unlink(b);
  إذا (ret != -1) {
    چاپ_کړه("unlink(%s) returned %d, not -1\n", b, ret);
    وتل(1);
  }

  صحيح fd = open(b, O_CREATE | O_WRONLY);
  إذا (fd != -1) {
    چاپ_کړه("open(%s) returned %d, not -1\n", b, fd);
    وتل(1);
  }

  ret = link(b, b);
  إذا (ret != -1) {
    چاپ_کړه("link(%s, %s) returned %d, not -1\n", b, b, ret);
    وتل(1);
  }

  حرف *دلیلونه[] = {"xx", 0};
  ret = exec(b, دلیلونه);
  إذا (ret != -1) {
    چاپ_کړه("exec(%s) returned %d, not -1\n", b, fd);
    وتل(1);
  }

  صحيح pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("fork failed\n");
    وتل(1);
  }
  إذا (pid == 0) {
    ساكن حرف big[PGSIZE + 1];
    لكل (صحيح i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    حرف *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    إذا (ret != -1) {
      چاپ_کړه("exec(echo, BIG) returned %d, not -1\n", fd);
      وتل(1);
    }
    وتل(747); // OK
  }

  صحيح st = 0;
  wait(&st);
  إذا (st != 747) {
    چاپ_کړه("exec(echo, BIG) succeeded, should have failed\n");
    وتل(1);
  }
}

// what if a string argument crosses over the end of last user page?
فراغ
copyinstr3(حرف *s)
{
  sbrk(8192);
  uint64 سر_2 = (uint64)sbrk(0);
  إذا ((سر_2 % PGSIZE) != 0) {
    sbrk(PGSIZE - (سر_2 % PGSIZE));
  }
  سر_2 = (uint64)sbrk(0);
  إذا (سر_2 % PGSIZE) {
    چاپ_کړه("oops\n");
    وتل(1);
  }

  حرف *b = (حرف *)(سر_2 - 1);
  *b = 'x';

  صحيح ret = unlink(b);
  إذا (ret != -1) {
    چاپ_کړه("unlink(%s) returned %d, not -1\n", b, ret);
    وتل(1);
  }

  صحيح fd = open(b, O_CREATE | O_WRONLY);
  إذا (fd != -1) {
    چاپ_کړه("open(%s) returned %d, not -1\n", b, fd);
    وتل(1);
  }

  ret = link(b, b);
  إذا (ret != -1) {
    چاپ_کړه("link(%s, %s) returned %d, not -1\n", b, b, ret);
    وتل(1);
  }

  حرف *دلیلونه[] = {"xx", 0};
  ret = exec(b, دلیلونه);
  إذا (ret != -1) {
    چاپ_کړه("exec(%s) returned %d, not -1\n", b, fd);
    وتل(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
فراغ
rwsbrk(حرف *s)
{
  صحيح fd, n;

  uint64 a = (uint64)sbrk(8192);

  إذا (a == (uint64)SBRK_ERROR) {
    چاپ_کړه("sbrk(rwsbrk) failed\n");
    وتل(1);
  }

  إذا (sbrk(-8192) == SBRK_ERROR) {
    چاپ_کړه("sbrk(rwsbrk) shrink failed\n");
    وتل(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  إذا (fd < 0) {
    چاپ_کړه("open(rwsbrk) failed\n");
    وتل(1);
  }
  n = اكتب(fd, (فراغ *)(a + PGSIZE), 1024);
  إذا (n >= 0) {
    چاپ_کړه("write(fd, %p, 1024) returned %d, not -1\n", (فراغ *)a + PGSIZE, n);
    وتل(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("open(README) failed\n");
    وتل(1);
  }
  n = اقرأ(fd, (فراغ *)(a + PGSIZE), 10);
  إذا (n >= 0) {
    چاپ_کړه("read(fd, %p, 10) returned %d, not -1\n", (فراغ *)a + PGSIZE, n);
    وتل(1);
  }
  close(fd);

  وتل(0);
}

// test O_TRUNC.
فراغ
truncate1(حرف *s)
{
  حرف buf[32];

  unlink("truncfile");
  صحيح fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  اكتب(fd1, "abcd", 4);
  close(fd1);

  صحيح fd2 = open("truncfile", O_RDONLY);
  صحيح n = اقرأ(fd2, buf, حجم(buf));
  إذا (n != 4) {
    چاپ_کړه("%s: read %d bytes, wanted 4\n", s, n);
    وتل(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  صحيح fd3 = open("truncfile", O_RDONLY);
  n = اقرأ(fd3, buf, حجم(buf));
  إذا (n != 0) {
    چاپ_کړه("aaa fd3=%d\n", fd3);
    چاپ_کړه("%s: read %d bytes, wanted 0\n", s, n);
    وتل(1);
  }

  n = اقرأ(fd2, buf, حجم(buf));
  إذا (n != 0) {
    چاپ_کړه("bbb fd2=%d\n", fd2);
    چاپ_کړه("%s: read %d bytes, wanted 0\n", s, n);
    وتل(1);
  }

  اكتب(fd1, "abcdef", 6);

  n = اقرأ(fd3, buf, حجم(buf));
  إذا (n != 6) {
    چاپ_کړه("%s: read %d bytes, wanted 6\n", s, n);
    وتل(1);
  }

  n = اقرأ(fd2, buf, حجم(buf));
  إذا (n != 2) {
    چاپ_کړه("%s: read %d bytes, wanted 2\n", s, n);
    وتل(1);
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
فراغ
truncate2(حرف *s)
{
  unlink("truncfile");

  صحيح fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  اكتب(fd1, "abcd", 4);

  صحيح fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  صحيح n = اكتب(fd1, "x", 1);
  إذا (n != -1) {
    چاپ_کړه("%s: write returned %d, expected -1\n", s, n);
    وتل(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

فراغ
truncate3(حرف *s)
{
  صحيح pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }

  إذا (pid == 0) {
    لكل (صحيح i = 0; i < 100; i++) {
      حرف buf[32];
      صحيح fd = open("truncfile", O_WRONLY);
      إذا (fd < 0) {
        چاپ_کړه("%s: open failed\n", s);
        وتل(1);
      }
      صحيح n = اكتب(fd, "1234567890", 10);
      إذا (n != 10) {
        چاپ_کړه("%s: write got %d, expected 10\n", s, n);
        وتل(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      اقرأ(fd, buf, حجم(buf));
      close(fd);
    }
    وتل(0);
  }

  لكل (صحيح i = 0; i < 150; i++) {
    صحيح fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    إذا (fd < 0) {
      چاپ_کړه("%s: open failed\n", s);
      وتل(1);
    }
    صحيح n = اكتب(fd, "xxx", 3);
    إذا (n != 3) {
      چاپ_کړه("%s: write got %d, expected 3\n", s, n);
      وتل(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  وتل(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
فراغ
iputtest(حرف *s)
{
  إذا (mkdir("iputdir") < 0) {
    چاپ_کړه("%s: mkdir failed\n", s);
    وتل(1);
  }
  إذا (chdir("iputdir") < 0) {
    چاپ_کړه("%s: chdir iputdir failed\n", s);
    وتل(1);
  }
  إذا (unlink("../iputdir") < 0) {
    چاپ_کړه("%s: unlink ../iputdir failed\n", s);
    وتل(1);
  }
  إذا (chdir("/") < 0) {
    چاپ_کړه("%s: chdir / failed\n", s);
    وتل(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
فراغ
exitiputtest(حرف *s)
{
  صحيح pid, xstatus;

  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid == 0) {
    إذا (mkdir("iputdir") < 0) {
      چاپ_کړه("%s: mkdir failed\n", s);
      وتل(1);
    }
    إذا (chdir("iputdir") < 0) {
      چاپ_کړه("%s: child chdir failed\n", s);
      وتل(1);
    }
    إذا (unlink("../iputdir") < 0) {
      چاپ_کړه("%s: unlink ../iputdir failed\n", s);
      وتل(1);
    }
    وتل(0);
  }
  wait(&xstatus);
  وتل(xstatus);
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
فراغ
openiputtest(حرف *s)
{
  صحيح pid, xstatus;

  إذا (mkdir("oidir") < 0) {
    چاپ_کړه("%s: mkdir oidir failed\n", s);
    وتل(1);
  }
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid == 0) {
    صحيح fd = open("oidir", O_RDWR);
    إذا (fd >= 0) {
      چاپ_کړه("%s: open directory for write succeeded\n", s);
      وتل(1);
    }
    وتل(0);
  }
  pause(1);
  إذا (unlink("oidir") != 0) {
    چاپ_کړه("%s: unlink failed\n", s);
    وتل(1);
  }
  wait(&xstatus);
  وتل(xstatus);
}

// simple file system tests

فراغ
opentest(حرف *s)
{
  صحيح fd;

  fd = open("echo", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: open echo failed!\n", s);
    وتل(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  إذا (fd >= 0) {
    چاپ_کړه("%s: open doesnotexist succeeded!\n", s);
    وتل(1);
  }
}

فراغ
writetest(حرف *s)
{
  صحيح fd;
  صحيح i;
  تعداد { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: error: creat small failed!\n", s);
    وتل(1);
  }
  لكل (i = 0; i < N; i++) {
    إذا (اكتب(fd, "aaaaaaaaaa", SZ) != SZ) {
      چاپ_کړه("%s: error: write aa %d new file failed\n", s, i);
      وتل(1);
    }
    إذا (اكتب(fd, "bbbbbbbbbb", SZ) != SZ) {
      چاپ_کړه("%s: error: write bb %d new file failed\n", s, i);
      وتل(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: error: open small failed!\n", s);
    وتل(1);
  }
  i = اقرأ(fd, buf, N * SZ * 2);
  إذا (i != N * SZ * 2) {
    چاپ_کړه("%s: read failed\n", s);
    وتل(1);
  }
  close(fd);

  إذا (unlink("small") < 0) {
    چاپ_کړه("%s: unlink small failed\n", s);
    وتل(1);
  }
}

فراغ
writebig(حرف *s)
{
  صحيح i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: error: creat big failed!\n", s);
    وتل(1);
  }

  لكل (i = 0; i < MAXFILE; i++) {
    ((صحيح *)buf)[0] = i;
    إذا (اكتب(fd, buf, BSIZE) != BSIZE) {
      چاپ_کړه("%s: error: write big file failed i=%d\n", s, i);
      وتل(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: error: open big failed!\n", s);
    وتل(1);
  }

  n = 0;
  لكل (;;) {
    i = اقرأ(fd, buf, BSIZE);
    إذا (i == 0) {
      إذا (n != MAXFILE) {
        چاپ_کړه("%s: read only %d blocks from big", s, n);
        وتل(1);
      }
      اكسر;
    } وإلا إذا (i != BSIZE) {
      چاپ_کړه("%s: read failed %d\n", s, i);
      وتل(1);
    }
    إذا (((صحيح *)buf)[0] != n) {
      چاپ_کړه("%s: read content of block %d is %d\n", s, n, ((صحيح *)buf)[0]);
      وتل(1);
    }
    n++;
  }
  close(fd);
  إذا (unlink("big") < 0) {
    چاپ_کړه("%s: unlink big failed\n", s);
    وتل(1);
  }
}

// many creates, followed by unlink test
فراغ
createtest(حرف *s)
{
  صحيح i, fd;
  تعداد { N = 52 };

  حرف name[3];
  name[0] = 'a';
  name[2] = '\0';
  لكل (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  لكل (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

فراغ
dirtest(حرف *s)
{
  إذا (mkdir("dir0") < 0) {
    چاپ_کړه("%s: mkdir failed\n", s);
    وتل(1);
  }

  إذا (chdir("dir0") < 0) {
    چاپ_کړه("%s: chdir dir0 failed\n", s);
    وتل(1);
  }

  إذا (chdir("..") < 0) {
    چاپ_کړه("%s: chdir .. failed\n", s);
    وتل(1);
  }

  إذا (unlink("dir0") < 0) {
    چاپ_کړه("%s: unlink dir0 failed\n", s);
    وتل(1);
  }
}

فراغ
exectest(حرف *s)
{
  صحيح fd, xstatus, pid;
  حرف *echoargv[] = {"echo", "OK", 0};
  حرف buf[3];

  unlink("echo-ok");
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid == 0) {
    صحيح errfd = dup(1);
    إذا (errfd < 0) {
      چاپ_کړه("%s: dup failed\n", s);
      وتل(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    إذا (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      وتل(1);
    }
    إذا (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      وتل(1);
    }
    إذا (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      وتل(1);
    }
    // won't get to here
  }
  إذا (wait(&xstatus) != pid) {
    چاپ_کړه("%s: wait failed!\n", s);
  }
  إذا (xstatus != 0) {
    چاپ_کړه("%s: nonzero wait status %d\n", s, xstatus);
    وتل(1);
  }

  fd = open("echo-ok", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: open failed\n", s);
    وتل(1);
  }
  إذا (اقرأ(fd, buf, 2) != 2) {
    چاپ_کړه("%s: read failed\n", s);
    وتل(1);
  }
  unlink("echo-ok");
  إذا (buf[0] == 'O' && buf[1] == 'K')
    وتل(0);
  وإلا {
    چاپ_کړه("%s: wrong output\n", s);
    وتل(1);
  }
}

// simple fork and pipe read/write

فراغ
pipe1(حرف *s)
{
  صحيح fds[2], pid, xstatus;
  صحيح seq, i, n, cc, ټول;
  تعداد { N = 5, SZ = 1033 };

  إذا (pipe(fds) != 0) {
    چاپ_کړه("%s: pipe() failed\n", s);
    وتل(1);
  }
  pid = fork();
  seq = 0;
  إذا (pid == 0) {
    close(fds[0]);
    لكل (n = 0; n < N; n++) {
      لكل (i = 0; i < SZ; i++)
        buf[i] = seq++;
      إذا (اكتب(fds[1], buf, SZ) != SZ) {
        چاپ_کړه("%s: pipe1 oops 1\n", s);
        وتل(1);
      }
    }
    وتل(0);
  } وإلا إذا (pid > 0) {
    close(fds[1]);
    ټول = 0;
    cc = 1;
    طالما ((n = اقرأ(fds[0], buf, cc)) > 0) {
      لكل (i = 0; i < n; i++) {
        إذا ((buf[i] & 0xff) != (seq++ & 0xff)) {
          چاپ_کړه("%s: pipe1 oops 2\n", s);
          أرجع;
        }
      }
      ټول += n;
      cc = cc * 2;
      إذا (cc > حجم(buf))
        cc = حجم(buf);
    }
    إذا (ټول != N * SZ) {
      چاپ_کړه("%s: pipe1 oops 3 total %d\n", s, ټول);
      وتل(1);
    }
    close(fds[0]);
    wait(&xstatus);
    وتل(xstatus);
  } وإلا {
    چاپ_کړه("%s: fork() failed\n", s);
    وتل(1);
  }
}

// test if child is killed (status = -1)
فراغ
killstatus(حرف *s)
{
  صحيح xst;

  لكل (صحيح i = 0; i < 100; i++) {
    صحيح pid1 = fork();
    إذا (pid1 < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid1 == 0) {
      طالما (1) {
        getpid();
      }
      وتل(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    إذا (xst != -1) {
      چاپ_کړه("%s: status should be -1\n", s);
      وتل(1);
    }
  }
  وتل(0);
}

// test that kill on unused process 0 doesn't set the killed flag
فراغ
killzero(حرف *s)
{
  صحيح pid, xst;

  kill(0);

  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid == 0) {
    وتل(7);
  }
  إذا (wait(&xst) != pid) {
    چاپ_کړه("%s: wait wrong pid\n", s);
    وتل(1);
  }
  إذا (xst != 7) {
    چاپ_کړه("%s: child exited with status %d, expected 7\n", s, xst);
    وتل(1);
  }
  وتل(0);
}

// meant to be run w/ at most two CPUs
فراغ
preempt(حرف *s)
{
  صحيح pid1, pid2, pid3;
  صحيح pfds[2];

  pid1 = fork();
  إذا (pid1 < 0) {
    چاپ_کړه("%s: fork failed", s);
    وتل(1);
  }
  إذا (pid1 == 0)
    لكل (;;)
      ;

  pid2 = fork();
  إذا (pid2 < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid2 == 0)
    لكل (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  إذا (pid3 < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid3 == 0) {
    close(pfds[0]);
    إذا (اكتب(pfds[1], "x", 1) != 1)
      چاپ_کړه("%s: preempt write error", s);
    close(pfds[1]);
    لكل (;;)
      ;
  }

  close(pfds[1]);
  إذا (اقرأ(pfds[0], buf, حجم(buf)) != 1) {
    چاپ_کړه("%s: preempt read error", s);
    أرجع;
  }
  close(pfds[0]);
  چاپ_کړه("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  چاپ_کړه("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
فراغ
exitwait(حرف *s)
{
  صحيح i, pid;

  لكل (i = 0; i < 100; i++) {
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid) {
      صحيح xstate;
      إذا (wait(&xstate) != pid) {
        چاپ_کړه("%s: wait wrong pid\n", s);
        وتل(1);
      }
      إذا (i != xstate) {
        چاپ_کړه("%s: wait wrong exit status\n", s);
        وتل(1);
      }
    } وإلا {
      وتل(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
فراغ
reparent(حرف *s)
{
  صحيح master_pid = getpid();
  لكل (صحيح i = 0; i < 200; i++) {
    صحيح pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid) {
      إذا (wait(0) != pid) {
        چاپ_کړه("%s: wait wrong pid\n", s);
        وتل(1);
      }
    } وإلا {
      صحيح pid2 = fork();
      إذا (pid2 < 0) {
        kill(master_pid);
        وتل(1);
      }
      وتل(0);
    }
  }
  وتل(0);
}

// what if two children exit() at the same time?
فراغ
twochildren(حرف *s)
{
  لكل (صحيح i = 0; i < 1000; i++) {
    صحيح pid1 = fork();
    إذا (pid1 < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid1 == 0) {
      وتل(0);
    } وإلا {
      صحيح pid2 = fork();
      إذا (pid2 < 0) {
        چاپ_کړه("%s: fork failed\n", s);
        وتل(1);
      }
      إذا (pid2 == 0) {
        وتل(0);
      } وإلا {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
فراغ
forkfork(حرف *s)
{
  تعداد { N = 2 };

  لكل (صحيح i = 0; i < N; i++) {
    صحيح pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed", s);
      وتل(1);
    }
    إذا (pid == 0) {
      لكل (صحيح j = 0; j < 200; j++) {
        صحيح pid1 = fork();
        إذا (pid1 < 0) {
          وتل(1);
        }
        إذا (pid1 == 0) {
          وتل(0);
        }
        wait(0);
      }
      وتل(0);
    }
  }

  صحيح xstatus;
  لكل (صحيح i = 0; i < N; i++) {
    wait(&xstatus);
    إذا (xstatus != 0) {
      چاپ_کړه("%s: fork in child failed", s);
      وتل(1);
    }
  }
}

فراغ
forkforkfork(حرف *s)
{
  unlink("stopforking");

  صحيح pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed", s);
    وتل(1);
  }
  إذا (pid == 0) {
    طالما (1) {
      صحيح fd = open("stopforking", 0);
      إذا (fd >= 0) {
        وتل(0);
      }
      إذا (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    وتل(0);
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
فراغ
reparent2(حرف *s)
{
  لكل (صحيح i = 0; i < 800; i++) {
    صحيح pid1 = fork();
    إذا (pid1 < 0) {
      چاپ_کړه("fork failed\n");
      وتل(1);
    }
    إذا (pid1 == 0) {
      fork();
      fork();
      وتل(0);
    }
    wait(0);
  }

  وتل(0);
}

// allocate all mem, free it, and allocate again
فراغ
mem(حرف *s)
{
  فراغ *m1, *m2;
  صحيح pid;

  إذا ((pid = fork()) == 0) {
    m1 = 0;
    طالما ((m2 = حافظه_ورکړه(10001)) != 0) {
      *(حرف **)m2 = m1;
      m1 = m2;
    }
    طالما (m1) {
      m2 = *(حرف **)m1;
      حرر(m1);
      m1 = m2;
    }
    m1 = حافظه_ورکړه(1024 * 20);
    إذا (m1 == 0) {
      چاپ_کړه("%s: couldn't allocate mem?!!\n", s);
      وتل(1);
    }
    حرر(m1);
    وتل(0);
  } وإلا {
    صحيح xstatus;
    wait(&xstatus);
    إذا (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      وتل(0);
    }
    وتل(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
فراغ
sharedfd(حرف *s)
{
  صحيح fd, pid, i, n, nc, np;
  تعداد { N = 1000, SZ = 10 };
  حرف buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot open sharedfd for writing", s);
    وتل(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', حجم(buf));
  لكل (i = 0; i < N; i++) {
    إذا (اكتب(fd, buf, حجم(buf)) != حجم(buf)) {
      چاپ_کړه("%s: write sharedfd failed\n", s);
      وتل(1);
    }
  }
  إذا (pid == 0) {
    وتل(0);
  } وإلا {
    صحيح xstatus;
    wait(&xstatus);
    إذا (xstatus != 0)
      وتل(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot open sharedfd for reading\n", s);
    وتل(1);
  }
  nc = np = 0;
  طالما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    لكل (i = 0; i < حجم(buf); i++) {
      إذا (buf[i] == 'c')
        nc++;
      إذا (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  إذا (nc == N * SZ && np == N * SZ) {
    وتل(0);
  } وإلا {
    چاپ_کړه("%s: nc/np test fails\n", s);
    وتل(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
فراغ
fourfiles(حرف *s)
{
  صحيح fd, pid, i, j, n, ټول, pi;
  حرف *نومونه[] = {"f0", "f1", "f2", "f3"};
  حرف *fname;
  تعداد { N = 12, NCHILD = 4, SZ = 500 };

  لكل (pi = 0; pi < NCHILD; pi++) {
    fname = نومونه[pi];
    unlink(fname);

    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }

    إذا (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      إذا (fd < 0) {
        چاپ_کړه("%s: create failed\n", s);
        وتل(1);
      }

      memset(buf, '0' + pi, SZ);
      لكل (i = 0; i < N; i++) {
        إذا ((n = اكتب(fd, buf, SZ)) != SZ) {
          چاپ_کړه("write failed %d\n", n);
          وتل(1);
        }
      }
      وتل(0);
    }
  }

  صحيح xstatus;
  لكل (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    إذا (xstatus != 0)
      وتل(xstatus);
  }

  لكل (i = 0; i < NCHILD; i++) {
    fname = نومونه[i];
    fd = open(fname, 0);
    ټول = 0;
    طالما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
      لكل (j = 0; j < n; j++) {
        إذا (buf[j] != '0' + i) {
          چاپ_کړه("%s: wrong char\n", s);
          وتل(1);
        }
      }
      ټول += n;
    }
    close(fd);
    إذا (ټول != N * SZ) {
      چاپ_کړه("wrong length %d\n", ټول);
      وتل(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
فراغ
createdelete(حرف *s)
{
  تعداد { N = 20, NCHILD = 4 };
  صحيح pid, i, fd, pi;
  حرف name[32];

  لكل (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }

    إذا (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      لكل (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        إذا (fd < 0) {
          چاپ_کړه("%s: create failed\n", s);
          وتل(1);
        }
        close(fd);
        إذا (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          إذا (unlink(name) < 0) {
            چاپ_کړه("%s: unlink failed\n", s);
            وتل(1);
          }
        }
      }
      وتل(0);
    }
  }

  صحيح xstatus;
  لكل (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    إذا (xstatus != 0)
      وتل(1);
  }

  name[0] = name[1] = name[2] = 0;
  لكل (i = 0; i < N; i++) {
    لكل (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      إذا ((i == 0 || i >= N / 2) && fd < 0) {
        چاپ_کړه("%s: oops createdelete %s didn't exist\n", s, name);
        وتل(1);
      } وإلا إذا ((i >= 1 && i < N / 2) && fd >= 0) {
        چاپ_کړه("%s: oops createdelete %s did exist\n", s, name);
        وتل(1);
      }
      إذا (fd >= 0)
        close(fd);
    }
  }

  لكل (i = 0; i < N; i++) {
    لكل (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
فراغ
unlinkread(حرف *s)
{
  تعداد { SZ = 5 };
  صحيح fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: create unlinkread failed\n", s);
    وتل(1);
  }
  اكتب(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: open unlinkread failed\n", s);
    وتل(1);
  }
  إذا (unlink("unlinkread") != 0) {
    چاپ_کړه("%s: unlink unlinkread failed\n", s);
    وتل(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  اكتب(fd1, "yyy", 3);
  close(fd1);

  إذا (اقرأ(fd, buf, حجم(buf)) != SZ) {
    چاپ_کړه("%s: unlinkread read failed", s);
    وتل(1);
  }
  إذا (buf[0] != 'h') {
    چاپ_کړه("%s: unlinkread wrong data\n", s);
    وتل(1);
  }
  إذا (اكتب(fd, buf, 10) != 10) {
    چاپ_کړه("%s: unlinkread write failed\n", s);
    وتل(1);
  }
  close(fd);
  unlink("unlinkread");
}

فراغ
linktest(حرف *s)
{
  تعداد { SZ = 5 };
  صحيح fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: create lf1 failed\n", s);
    وتل(1);
  }
  إذا (اكتب(fd, "hello", SZ) != SZ) {
    چاپ_کړه("%s: write lf1 failed\n", s);
    وتل(1);
  }
  close(fd);

  إذا (link("lf1", "lf2") < 0) {
    چاپ_کړه("%s: link lf1 lf2 failed\n", s);
    وتل(1);
  }
  unlink("lf1");

  إذا (open("lf1", 0) >= 0) {
    چاپ_کړه("%s: unlinked lf1 but it is still there!\n", s);
    وتل(1);
  }

  fd = open("lf2", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: open lf2 failed\n", s);
    وتل(1);
  }
  إذا (اقرأ(fd, buf, حجم(buf)) != SZ) {
    چاپ_کړه("%s: read lf2 failed\n", s);
    وتل(1);
  }
  close(fd);

  إذا (link("lf2", "lf2") >= 0) {
    چاپ_کړه("%s: link lf2 lf2 succeeded! oops\n", s);
    وتل(1);
  }

  unlink("lf2");
  إذا (link("lf2", "lf1") >= 0) {
    چاپ_کړه("%s: link non-existent succeeded! oops\n", s);
    وتل(1);
  }

  إذا (link(".", "lf1") >= 0) {
    چاپ_کړه("%s: link . lf1 succeeded! oops\n", s);
    وتل(1);
  }
}

// test concurrent create/link/unlink of the same file
فراغ
concreate(حرف *s)
{
  تعداد { N = 40 };
  حرف file[3];
  صحيح i, pid, n, fd;
  حرف fa[N];
  بنية {
    ushort inum;
    حرف name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  لكل (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    إذا (pid && (i % 3) == 1) {
      link("C0", file);
    } وإلا إذا (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } وإلا {
      fd = open(file, O_CREATE | O_RDWR);
      إذا (fd < 0) {
        چاپ_کړه("concreate create %s failed\n", file);
        وتل(1);
      }
      close(fd);
    }
    إذا (pid == 0) {
      وتل(0);
    } وإلا {
      صحيح xstatus;
      wait(&xstatus);
      إذا (xstatus != 0)
        وتل(1);
    }
  }

  memset(fa, 0, حجم(fa));
  fd = open(".", 0);
  n = 0;
  طالما (اقرأ(fd, &de, حجم(de)) > 0) {
    إذا (de.inum == 0)
      استمر;
    إذا (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      إذا (i < 0 || i >= حجم(fa)) {
        چاپ_کړه("%s: concreate weird file %s\n", s, de.name);
        وتل(1);
      }
      إذا (fa[i]) {
        چاپ_کړه("%s: concreate duplicate file %s\n", s, de.name);
        وتل(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  إذا (n != N) {
    چاپ_کړه("%s: concreate not enough files in directory listing\n", s);
    وتل(1);
  }

  لكل (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } وإلا {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    إذا (pid == 0)
      وتل(0);
    وإلا
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
فراغ
linkunlink(حرف *s)
{
  صحيح pid, i;

  unlink("x");
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }

  بینښه صحيح x = (pid ? 1 : 97);
  لكل (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    إذا ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } وإلا إذا ((x % 3) == 1) {
      link("cat", "x");
    } وإلا {
      unlink("x");
    }
  }

  إذا (pid)
    wait(0);
  وإلا
    وتل(0);
}

فراغ
subdir(حرف *s)
{
  صحيح fd, cc;

  unlink("ff");
  إذا (mkdir("dd") != 0) {
    چاپ_کړه("%s: mkdir dd failed\n", s);
    وتل(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: create dd/ff failed\n", s);
    وتل(1);
  }
  اكتب(fd, "ff", 2);
  close(fd);

  إذا (unlink("dd") >= 0) {
    چاپ_کړه("%s: unlink dd (non-empty dir) succeeded!\n", s);
    وتل(1);
  }

  إذا (mkdir("/dd/dd") != 0) {
    چاپ_کړه("%s: subdir mkdir dd/dd failed\n", s);
    وتل(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: create dd/dd/ff failed\n", s);
    وتل(1);
  }
  اكتب(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: open dd/dd/../ff failed\n", s);
    وتل(1);
  }
  cc = اقرأ(fd, buf, حجم(buf));
  إذا (cc != 2 || buf[0] != 'f') {
    چاپ_کړه("%s: dd/dd/../ff wrong content\n", s);
    وتل(1);
  }
  close(fd);

  إذا (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    چاپ_کړه("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    وتل(1);
  }

  إذا (unlink("dd/dd/ff") != 0) {
    چاپ_کړه("%s: unlink dd/dd/ff failed\n", s);
    وتل(1);
  }
  إذا (open("dd/dd/ff", O_RDONLY) >= 0) {
    چاپ_کړه("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    وتل(1);
  }

  إذا (chdir("dd") != 0) {
    چاپ_کړه("%s: chdir dd failed\n", s);
    وتل(1);
  }
  إذا (chdir("dd/../../dd") != 0) {
    چاپ_کړه("%s: chdir dd/../../dd failed\n", s);
    وتل(1);
  }
  إذا (chdir("dd/../../../dd") != 0) {
    چاپ_کړه("%s: chdir dd/../../../dd failed\n", s);
    وتل(1);
  }
  إذا (chdir("./..") != 0) {
    چاپ_کړه("%s: chdir ./.. failed\n", s);
    وتل(1);
  }

  fd = open("dd/dd/ffff", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: open dd/dd/ffff failed\n", s);
    وتل(1);
  }
  إذا (اقرأ(fd, buf, حجم(buf)) != 2) {
    چاپ_کړه("%s: read dd/dd/ffff wrong len\n", s);
    وتل(1);
  }
  close(fd);

  إذا (open("dd/dd/ff", O_RDONLY) >= 0) {
    چاپ_کړه("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    وتل(1);
  }

  إذا (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    چاپ_کړه("%s: create dd/ff/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    چاپ_کړه("%s: create dd/xx/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (open("dd", O_CREATE) >= 0) {
    چاپ_کړه("%s: create dd succeeded!\n", s);
    وتل(1);
  }
  إذا (open("dd", O_RDWR) >= 0) {
    چاپ_کړه("%s: open dd rdwr succeeded!\n", s);
    وتل(1);
  }
  إذا (open("dd", O_WRONLY) >= 0) {
    چاپ_کړه("%s: open dd wronly succeeded!\n", s);
    وتل(1);
  }
  إذا (link("dd/ff/ff", "dd/dd/xx") == 0) {
    چاپ_کړه("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (link("dd/xx/ff", "dd/dd/xx") == 0) {
    چاپ_کړه("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (link("dd/ff", "dd/dd/ffff") == 0) {
    چاپ_کړه("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    وتل(1);
  }
  إذا (mkdir("dd/ff/ff") == 0) {
    چاپ_کړه("%s: mkdir dd/ff/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (mkdir("dd/xx/ff") == 0) {
    چاپ_کړه("%s: mkdir dd/xx/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (mkdir("dd/dd/ffff") == 0) {
    چاپ_کړه("%s: mkdir dd/dd/ffff succeeded!\n", s);
    وتل(1);
  }
  إذا (unlink("dd/xx/ff") == 0) {
    چاپ_کړه("%s: unlink dd/xx/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (unlink("dd/ff/ff") == 0) {
    چاپ_کړه("%s: unlink dd/ff/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (chdir("dd/ff") == 0) {
    چاپ_کړه("%s: chdir dd/ff succeeded!\n", s);
    وتل(1);
  }
  إذا (chdir("dd/xx") == 0) {
    چاپ_کړه("%s: chdir dd/xx succeeded!\n", s);
    وتل(1);
  }

  إذا (unlink("dd/dd/ffff") != 0) {
    چاپ_کړه("%s: unlink dd/dd/ff failed\n", s);
    وتل(1);
  }
  إذا (unlink("dd/ff") != 0) {
    چاپ_کړه("%s: unlink dd/ff failed\n", s);
    وتل(1);
  }
  إذا (unlink("dd") == 0) {
    چاپ_کړه("%s: unlink non-empty dd succeeded!\n", s);
    وتل(1);
  }
  إذا (unlink("dd/dd") < 0) {
    چاپ_کړه("%s: unlink dd/dd failed\n", s);
    وتل(1);
  }
  إذا (unlink("dd") < 0) {
    چاپ_کړه("%s: unlink dd failed\n", s);
    وتل(1);
  }
}

// test writes that are larger than the log.
فراغ
bigwrite(حرف *s)
{
  صحيح fd, sz;

  unlink("bigwrite");
  لكل (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    إذا (fd < 0) {
      چاپ_کړه("%s: cannot create bigwrite\n", s);
      وتل(1);
    }
    صحيح i;
    لكل (i = 0; i < 2; i++) {
      صحيح cc = اكتب(fd, buf, sz);
      إذا (cc != sz) {
        چاپ_کړه("%s: write(%d) ret %d\n", s, sz, cc);
        وتل(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

فراغ
bigfile(حرف *s)
{
  تعداد { N = 20, SZ = 600 };
  صحيح fd, i, ټول, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot create bigfile", s);
    وتل(1);
  }
  لكل (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    إذا (اكتب(fd, buf, SZ) != SZ) {
      چاپ_کړه("%s: write bigfile failed\n", s);
      وتل(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot open bigfile\n", s);
    وتل(1);
  }
  ټول = 0;
  لكل (i = 0;; i++) {
    cc = اقرأ(fd, buf, SZ / 2);
    إذا (cc < 0) {
      چاپ_کړه("%s: read bigfile failed\n", s);
      وتل(1);
    }
    إذا (cc == 0)
      اكسر;
    إذا (cc != SZ / 2) {
      چاپ_کړه("%s: short read bigfile\n", s);
      وتل(1);
    }
    إذا (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      چاپ_کړه("%s: read bigfile wrong data\n", s);
      وتل(1);
    }
    ټول += cc;
  }
  close(fd);
  إذا (ټول != N * SZ) {
    چاپ_کړه("%s: read bigfile wrong total\n", s);
    وتل(1);
  }
  unlink("bigfile.dat");
}

فراغ
fourteen(حرف *s)
{
  صحيح fd;

  // DIRSIZ is 14.

  إذا (mkdir("12345678901234") != 0) {
    چاپ_کړه("%s: mkdir 12345678901234 failed\n", s);
    وتل(1);
  }
  إذا (mkdir("12345678901234/123456789012345") != 0) {
    چاپ_کړه("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    وتل(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  إذا (fd < 0) {
    چاپ_کړه(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    وتل(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    وتل(1);
  }
  close(fd);

  إذا (mkdir("12345678901234/12345678901234") == 0) {
    چاپ_کړه("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    وتل(1);
  }
  إذا (mkdir("123456789012345/12345678901234") == 0) {
    چاپ_کړه("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    وتل(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

فراغ
rmdot(حرف *s)
{
  إذا (mkdir("dots") != 0) {
    چاپ_کړه("%s: mkdir dots failed\n", s);
    وتل(1);
  }
  إذا (chdir("dots") != 0) {
    چاپ_کړه("%s: chdir dots failed\n", s);
    وتل(1);
  }
  إذا (unlink(".") == 0) {
    چاپ_کړه("%s: rm . worked!\n", s);
    وتل(1);
  }
  إذا (unlink("..") == 0) {
    چاپ_کړه("%s: rm .. worked!\n", s);
    وتل(1);
  }
  إذا (chdir("/") != 0) {
    چاپ_کړه("%s: chdir / failed\n", s);
    وتل(1);
  }
  إذا (unlink("dots/.") == 0) {
    چاپ_کړه("%s: unlink dots/. worked!\n", s);
    وتل(1);
  }
  إذا (unlink("dots/..") == 0) {
    چاپ_کړه("%s: unlink dots/.. worked!\n", s);
    وتل(1);
  }
  إذا (unlink("dots") != 0) {
    چاپ_کړه("%s: unlink dots failed!\n", s);
    وتل(1);
  }
}

فراغ
dirfile(حرف *s)
{
  صحيح fd;

  fd = open("dirfile", O_CREATE);
  إذا (fd < 0) {
    چاپ_کړه("%s: create dirfile failed\n", s);
    وتل(1);
  }
  close(fd);
  إذا (chdir("dirfile") == 0) {
    چاپ_کړه("%s: chdir dirfile succeeded!\n", s);
    وتل(1);
  }
  fd = open("dirfile/xx", 0);
  إذا (fd >= 0) {
    چاپ_کړه("%s: create dirfile/xx succeeded!\n", s);
    وتل(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  إذا (fd >= 0) {
    چاپ_کړه("%s: create dirfile/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (mkdir("dirfile/xx") == 0) {
    چاپ_کړه("%s: mkdir dirfile/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (unlink("dirfile/xx") == 0) {
    چاپ_کړه("%s: unlink dirfile/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (link("README", "dirfile/xx") == 0) {
    چاپ_کړه("%s: link to dirfile/xx succeeded!\n", s);
    وتل(1);
  }
  إذا (unlink("dirfile") != 0) {
    چاپ_کړه("%s: unlink dirfile failed!\n", s);
    وتل(1);
  }

  fd = open(".", O_RDWR);
  إذا (fd >= 0) {
    چاپ_کړه("%s: open . for writing succeeded!\n", s);
    وتل(1);
  }
  fd = open(".", 0);
  إذا (اكتب(fd, "x", 1) > 0) {
    چاپ_کړه("%s: write . succeeded!\n", s);
    وتل(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
فراغ
iref(حرف *s)
{
  صحيح i, fd;

  لكل (i = 0; i < NINODE + 1; i++) {
    إذا (mkdir("irefd") != 0) {
      چاپ_کړه("%s: mkdir irefd failed\n", s);
      وتل(1);
    }
    إذا (chdir("irefd") != 0) {
      چاپ_کړه("%s: chdir irefd failed\n", s);
      وتل(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    إذا (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    إذا (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  لكل (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
فراغ
forktest(حرف *s)
{
  تعداد { N = 1000 };
  صحيح n, pid;

  لكل (n = 0; n < N; n++) {
    pid = fork();
    إذا (pid < 0)
      اكسر;
    إذا (pid == 0)
      وتل(0);
  }

  إذا (n == 0) {
    چاپ_کړه("%s: no fork at all!\n", s);
    وتل(1);
  }

  إذا (n == N) {
    چاپ_کړه("%s: fork claimed to work 1000 times!\n", s);
    وتل(1);
  }

  لكل (; n > 0; n--) {
    إذا (wait(0) < 0) {
      چاپ_کړه("%s: wait stopped early\n", s);
      وتل(1);
    }
  }

  إذا (wait(0) != -1) {
    چاپ_کړه("%s: wait got too many\n", s);
    وتل(1);
  }
}

فراغ
sbrkbasic(حرف *s)
{
  تعداد { TOOMUCH = 1024 * 1024 * 1024 };
  صحيح i, pid, xstatus;
  حرف *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("fork failed in sbrkbasic\n");
    وتل(1);
  }
  إذا (pid == 0) {
    a = sbrk(TOOMUCH);
    إذا (a == (حرف *)SBRK_ERROR) {
      // it's OK if this fails.
      وتل(0);
    }

    لكل (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    وتل(1);
  }

  wait(&xstatus);
  إذا (xstatus == 1) {
    چاپ_کړه("%s: too much memory allocated!\n", s);
    وتل(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  لكل (i = 0; i < 5000; i++) {
    b = sbrk(1);
    إذا (b != a) {
      چاپ_کړه("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      وتل(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: sbrk test fork failed\n", s);
    وتل(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  إذا (c != a + 1) {
    چاپ_کړه("%s: sbrk test failed post-fork\n", s);
    وتل(1);
  }
  إذا (pid == 0)
    وتل(0);
  wait(&xstatus);
  وتل(xstatus);
}

فراغ
sbrkmuch(حرف *s)
{
  تعداد { BIG = 100 * 1024 * 1024 };
  حرف *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  إذا (p != a) {
    چاپ_کړه("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    وتل(1);
  }

  lastaddr = (حرف *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  إذا (c == (حرف *)SBRK_ERROR) {
    چاپ_کړه("%s: sbrk could not deallocate\n", s);
    وتل(1);
  }
  c = sbrk(0);
  إذا (c != a - PGSIZE) {
    چاپ_کړه("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    وتل(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  إذا (c != a || sbrk(0) != a + PGSIZE) {
    چاپ_کړه("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    وتل(1);
  }
  إذا (*lastaddr == 99) {
    // should be zero
    چاپ_کړه("%s: sbrk de-allocation didn't really deallocate\n", s);
    وتل(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  إذا (c != a) {
    چاپ_کړه("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    وتل(1);
  }
}

// can we read the kernel's memory?
فراغ
kernmem(حرف *s)
{
  حرف *a;
  صحيح pid;

  لكل (a = (حرف *)(KERNBASE); a < (حرف *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid == 0) {
      چاپ_کړه("%s: oops could read %p = %x\n", s, a, *a);
      وتل(1);
    }
    صحيح xstatus;
    wait(&xstatus);
    إذا (xstatus != -1) // did kernel kill child?
      وتل(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
فراغ
MAXVAplus(حرف *s)
{
  متطاير uint64 a = MAXVA;
  لكل (; a != 0; a <<= 1) {
    صحيح pid;
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    إذا (pid == 0) {
      *(حرف *)a = 99;
      چاپ_کړه("%s: oops wrote %p\n", s, (فراغ *)a);
      وتل(1);
    }
    صحيح xstatus;
    wait(&xstatus);
    إذا (xstatus != -1) // did kernel kill child?
      وتل(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
فراغ
sbrkfail(حرف *s)
{
  تعداد { BIG = 100 * 1024 * 1024 };
  صحيح i, xstatus;
  صحيح fds[2];
  حرف scratch;
  حرف *c, *a;
  صحيح pids[10];
  صحيح pid;
  صحيح failed;

  failed = 0;
  إذا (pipe(fds) != 0) {
    چاپ_کړه("%s: pipe() failed\n", s);
    وتل(1);
  }
  لكل (i = 0; i < حجم(pids) / حجم(pids[0]); i++) {
    إذا ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      إذا (sbrk(BIG - (uint64)sbrk(0)) == (حرف *)SBRK_ERROR)
        اكتب(fds[1], "0", 1);
      وإلا
        اكتب(fds[1], "1", 1);
      // sit around until killed
      لكل (;;)
        pause(1000);
    }
    إذا (pids[i] != -1) {
      اقرأ(fds[0], &scratch, 1);
      إذا (scratch == '0')
        failed = 1;
    }
  }
  إذا (!failed) {
    چاپ_کړه("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  لكل (i = 0; i < حجم(pids) / حجم(pids[0]); i++) {
    إذا (pids[i] == -1)
      استمر;
    kill(pids[i]);
    wait(0);
  }
  إذا (c == (حرف *)SBRK_ERROR) {
    چاپ_کړه("%s: failed sbrk leaked memory\n", s);
    وتل(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  إذا (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    إذا (a == (حرف *)SBRK_ERROR) {
      وتل(0);
    }
    چاپ_کړه("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    وتل(1);
  }
  wait(&xstatus);
  إذا (xstatus != 0)
    وتل(1);
}

// test reads/writes from/to allocated memory
فراغ
sbrkarg(حرف *s)
{
  حرف *a;
  صحيح fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  إذا (fd < 0) {
    چاپ_کړه("%s: open sbrk failed\n", s);
    وتل(1);
  }
  إذا ((n = اكتب(fd, a, PGSIZE)) < 0) {
    چاپ_کړه("%s: write sbrk failed\n", s);
    وتل(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  إذا (pipe((صحيح *)a) != 0) {
    چاپ_کړه("%s: pipe() failed\n", s);
    وتل(1);
  }
}

فراغ
validatetest(حرف *s)
{
  صحيح hi;
  uint64 p;

  hi = 1100 * 1024;
  لكل (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    إذا (link("nosuchfile", (حرف *)p) != -1) {
      چاپ_کړه("%s: link should not succeed\n", s);
      وتل(1);
    }
  }
}

// does uninitialized data start out zero?
حرف uninit[10000];
فراغ
bsstest(حرف *s)
{
  صحيح i;

  لكل (i = 0; i < حجم(uninit); i++) {
    إذا (uninit[i] != '\0') {
      چاپ_کړه("%s: bss test failed\n", s);
      وتل(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
فراغ
bigargtest(حرف *s)
{
  صحيح pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  إذا (pid == 0) {
    ساكن حرف *دلیلونه[MAXARG];
    صحيح i;
    حرف big[400];
    memset(big, ' ', حجم(big));
    big[حجم(big) - 1] = '\0';
    لكل (i = 0; i < MAXARG - 1; i++)
      دلیلونه[i] = big;
    دلیلونه[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", دلیلونه);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    وتل(0);
  } وإلا إذا (pid < 0) {
    چاپ_کړه("%s: bigargtest: fork failed\n", s);
    وتل(1);
  }

  wait(&xstatus);
  إذا (xstatus != 0)
    وتل(xstatus);
  fd = open("bigarg-ok", 0);
  إذا (fd < 0) {
    چاپ_کړه("%s: bigarg test failed!\n", s);
    وتل(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
فراغ
fsfull()
{
  صحيح nfiles;
  صحيح fsblocks = 0;

  چاپ_کړه("fsfull test\n");

  لكل (nfiles = 0;; nfiles++) {
    حرف name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    چاپ_کړه("writing %s\n", name);
    صحيح fd = open(name, O_CREATE | O_RDWR);
    إذا (fd < 0) {
      چاپ_کړه("open %s failed\n", name);
      اكسر;
    }
    صحيح ټول = 0;
    طالما (1) {
      صحيح cc = اكتب(fd, buf, BSIZE);
      إذا (cc < BSIZE)
        اكسر;
      ټول += cc;
      fsblocks++;
    }
    چاپ_کړه("wrote %d bytes\n", ټول);
    close(fd);
    إذا (ټول == 0)
      اكسر;
  }

  طالما (nfiles >= 0) {
    حرف name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  چاپ_کړه("fsfull test finished, %d blocks\n", fsblocks);
}

فراغ
argptest(حرف *s)
{
  صحيح fd;
  fd = open("init", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: open failed\n", s);
    وتل(1);
  }
  اقرأ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
فراغ
stacktest(حرف *s)
{
  صحيح pid;
  صحيح xstatus;

  pid = fork();
  إذا (pid == 0) {
    حرف *sp = (حرف *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    چاپ_کړه("%s: stacktest: read below stack %d\n", s, *sp);
    وتل(1);
  } وإلا إذا (pid < 0) {
    چاپ_کړه("%s: fork failed\n", s);
    وتل(1);
  }
  wait(&xstatus);
  إذا (xstatus == -1) // kernel killed child?
    وتل(0);
  وإلا
    وتل(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
فراغ
nowrite(حرف *s)
{
  صحيح pid;
  صحيح xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  لكل (صحيح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    pid = fork();
    إذا (pid == 0) {
      متطاير صحيح *addr = (صحيح *)addrs[ai];
      *addr = 10;
      چاپ_کړه("%s: write to %p did not fail!\n", s, addr);
      وتل(0);
    } وإلا إذا (pid < 0) {
      چاپ_کړه("%s: fork failed\n", s);
      وتل(1);
    }
    wait(&xstatus);
    إذا (xstatus == 0) {
      // kernel did not kill child!
      وتل(1);
    }
  }
  وتل(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
فراغ *big = (فراغ *)0xeaeb0b5b00002f5e;
فراغ
pgbug(حرف *s)
{
  حرف *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  وتل(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
فراغ
sbrkbugs(حرف *s)
{
  صحيح pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("fork failed\n");
    وتل(1);
  }
  إذا (pid == 0) {
    صحيح sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    وتل(0);
  }
  wait(0);

  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("fork failed\n");
    وتل(1);
  }
  إذا (pid == 0) {
    صحيح sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    وتل(0);
  }
  wait(0);

  pid = fork();
  إذا (pid < 0) {
    چاپ_کړه("fork failed\n");
    وتل(1);
  }
  إذا (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    وتل(0);
  }
  wait(0);

  وتل(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
فراغ
sbrklast(حرف *s)
{
  uint64 سر_2 = (uint64)sbrk(0);
  إذا ((سر_2 % PGSIZE) != 0)
    sbrk(PGSIZE - (سر_2 % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  سر_2 = (uint64)sbrk(0);
  حرف *p = (حرف *)(سر_2 - 64);
  p[0] = 'x';
  p[1] = '\0';
  صحيح fd = open(p, O_RDWR | O_CREATE);
  اكتب(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  اقرأ(fd, p, 1);
  إذا (p[0] != 'x')
    وتل(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
فراغ
sbrk8000(حرف *s)
{
  sbrk(0x80000004);
  متطاير حرف *سر_2 = sbrk(0);
  *(سر_2 - 1) = *(سر_2 - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
فراغ
badarg(حرف *s)
{
  لكل (صحيح i = 0; i < 50000; i++) {
    حرف *argv[2];
    argv[0] = (حرف *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  وتل(0);
}

#تعریف_کړه REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
فراغ
lazy_alloc(حرف *s)
{
  حرف *i, *prev_پای_ټکی, *new_پای_ټکی;

  prev_پای_ټکی = sbrklazy(REGION_SZ);
  إذا (prev_پای_ټکی == (حرف *)SBRK_ERROR) {
    چاپ_کړه("sbrklazy() failed\n");
    وتل(1);
  }
  new_پای_ټکی = prev_پای_ټکی + REGION_SZ;

  لكل (i = prev_پای_ټکی + PGSIZE; i < new_پای_ټکی; i += 64 * PGSIZE)
    *(حرف **)i = i;

  لكل (i = prev_پای_ټکی + PGSIZE; i < new_پای_ټکی; i += 64 * PGSIZE) {
    إذا (*(حرف **)i != i) {
      چاپ_کړه("failed to read value from memory\n");
      وتل(1);
    }
  }

  وتل(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
فراغ
lazy_unmap(حرف *s)
{
  صحيح pid;
  حرف *i, *prev_پای_ټکی, *new_پای_ټکی;

  prev_پای_ټکی = sbrklazy(REGION_SZ);
  إذا (prev_پای_ټکی == (حرف *)SBRK_ERROR) {
    چاپ_کړه("sbrklazy() failed\n");
    وتل(1);
  }
  new_پای_ټکی = prev_پای_ټکی + REGION_SZ;

  لكل (i = prev_پای_ټکی + PGSIZE; i < new_پای_ټکی; i += PGSIZE * PGSIZE)
    *(حرف **)i = i;

  لكل (i = prev_پای_ټکی + PGSIZE; i < new_پای_ټکی; i += PGSIZE * PGSIZE) {
    pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("error forking\n");
      وتل(1);
    } وإلا إذا (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(حرف **)i = i;
      وتل(0);
    } وإلا {
      صحيح حالت;
      wait(&حالت);
      إذا (حالت == 0) {
        چاپ_کړه("memory not unmapped\n");
        وتل(1);
      }
    }
  }

  وتل(0);
}

فراغ
lazy_copy(حرف *s)
{
  // copyinstr on lazy page
  {
    حرف *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    فراغ *xx = sbrk(0);
    فراغ *ret = sbrk(-(((uint64)xx) + 1));
    إذا (ret != xx) {
      چاپ_کړه("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      وتل(1);
    }
  }

  // read() and write() to these addresses should fail.
  بینښه اوږد bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  لكل (صحيح i = 0; i < حجم(bad) / حجم(bad[0]); i++) {
    صحيح fd = open("README", 0);
    إذا (fd < 0) {
      چاپ_کړه("cannot open README\n");
      وتل(1);
    }
    إذا (اقرأ(fd, (حرف *)bad[i], 512) >= 0) {
      چاپ_کړه("read succeeded\n");
      وتل(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    إذا (fd < 0) {
      چاپ_کړه("cannot open junk\n");
      وتل(1);
    }
    إذا (اكتب(fd, (حرف *)bad[i], 512) >= 0) {
      چاپ_کړه("write succeeded\n");
      وتل(1);
    }
    close(fd);
  }

  وتل(0);
}

فراغ
lazy_copyinstr(حرف *s)
{
  حرف *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  إذا ((uint64)p % PGSIZE != 0) {
    چاپ_کړه("%s: sbrk did not align\n", s);
    وتل(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  صحيح fd = open(&p[4095], O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("could not open /");
    وتل(1);
  }

  بنية stat st;
  صحيح r = fstat(fd, &st);
  إذا (r < 0) {
    چاپ_کړه("could not stat /");
    وتل(1);
  }

  إذا (st.type != T_DIR) {
    چاپ_کړه("/ is not T_DIR");
    وتل(1);
  }

  close(fd);
}

فراغ
lazy_sbrk(حرف *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  حرف *p = sbrk(0);
  طالما ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    إذا (p < 0) {
      چاپ_کړه("sbrklazy(%d) returned %p\n", 1 << 30, p);
      وتل(1);
    }

    p = sbrklazy(0);
  }

  صحيح n = TRAPFRAME - PGSIZE - (uint64)p;

  حرف *p1 = sbrklazy(n);
  إذا (p1 < 0 || p1 != p) {
    چاپ_کړه("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    وتل(1);
  }

  p = sbrk(PGSIZE);
  إذا (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    چاپ_کړه("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    وتل(1);
  }

  p[0] = 1;
  إذا (p[1] != 0) {
    چاپ_کړه("sbrk() returned non-zero-filled memory\n");
    وتل(1);
  }

  p = sbrk(1);
  إذا ((uint64)p != -1) {
    چاپ_کړه("sbrk(1) returned %p, expected error\n", p);
    وتل(1);
  }

  p = sbrklazy(1);
  إذا ((uint64)p != -1) {
    چاپ_کړه("sbrklazy(1) returned %p, expected error\n", p);
    وتل(1);
  }

  وتل(0);
}

فراغ
partial_write(حرف *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  صحيح fd = open("testfile", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot create testfile\n", s);
    وتل(1);
  }

  صحيح cc = اكتب(fd, "A", 1);
  إذا (cc != 1) {
    چاپ_کړه("%s: could not write A\n", s);
    وتل(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot re-open testfile\n", s);
    وتل(1);
  }

  حرف *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  إذا ((uint64)p % PGSIZE != 0) {
    چاپ_کړه("%s: sbrk did not align\n", s);
    وتل(1);
  }

  p[-1] = 'X';

  cc = اكتب(fd, p - 1, 2);
  إذا (cc != -1) {
    چاپ_کړه("%s: write succeeded, should have failed\n", s);
    وتل(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot re-open testfile\n", s);
    وتل(1);
  }

  حرف b;
  cc = اقرأ(fd, &b, 1);
  إذا (cc != 1) {
    چاپ_کړه("%s: cannot read testfile\n", s);
    وتل(1);
  }

  close(fd);

  إذا (b != 'X') {
    چاپ_کړه("%s: read returned %c, expected X\n", s, b);
    وتل(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  لكل (صحيح i = 0; i < 64; i++) {
    حرف buf[1024];
    memset(buf, 0, حجم(buf));
    cc = اكتب(fd, buf, حجم(buf));
    إذا (cc != حجم(buf)) {
      چاپ_کړه("%s: could not write to bigfile\n", s);
      وتل(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot re-open testfile\n", s);
    وتل(1);
  }

  cc = اقرأ(fd, &b, 1);
  إذا (cc != 1) {
    چاپ_کړه("%s: cannot read testfile\n", s);
    وتل(1);
  }

  close(fd);

  إذا (b != 'X') {
    چاپ_کړه("%s: read returned %c, expected X\n", s, b);
    وتل(1);
  }

  unlink("testfile");
}

فراغ
unlinkcwd(حرف *s)
{
  إذا (mkdir("/a") < 0) {
    چاپ_کړه("%s: mkdir /a failed\n", s);
    وتل(1);
  }
  إذا (mkdir("/a/b") < 0) {
    چاپ_کړه("%s: mkdir /a/b failed\n", s);
    وتل(1);
  }
  إذا (chdir("/a/b") < 0) {
    چاپ_کړه("%s: chdir failed\n", s);
    وتل(1);
  }
  إذا (unlink("/a/b") < 0) {
    چاپ_کړه("%s: unlink /a/b failed\n", s);
    وتل(1);
  }
  إذا (unlink("/a") < 0) {
    چاپ_کړه("%s: unlink /a failed\n", s);
    وتل(1);
  }
  إذا (open("../", O_RDONLY) > 0) {
    چاپ_کړه("%s: open ../ non-existing directory\n", s);
  }
  إذا (open("../c", O_CREATE) > 0) {
    چاپ_کړه("%s: create ../c non-existing file\n", s);
  }
}

بنية test {
  فراغ (*f)(حرف *);
  حرف *s;
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
فراغ
bigdir(حرف *s)
{
  تعداد { N = 500 };
  صحيح i, fd;
  حرف name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  إذا (fd < 0) {
    چاپ_کړه("%s: bigdir create failed\n", s);
    وتل(1);
  }
  close(fd);

  لكل (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    إذا (link("bd", name) != 0) {
      چاپ_کړه("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      وتل(1);
    }
  }

  unlink("bd");
  لكل (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    إذا (unlink(name) != 0) {
      چاپ_کړه("%s: bigdir unlink failed", s);
      وتل(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
فراغ
manywrites(حرف *s)
{
  صحيح nchildren = 4;
  صحيح howmany = 30; // increase to look for deadlock

  لكل (صحيح ci = 0; ci < nchildren; ci++) {
    صحيح pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("fork failed\n");
      وتل(1);
    }

    إذا (pid == 0) {
      حرف name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      لكل (صحيح iters = 0; iters < howmany; iters++) {
        لكل (صحيح i = 0; i < ci + 1; i++) {
          صحيح fd = open(name, O_CREATE | O_RDWR);
          إذا (fd < 0) {
            چاپ_کړه("%s: cannot create %s\n", s, name);
            وتل(1);
          }
          صحيح sz = حجم(buf);
          صحيح cc = اكتب(fd, buf, sz);
          إذا (cc != sz) {
            چاپ_کړه("%s: write(%d) ret %d\n", s, sz, cc);
            وتل(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      وتل(0);
    }
  }

  لكل (صحيح ci = 0; ci < nchildren; ci++) {
    صحيح st = 0;
    wait(&st);
    إذا (st != 0)
      وتل(st);
  }
  وتل(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
فراغ
badwrite(حرف *s)
{
  صحيح assumed_free = 600;

  unlink("junk");
  لكل (صحيح i = 0; i < assumed_free; i++) {
    صحيح fd = open("junk", O_CREATE | O_WRONLY);
    إذا (fd < 0) {
      چاپ_کړه("open junk failed\n");
      وتل(1);
    }
    اكتب(fd, (حرف *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  صحيح fd = open("junk", O_CREATE | O_WRONLY);
  إذا (fd < 0) {
    چاپ_کړه("open junk failed\n");
    وتل(1);
  }
  إذا (اكتب(fd, "x", 1) != 1) {
    چاپ_کړه("write failed\n");
    وتل(1);
  }
  close(fd);
  unlink("junk");

  وتل(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
فراغ
execout(حرف *s)
{
  لكل (صحيح avail = 0; avail < 15; avail++) {
    صحيح pid = fork();
    إذا (pid < 0) {
      چاپ_کړه("fork failed\n");
      وتل(1);
    } وإلا إذا (pid == 0) {
      // allocate all of memory.
      طالما (1) {
        حرف *a = sbrk(PGSIZE);
        إذا (a == SBRK_ERROR)
          اكسر;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      لكل (صحيح i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      حرف *دلیلونه[] = {"echo", "x", 0};
      exec("echo", دلیلونه);
      وتل(0);
    } وإلا {
      wait((صحيح *)0);
    }
  }

  وتل(0);
}

// can the kernel tolerate running out of disk space?
فراغ
diskfull(حرف *s)
{
  صحيح fi;
  صحيح بشپړ = 0;

  unlink("diskfulldir");

  لكل (fi = 0; بشپړ == 0 && '0' + fi < 0177; fi++) {
    حرف name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    صحيح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    إذا (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      چاپ_کړه("%s: could not create file %s\n", s, name);
      بشپړ = 1;
      اكسر;
    }
    لكل (صحيح i = 0; i < MAXFILE; i++) {
      حرف buf[BSIZE];
      إذا (اكتب(fd, buf, BSIZE) != BSIZE) {
        بشپړ = 1;
        close(fd);
        اكسر;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  صحيح nzz = 128;
  لكل (صحيح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحيح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    إذا (fd < 0)
      اكسر;
    close(fd);
  }

  // this mkdir() is expected to fail.
  إذا (mkdir("diskfulldir") == 0)
    چاپ_کړه("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  لكل (صحيح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  لكل (صحيح i = 0; '0' + i < 0177; i++) {
    حرف name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

فراغ
outofinodes(حرف *s)
{
  صحيح nzz = 32 * 32;
  لكل (صحيح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحيح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    إذا (fd < 0) {
      // failure is eventually expected.
      اكسر;
    }
    close(fd);
  }

  لكل (صحيح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

فراغ
linkoverflow(حرف *s)
{
  تعداد { موخه_2 = 32768 };
  تعداد { DIRS = 64 };
  بنية stat st;
  صحيح i;

  unlink("/lof");
  صحيح fd = open("/lof", O_CREATE | O_RDWR);
  إذا (fd < 0) {
    چاپ_کړه("%s: cannot create /lof\n", s);
    وتل(1);
  }
  close(fd);

  لكل (i = 0; i < موخه_2; i++) {
    صحيح d = i % DIRS;
    صحيح f = i / DIRS;

    حرف pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    إذا (f == 0 && mkdir(pn) < 0) {
      چاپ_کړه("%s: mkdir(%s) failed\n", s, pn);
      وتل(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    إذا (link("/lof", pn) < 0) {
      إذا (stat("/lof", &st) < 0) {
        چاپ_کړه("%s: stat(/lof) failed\n", s);
        وتل(1);
      }
      إذا (st.nlink >= 32767) {
        // overflow check succeeded.
        اكسر;
      }
      چاپ_کړه("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      وتل(1);
    }

    إذا (i % 100 == 0) {
      چاپ_کړه("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  إذا (stat("/lof", &st) < 0) {
    چاپ_کړه("%s: stat(/lof) failed\n", s);
    وتل(1);
  }

  unlink("/lof");

  إذا (st.nlink < 0) {
    چاپ_کړه("%s: negative link count: %d\n", s, st.nlink);
    وتل(1);
  }
}

بنية test slowtests[] = {
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
صحيح
پرلپسې(فراغ f(حرف *), حرف *s)
{
  صحيح pid;
  صحيح xstatus;

  چاپ_کړه("test %s: ", s);
  إذا ((pid = fork()) < 0) {
    چاپ_کړه("runtest: fork error\n");
    وتل(1);
  }
  إذا (pid == 0) {
    f(s);
    وتل(0);
  } وإلا {
    wait(&xstatus);
    إذا (xstatus != 0)
      چاپ_کړه("FAILED\n");
    وإلا
      چاپ_کړه("OK\n");
    أرجع xstatus == 0;
  }
}

صحيح
runtests(بنية test *tests, حرف *justone, صحيح continuous)
{
  صحيح ntests = 0;
  لكل (بنية test *t = tests; t->s != 0; t++) {
    إذا ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      إذا (!پرلپسې(t->f, t->s)) {
        إذا (continuous != 2) {
          چاپ_کړه("SOME TESTS FAILED\n");
          أرجع -1;
        }
      }
    }
  }
  أرجع ntests;
}

// use sbrk() to count how many free physical memory pages there are.
صحيح
countfree()
{
  صحيح n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  طالما (1) {
    حرف *a = sbrk(PGSIZE);
    إذا (a == SBRK_ERROR) {
      اكسر;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  أرجع n;
}

صحيح
drivetests(صحيح چټک, صحيح continuous, حرف *justone)
{
  افعل {
    چاپ_کړه("usertests starting\n");
    صحيح free0 = countfree();
    صحيح free1 = 0;
    صحيح ntests = 0;
    صحيح n;
    n = runtests(quicktests, justone, continuous);
    إذا (n < 0) {
      إذا (continuous != 2) {
        أرجع 1;
      }
    } وإلا {
      ntests += n;
    }
    إذا (!چټک) {
      إذا (justone == 0)
        چاپ_کړه("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      إذا (n < 0) {
        إذا (continuous != 2) {
          أرجع 1;
        }
      } وإلا {
        ntests += n;
      }
    }
    إذا ((free1 = countfree()) < free0) {
      چاپ_کړه("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      إذا (continuous != 2) {
        أرجع 1;
      }
    }
    إذا (justone != 0 && ntests == 0) {
      چاپ_کړه("NO TESTS EXECUTED\n");
      أرجع 1;
    }
  } طالما (continuous);
  أرجع 0;
}

صحيح
اصلي(صحيح argc, حرف *argv[])
{
  صحيح continuous = 0;
  صحيح چټک = 0;
  حرف *justone = 0;

  إذا (argc == 2 && strcmp(argv[1], "-q") == 0) {
    چټک = 1;
  } وإلا إذا (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } وإلا إذا (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } وإلا إذا (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } وإلا إذا (argc > 1) {
    چاپ_کړه("Usage: usertests [-c] [-C] [-q] [testname]\n");
    وتل(1);
  }
  إذا (drivetests(چټک, continuous, justone)) {
    وتل(1);
  }
  چاپ_کړه("ALL TESTS PASSED\n");
  وتل(0);
}
