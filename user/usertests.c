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

#تعریف_کرو BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

حرف buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
خالی
copyin(حرف *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  برائے (صحیح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open("copyin1", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      لکھو("open(copyin1) failed\n");
      باہر_نکلو(1);
    }
    صحیح n = اكتب(fd, (خالی *)addr, 8192);
    اگر (n >= 0) {
      لکھو("write(fd, %p, 8192) returned %d, not -1\n", (خالی *)addr, n);
      باہر_نکلو(1);
    }
    close(fd);
    unlink("copyin1");

    n = اكتب(1, (حرف *)addr, 8192);
    اگر (n > 0) {
      لکھو("write(1, %p, 8192) returned %d, not -1 or 0\n", (خالی *)addr, n);
      باہر_نکلو(1);
    }

    صحیح fds[2];
    اگر (pipe(fds) < 0) {
      لکھو("pipe() failed\n");
      باہر_نکلو(1);
    }
    n = اكتب(fds[1], (حرف *)addr, 8192);
    اگر (n > 0) {
      لکھو("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (خالی *)addr,
             n);
      باہر_نکلو(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
خالی
copyout(حرف *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  برائے (صحیح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open("README", 0);
    اگر (fd < 0) {
      لکھو("open(README) failed\n");
      باہر_نکلو(1);
    }
    صحیح n = اقرأ(fd, (خالی *)addr, 8192);
    اگر (n > 0) {
      لکھو("read(fd, %p, 8192) returned %d, not -1 or 0\n", (خالی *)addr, n);
      باہر_نکلو(1);
    }
    close(fd);

    صحیح fds[2];
    اگر (pipe(fds) < 0) {
      لکھو("pipe() failed\n");
      باہر_نکلو(1);
    }
    n = اكتب(fds[1], "x", 1);
    اگر (n != 1) {
      لکھو("pipe write failed\n");
      باہر_نکلو(1);
    }
    n = اقرأ(fds[0], (خالی *)addr, 8192);
    اگر (n > 0) {
      لکھو("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (خالی *)addr,
             n);
      باہر_نکلو(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
خالی
copyinstr1(حرف *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  برائے (صحیح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    صحیح fd = open((حرف *)addr, O_CREATE | O_WRONLY);
    اگر (fd >= 0) {
      لکھو("open(%p) returned %d, not -1\n", (خالی *)addr, fd);
      باہر_نکلو(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
خالی
copyinstr2(حرف *s)
{
  حرف b[MAXPATH + 1];

  برائے (صحیح i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  صحیح ret = unlink(b);
  اگر (ret != -1) {
    لکھو("unlink(%s) returned %d, not -1\n", b, ret);
    باہر_نکلو(1);
  }

  صحیح fd = open(b, O_CREATE | O_WRONLY);
  اگر (fd != -1) {
    لکھو("open(%s) returned %d, not -1\n", b, fd);
    باہر_نکلو(1);
  }

  ret = link(b, b);
  اگر (ret != -1) {
    لکھو("link(%s, %s) returned %d, not -1\n", b, b, ret);
    باہر_نکلو(1);
  }

  حرف *دلائل[] = {"xx", 0};
  ret = exec(b, دلائل);
  اگر (ret != -1) {
    لکھو("exec(%s) returned %d, not -1\n", b, fd);
    باہر_نکلو(1);
  }

  صحیح pid = fork();
  اگر (pid < 0) {
    لکھو("fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    ساکن حرف big[PGSIZE + 1];
    برائے (صحیح i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    حرف *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    اگر (ret != -1) {
      لکھو("exec(echo, BIG) returned %d, not -1\n", fd);
      باہر_نکلو(1);
    }
    باہر_نکلو(747); // OK
  }

  صحیح st = 0;
  wait(&st);
  اگر (st != 747) {
    لکھو("exec(echo, BIG) succeeded, should have failed\n");
    باہر_نکلو(1);
  }
}

// what if a string argument crosses over the end of last user page?
خالی
copyinstr3(حرف *s)
{
  sbrk(8192);
  uint64 چوٹی = (uint64)sbrk(0);
  اگر ((چوٹی % PGSIZE) != 0) {
    sbrk(PGSIZE - (چوٹی % PGSIZE));
  }
  چوٹی = (uint64)sbrk(0);
  اگر (چوٹی % PGSIZE) {
    لکھو("oops\n");
    باہر_نکلو(1);
  }

  حرف *b = (حرف *)(چوٹی - 1);
  *b = 'x';

  صحیح ret = unlink(b);
  اگر (ret != -1) {
    لکھو("unlink(%s) returned %d, not -1\n", b, ret);
    باہر_نکلو(1);
  }

  صحیح fd = open(b, O_CREATE | O_WRONLY);
  اگر (fd != -1) {
    لکھو("open(%s) returned %d, not -1\n", b, fd);
    باہر_نکلو(1);
  }

  ret = link(b, b);
  اگر (ret != -1) {
    لکھو("link(%s, %s) returned %d, not -1\n", b, b, ret);
    باہر_نکلو(1);
  }

  حرف *دلائل[] = {"xx", 0};
  ret = exec(b, دلائل);
  اگر (ret != -1) {
    لکھو("exec(%s) returned %d, not -1\n", b, fd);
    باہر_نکلو(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
خالی
rwsbrk(حرف *s)
{
  صحیح fd, n;

  uint64 a = (uint64)sbrk(8192);

  اگر (a == (uint64)SBRK_ERROR) {
    لکھو("sbrk(rwsbrk) failed\n");
    باہر_نکلو(1);
  }

  اگر (sbrk(-8192) == SBRK_ERROR) {
    لکھو("sbrk(rwsbrk) shrink failed\n");
    باہر_نکلو(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  اگر (fd < 0) {
    لکھو("open(rwsbrk) failed\n");
    باہر_نکلو(1);
  }
  n = اكتب(fd, (خالی *)(a + PGSIZE), 1024);
  اگر (n >= 0) {
    لکھو("write(fd, %p, 1024) returned %d, not -1\n", (خالی *)a + PGSIZE, n);
    باہر_نکلو(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  اگر (fd < 0) {
    لکھو("open(README) failed\n");
    باہر_نکلو(1);
  }
  n = اقرأ(fd, (خالی *)(a + PGSIZE), 10);
  اگر (n >= 0) {
    لکھو("read(fd, %p, 10) returned %d, not -1\n", (خالی *)a + PGSIZE, n);
    باہر_نکلو(1);
  }
  close(fd);

  باہر_نکلو(0);
}

// test O_TRUNC.
خالی
truncate1(حرف *s)
{
  حرف buf[32];

  unlink("truncfile");
  صحیح fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  اكتب(fd1, "abcd", 4);
  close(fd1);

  صحیح fd2 = open("truncfile", O_RDONLY);
  صحیح n = اقرأ(fd2, buf, حجم(buf));
  اگر (n != 4) {
    لکھو("%s: read %d bytes, wanted 4\n", s, n);
    باہر_نکلو(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  صحیح fd3 = open("truncfile", O_RDONLY);
  n = اقرأ(fd3, buf, حجم(buf));
  اگر (n != 0) {
    لکھو("aaa fd3=%d\n", fd3);
    لکھو("%s: read %d bytes, wanted 0\n", s, n);
    باہر_نکلو(1);
  }

  n = اقرأ(fd2, buf, حجم(buf));
  اگر (n != 0) {
    لکھو("bbb fd2=%d\n", fd2);
    لکھو("%s: read %d bytes, wanted 0\n", s, n);
    باہر_نکلو(1);
  }

  اكتب(fd1, "abcdef", 6);

  n = اقرأ(fd3, buf, حجم(buf));
  اگر (n != 6) {
    لکھو("%s: read %d bytes, wanted 6\n", s, n);
    باہر_نکلو(1);
  }

  n = اقرأ(fd2, buf, حجم(buf));
  اگر (n != 2) {
    لکھو("%s: read %d bytes, wanted 2\n", s, n);
    باہر_نکلو(1);
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
خالی
truncate2(حرف *s)
{
  unlink("truncfile");

  صحیح fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  اكتب(fd1, "abcd", 4);

  صحیح fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  صحیح n = اكتب(fd1, "x", 1);
  اگر (n != -1) {
    لکھو("%s: write returned %d, expected -1\n", s, n);
    باہر_نکلو(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

خالی
truncate3(حرف *s)
{
  صحیح pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }

  اگر (pid == 0) {
    برائے (صحیح i = 0; i < 100; i++) {
      حرف buf[32];
      صحیح fd = open("truncfile", O_WRONLY);
      اگر (fd < 0) {
        لکھو("%s: open failed\n", s);
        باہر_نکلو(1);
      }
      صحیح n = اكتب(fd, "1234567890", 10);
      اگر (n != 10) {
        لکھو("%s: write got %d, expected 10\n", s, n);
        باہر_نکلو(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      اقرأ(fd, buf, حجم(buf));
      close(fd);
    }
    باہر_نکلو(0);
  }

  برائے (صحیح i = 0; i < 150; i++) {
    صحیح fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    اگر (fd < 0) {
      لکھو("%s: open failed\n", s);
      باہر_نکلو(1);
    }
    صحیح n = اكتب(fd, "xxx", 3);
    اگر (n != 3) {
      لکھو("%s: write got %d, expected 3\n", s, n);
      باہر_نکلو(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  باہر_نکلو(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
خالی
iputtest(حرف *s)
{
  اگر (mkdir("iputdir") < 0) {
    لکھو("%s: mkdir failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("iputdir") < 0) {
    لکھو("%s: chdir iputdir failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("../iputdir") < 0) {
    لکھو("%s: unlink ../iputdir failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("/") < 0) {
    لکھو("%s: chdir / failed\n", s);
    باہر_نکلو(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
خالی
exitiputtest(حرف *s)
{
  صحیح pid, xstatus;

  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    اگر (mkdir("iputdir") < 0) {
      لکھو("%s: mkdir failed\n", s);
      باہر_نکلو(1);
    }
    اگر (chdir("iputdir") < 0) {
      لکھو("%s: child chdir failed\n", s);
      باہر_نکلو(1);
    }
    اگر (unlink("../iputdir") < 0) {
      لکھو("%s: unlink ../iputdir failed\n", s);
      باہر_نکلو(1);
    }
    باہر_نکلو(0);
  }
  wait(&xstatus);
  باہر_نکلو(xstatus);
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
خالی
openiputtest(حرف *s)
{
  صحیح pid, xstatus;

  اگر (mkdir("oidir") < 0) {
    لکھو("%s: mkdir oidir failed\n", s);
    باہر_نکلو(1);
  }
  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    صحیح fd = open("oidir", O_RDWR);
    اگر (fd >= 0) {
      لکھو("%s: open directory for write succeeded\n", s);
      باہر_نکلو(1);
    }
    باہر_نکلو(0);
  }
  pause(1);
  اگر (unlink("oidir") != 0) {
    لکھو("%s: unlink failed\n", s);
    باہر_نکلو(1);
  }
  wait(&xstatus);
  باہر_نکلو(xstatus);
}

// simple file system tests

خالی
opentest(حرف *s)
{
  صحیح fd;

  fd = open("echo", 0);
  اگر (fd < 0) {
    لکھو("%s: open echo failed!\n", s);
    باہر_نکلو(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  اگر (fd >= 0) {
    لکھو("%s: open doesnotexist succeeded!\n", s);
    باہر_نکلو(1);
  }
}

خالی
writetest(حرف *s)
{
  صحیح fd;
  صحیح i;
  شمار { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: error: creat small failed!\n", s);
    باہر_نکلو(1);
  }
  برائے (i = 0; i < N; i++) {
    اگر (اكتب(fd, "aaaaaaaaaa", SZ) != SZ) {
      لکھو("%s: error: write aa %d new file failed\n", s, i);
      باہر_نکلو(1);
    }
    اگر (اكتب(fd, "bbbbbbbbbb", SZ) != SZ) {
      لکھو("%s: error: write bb %d new file failed\n", s, i);
      باہر_نکلو(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: error: open small failed!\n", s);
    باہر_نکلو(1);
  }
  i = اقرأ(fd, buf, N * SZ * 2);
  اگر (i != N * SZ * 2) {
    لکھو("%s: read failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (unlink("small") < 0) {
    لکھو("%s: unlink small failed\n", s);
    باہر_نکلو(1);
  }
}

خالی
writebig(حرف *s)
{
  صحیح i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: error: creat big failed!\n", s);
    باہر_نکلو(1);
  }

  برائے (i = 0; i < MAXFILE; i++) {
    ((صحیح *)buf)[0] = i;
    اگر (اكتب(fd, buf, BSIZE) != BSIZE) {
      لکھو("%s: error: write big file failed i=%d\n", s, i);
      باہر_نکلو(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: error: open big failed!\n", s);
    باہر_نکلو(1);
  }

  n = 0;
  برائے (;;) {
    i = اقرأ(fd, buf, BSIZE);
    اگر (i == 0) {
      اگر (n != MAXFILE) {
        لکھو("%s: read only %d blocks from big", s, n);
        باہر_نکلو(1);
      }
      توڑو;
    } ورنہ اگر (i != BSIZE) {
      لکھو("%s: read failed %d\n", s, i);
      باہر_نکلو(1);
    }
    اگر (((صحیح *)buf)[0] != n) {
      لکھو("%s: read content of block %d is %d\n", s, n, ((صحیح *)buf)[0]);
      باہر_نکلو(1);
    }
    n++;
  }
  close(fd);
  اگر (unlink("big") < 0) {
    لکھو("%s: unlink big failed\n", s);
    باہر_نکلو(1);
  }
}

// many creates, followed by unlink test
خالی
createtest(حرف *s)
{
  صحیح i, fd;
  شمار { N = 52 };

  حرف name[3];
  name[0] = 'a';
  name[2] = '\0';
  برائے (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  برائے (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

خالی
dirtest(حرف *s)
{
  اگر (mkdir("dir0") < 0) {
    لکھو("%s: mkdir failed\n", s);
    باہر_نکلو(1);
  }

  اگر (chdir("dir0") < 0) {
    لکھو("%s: chdir dir0 failed\n", s);
    باہر_نکلو(1);
  }

  اگر (chdir("..") < 0) {
    لکھو("%s: chdir .. failed\n", s);
    باہر_نکلو(1);
  }

  اگر (unlink("dir0") < 0) {
    لکھو("%s: unlink dir0 failed\n", s);
    باہر_نکلو(1);
  }
}

خالی
exectest(حرف *s)
{
  صحیح fd, xstatus, pid;
  حرف *echoargv[] = {"echo", "OK", 0};
  حرف buf[3];

  unlink("echo-ok");
  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    صحیح errfd = dup(1);
    اگر (errfd < 0) {
      لکھو("%s: dup failed\n", s);
      باہر_نکلو(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      باہر_نکلو(1);
    }
    اگر (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      باہر_نکلو(1);
    }
    اگر (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      باہر_نکلو(1);
    }
    // won't get to here
  }
  اگر (wait(&xstatus) != pid) {
    لکھو("%s: wait failed!\n", s);
  }
  اگر (xstatus != 0) {
    لکھو("%s: nonzero wait status %d\n", s, xstatus);
    باہر_نکلو(1);
  }

  fd = open("echo-ok", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: open failed\n", s);
    باہر_نکلو(1);
  }
  اگر (اقرأ(fd, buf, 2) != 2) {
    لکھو("%s: read failed\n", s);
    باہر_نکلو(1);
  }
  unlink("echo-ok");
  اگر (buf[0] == 'O' && buf[1] == 'K')
    باہر_نکلو(0);
  ورنہ {
    لکھو("%s: wrong output\n", s);
    باہر_نکلو(1);
  }
}

// simple fork and pipe read/write

خالی
pipe1(حرف *s)
{
  صحیح fds[2], pid, xstatus;
  صحیح seq, i, n, cc, کل;
  شمار { N = 5, SZ = 1033 };

  اگر (pipe(fds) != 0) {
    لکھو("%s: pipe() failed\n", s);
    باہر_نکلو(1);
  }
  pid = fork();
  seq = 0;
  اگر (pid == 0) {
    close(fds[0]);
    برائے (n = 0; n < N; n++) {
      برائے (i = 0; i < SZ; i++)
        buf[i] = seq++;
      اگر (اكتب(fds[1], buf, SZ) != SZ) {
        لکھو("%s: pipe1 oops 1\n", s);
        باہر_نکلو(1);
      }
    }
    باہر_نکلو(0);
  } ورنہ اگر (pid > 0) {
    close(fds[1]);
    کل = 0;
    cc = 1;
    جب ((n = اقرأ(fds[0], buf, cc)) > 0) {
      برائے (i = 0; i < n; i++) {
        اگر ((buf[i] & 0xff) != (seq++ & 0xff)) {
          لکھو("%s: pipe1 oops 2\n", s);
          واپس;
        }
      }
      کل += n;
      cc = cc * 2;
      اگر (cc > حجم(buf))
        cc = حجم(buf);
    }
    اگر (کل != N * SZ) {
      لکھو("%s: pipe1 oops 3 total %d\n", s, کل);
      باہر_نکلو(1);
    }
    close(fds[0]);
    wait(&xstatus);
    باہر_نکلو(xstatus);
  } ورنہ {
    لکھو("%s: fork() failed\n", s);
    باہر_نکلو(1);
  }
}

// test if child is killed (status = -1)
خالی
killstatus(حرف *s)
{
  صحیح xst;

  برائے (صحیح i = 0; i < 100; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid1 == 0) {
      جب (1) {
        getpid();
      }
      باہر_نکلو(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    اگر (xst != -1) {
      لکھو("%s: status should be -1\n", s);
      باہر_نکلو(1);
    }
  }
  باہر_نکلو(0);
}

// test that kill on unused process 0 doesn't set the killed flag
خالی
killzero(حرف *s)
{
  صحیح pid, xst;

  kill(0);

  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    باہر_نکلو(7);
  }
  اگر (wait(&xst) != pid) {
    لکھو("%s: wait wrong pid\n", s);
    باہر_نکلو(1);
  }
  اگر (xst != 7) {
    لکھو("%s: child exited with status %d, expected 7\n", s, xst);
    باہر_نکلو(1);
  }
  باہر_نکلو(0);
}

// meant to be run w/ at most two CPUs
خالی
preempt(حرف *s)
{
  صحیح pid1, pid2, pid3;
  صحیح pfds[2];

  pid1 = fork();
  اگر (pid1 < 0) {
    لکھو("%s: fork failed", s);
    باہر_نکلو(1);
  }
  اگر (pid1 == 0)
    برائے (;;)
      ;

  pid2 = fork();
  اگر (pid2 < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid2 == 0)
    برائے (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  اگر (pid3 < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid3 == 0) {
    close(pfds[0]);
    اگر (اكتب(pfds[1], "x", 1) != 1)
      لکھو("%s: preempt write error", s);
    close(pfds[1]);
    برائے (;;)
      ;
  }

  close(pfds[1]);
  اگر (اقرأ(pfds[0], buf, حجم(buf)) != 1) {
    لکھو("%s: preempt read error", s);
    واپس;
  }
  close(pfds[0]);
  لکھو("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  لکھو("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
خالی
exitwait(حرف *s)
{
  صحیح i, pid;

  برائے (i = 0; i < 100; i++) {
    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid) {
      صحیح xstate;
      اگر (wait(&xstate) != pid) {
        لکھو("%s: wait wrong pid\n", s);
        باہر_نکلو(1);
      }
      اگر (i != xstate) {
        لکھو("%s: wait wrong exit status\n", s);
        باہر_نکلو(1);
      }
    } ورنہ {
      باہر_نکلو(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
خالی
reparent(حرف *s)
{
  صحیح master_pid = getpid();
  برائے (صحیح i = 0; i < 200; i++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid) {
      اگر (wait(0) != pid) {
        لکھو("%s: wait wrong pid\n", s);
        باہر_نکلو(1);
      }
    } ورنہ {
      صحیح pid2 = fork();
      اگر (pid2 < 0) {
        kill(master_pid);
        باہر_نکلو(1);
      }
      باہر_نکلو(0);
    }
  }
  باہر_نکلو(0);
}

// what if two children exit() at the same time?
خالی
twochildren(حرف *s)
{
  برائے (صحیح i = 0; i < 1000; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid1 == 0) {
      باہر_نکلو(0);
    } ورنہ {
      صحیح pid2 = fork();
      اگر (pid2 < 0) {
        لکھو("%s: fork failed\n", s);
        باہر_نکلو(1);
      }
      اگر (pid2 == 0) {
        باہر_نکلو(0);
      } ورنہ {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
خالی
forkfork(حرف *s)
{
  شمار { N = 2 };

  برائے (صحیح i = 0; i < N; i++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed", s);
      باہر_نکلو(1);
    }
    اگر (pid == 0) {
      برائے (صحیح j = 0; j < 200; j++) {
        صحیح pid1 = fork();
        اگر (pid1 < 0) {
          باہر_نکلو(1);
        }
        اگر (pid1 == 0) {
          باہر_نکلو(0);
        }
        wait(0);
      }
      باہر_نکلو(0);
    }
  }

  صحیح xstatus;
  برائے (صحیح i = 0; i < N; i++) {
    wait(&xstatus);
    اگر (xstatus != 0) {
      لکھو("%s: fork in child failed", s);
      باہر_نکلو(1);
    }
  }
}

خالی
forkforkfork(حرف *s)
{
  unlink("stopforking");

  صحیح pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    جب (1) {
      صحیح fd = open("stopforking", 0);
      اگر (fd >= 0) {
        باہر_نکلو(0);
      }
      اگر (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    باہر_نکلو(0);
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
خالی
reparent2(حرف *s)
{
  برائے (صحیح i = 0; i < 800; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      لکھو("fork failed\n");
      باہر_نکلو(1);
    }
    اگر (pid1 == 0) {
      fork();
      fork();
      باہر_نکلو(0);
    }
    wait(0);
  }

  باہر_نکلو(0);
}

// allocate all mem, free it, and allocate again
خالی
mem(حرف *s)
{
  خالی *m1, *m2;
  صحیح pid;

  اگر ((pid = fork()) == 0) {
    m1 = 0;
    جب ((m2 = حافظہ_دو(10001)) != 0) {
      *(حرف **)m2 = m1;
      m1 = m2;
    }
    جب (m1) {
      m2 = *(حرف **)m1;
      آزاد(m1);
      m1 = m2;
    }
    m1 = حافظہ_دو(1024 * 20);
    اگر (m1 == 0) {
      لکھو("%s: couldn't allocate mem?!!\n", s);
      باہر_نکلو(1);
    }
    آزاد(m1);
    باہر_نکلو(0);
  } ورنہ {
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      باہر_نکلو(0);
    }
    باہر_نکلو(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
خالی
sharedfd(حرف *s)
{
  صحیح fd, pid, i, n, nc, np;
  شمار { N = 1000, SZ = 10 };
  حرف buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: cannot open sharedfd for writing", s);
    باہر_نکلو(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', حجم(buf));
  برائے (i = 0; i < N; i++) {
    اگر (اكتب(fd, buf, حجم(buf)) != حجم(buf)) {
      لکھو("%s: write sharedfd failed\n", s);
      باہر_نکلو(1);
    }
  }
  اگر (pid == 0) {
    باہر_نکلو(0);
  } ورنہ {
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != 0)
      باہر_نکلو(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  اگر (fd < 0) {
    لکھو("%s: cannot open sharedfd for reading\n", s);
    باہر_نکلو(1);
  }
  nc = np = 0;
  جب ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    برائے (i = 0; i < حجم(buf); i++) {
      اگر (buf[i] == 'c')
        nc++;
      اگر (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  اگر (nc == N * SZ && np == N * SZ) {
    باہر_نکلو(0);
  } ورنہ {
    لکھو("%s: nc/np test fails\n", s);
    باہر_نکلو(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
خالی
fourfiles(حرف *s)
{
  صحیح fd, pid, i, j, n, کل, pi;
  حرف *نام[] = {"f0", "f1", "f2", "f3"};
  حرف *fname;
  شمار { N = 12, NCHILD = 4, SZ = 500 };

  برائے (pi = 0; pi < NCHILD; pi++) {
    fname = نام[pi];
    unlink(fname);

    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }

    اگر (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      اگر (fd < 0) {
        لکھو("%s: create failed\n", s);
        باہر_نکلو(1);
      }

      memset(buf, '0' + pi, SZ);
      برائے (i = 0; i < N; i++) {
        اگر ((n = اكتب(fd, buf, SZ)) != SZ) {
          لکھو("write failed %d\n", n);
          باہر_نکلو(1);
        }
      }
      باہر_نکلو(0);
    }
  }

  صحیح xstatus;
  برائے (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      باہر_نکلو(xstatus);
  }

  برائے (i = 0; i < NCHILD; i++) {
    fname = نام[i];
    fd = open(fname, 0);
    کل = 0;
    جب ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
      برائے (j = 0; j < n; j++) {
        اگر (buf[j] != '0' + i) {
          لکھو("%s: wrong char\n", s);
          باہر_نکلو(1);
        }
      }
      کل += n;
    }
    close(fd);
    اگر (کل != N * SZ) {
      لکھو("wrong length %d\n", کل);
      باہر_نکلو(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
خالی
createdelete(حرف *s)
{
  شمار { N = 20, NCHILD = 4 };
  صحیح pid, i, fd, pi;
  حرف name[32];

  برائے (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }

    اگر (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      برائے (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        اگر (fd < 0) {
          لکھو("%s: create failed\n", s);
          باہر_نکلو(1);
        }
        close(fd);
        اگر (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          اگر (unlink(name) < 0) {
            لکھو("%s: unlink failed\n", s);
            باہر_نکلو(1);
          }
        }
      }
      باہر_نکلو(0);
    }
  }

  صحیح xstatus;
  برائے (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      باہر_نکلو(1);
  }

  name[0] = name[1] = name[2] = 0;
  برائے (i = 0; i < N; i++) {
    برائے (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      اگر ((i == 0 || i >= N / 2) && fd < 0) {
        لکھو("%s: oops createdelete %s didn't exist\n", s, name);
        باہر_نکلو(1);
      } ورنہ اگر ((i >= 1 && i < N / 2) && fd >= 0) {
        لکھو("%s: oops createdelete %s did exist\n", s, name);
        باہر_نکلو(1);
      }
      اگر (fd >= 0)
        close(fd);
    }
  }

  برائے (i = 0; i < N; i++) {
    برائے (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
خالی
unlinkread(حرف *s)
{
  شمار { SZ = 5 };
  صحیح fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: create unlinkread failed\n", s);
    باہر_نکلو(1);
  }
  اكتب(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: open unlinkread failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("unlinkread") != 0) {
    لکھو("%s: unlink unlinkread failed\n", s);
    باہر_نکلو(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  اكتب(fd1, "yyy", 3);
  close(fd1);

  اگر (اقرأ(fd, buf, حجم(buf)) != SZ) {
    لکھو("%s: unlinkread read failed", s);
    باہر_نکلو(1);
  }
  اگر (buf[0] != 'h') {
    لکھو("%s: unlinkread wrong data\n", s);
    باہر_نکلو(1);
  }
  اگر (اكتب(fd, buf, 10) != 10) {
    لکھو("%s: unlinkread write failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);
  unlink("unlinkread");
}

خالی
linktest(حرف *s)
{
  شمار { SZ = 5 };
  صحیح fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: create lf1 failed\n", s);
    باہر_نکلو(1);
  }
  اگر (اكتب(fd, "hello", SZ) != SZ) {
    لکھو("%s: write lf1 failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (link("lf1", "lf2") < 0) {
    لکھو("%s: link lf1 lf2 failed\n", s);
    باہر_نکلو(1);
  }
  unlink("lf1");

  اگر (open("lf1", 0) >= 0) {
    لکھو("%s: unlinked lf1 but it is still there!\n", s);
    باہر_نکلو(1);
  }

  fd = open("lf2", 0);
  اگر (fd < 0) {
    لکھو("%s: open lf2 failed\n", s);
    باہر_نکلو(1);
  }
  اگر (اقرأ(fd, buf, حجم(buf)) != SZ) {
    لکھو("%s: read lf2 failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (link("lf2", "lf2") >= 0) {
    لکھو("%s: link lf2 lf2 succeeded! oops\n", s);
    باہر_نکلو(1);
  }

  unlink("lf2");
  اگر (link("lf2", "lf1") >= 0) {
    لکھو("%s: link non-existent succeeded! oops\n", s);
    باہر_نکلو(1);
  }

  اگر (link(".", "lf1") >= 0) {
    لکھو("%s: link . lf1 succeeded! oops\n", s);
    باہر_نکلو(1);
  }
}

// test concurrent create/link/unlink of the same file
خالی
concreate(حرف *s)
{
  شمار { N = 40 };
  حرف file[3];
  صحیح i, pid, n, fd;
  حرف fa[N];
  ساخت {
    ushort inum;
    حرف name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  برائے (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    اگر (pid && (i % 3) == 1) {
      link("C0", file);
    } ورنہ اگر (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ورنہ {
      fd = open(file, O_CREATE | O_RDWR);
      اگر (fd < 0) {
        لکھو("concreate create %s failed\n", file);
        باہر_نکلو(1);
      }
      close(fd);
    }
    اگر (pid == 0) {
      باہر_نکلو(0);
    } ورنہ {
      صحیح xstatus;
      wait(&xstatus);
      اگر (xstatus != 0)
        باہر_نکلو(1);
    }
  }

  memset(fa, 0, حجم(fa));
  fd = open(".", 0);
  n = 0;
  جب (اقرأ(fd, &de, حجم(de)) > 0) {
    اگر (de.inum == 0)
      جاری;
    اگر (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      اگر (i < 0 || i >= حجم(fa)) {
        لکھو("%s: concreate weird file %s\n", s, de.name);
        باہر_نکلو(1);
      }
      اگر (fa[i]) {
        لکھو("%s: concreate duplicate file %s\n", s, de.name);
        باہر_نکلو(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  اگر (n != N) {
    لکھو("%s: concreate not enough files in directory listing\n", s);
    باہر_نکلو(1);
  }

  برائے (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ورنہ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    اگر (pid == 0)
      باہر_نکلو(0);
    ورنہ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
خالی
linkunlink(حرف *s)
{
  صحیح pid, i;

  unlink("x");
  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }

  غیرعلامتی صحیح x = (pid ? 1 : 97);
  برائے (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    اگر ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ورنہ اگر ((x % 3) == 1) {
      link("cat", "x");
    } ورنہ {
      unlink("x");
    }
  }

  اگر (pid)
    wait(0);
  ورنہ
    باہر_نکلو(0);
}

خالی
subdir(حرف *s)
{
  صحیح fd, cc;

  unlink("ff");
  اگر (mkdir("dd") != 0) {
    لکھو("%s: mkdir dd failed\n", s);
    باہر_نکلو(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: create dd/ff failed\n", s);
    باہر_نکلو(1);
  }
  اكتب(fd, "ff", 2);
  close(fd);

  اگر (unlink("dd") >= 0) {
    لکھو("%s: unlink dd (non-empty dir) succeeded!\n", s);
    باہر_نکلو(1);
  }

  اگر (mkdir("/dd/dd") != 0) {
    لکھو("%s: subdir mkdir dd/dd failed\n", s);
    باہر_نکلو(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: create dd/dd/ff failed\n", s);
    باہر_نکلو(1);
  }
  اكتب(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  اگر (fd < 0) {
    لکھو("%s: open dd/dd/../ff failed\n", s);
    باہر_نکلو(1);
  }
  cc = اقرأ(fd, buf, حجم(buf));
  اگر (cc != 2 || buf[0] != 'f') {
    لکھو("%s: dd/dd/../ff wrong content\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    لکھو("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    باہر_نکلو(1);
  }

  اگر (unlink("dd/dd/ff") != 0) {
    لکھو("%s: unlink dd/dd/ff failed\n", s);
    باہر_نکلو(1);
  }
  اگر (open("dd/dd/ff", O_RDONLY) >= 0) {
    لکھو("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    باہر_نکلو(1);
  }

  اگر (chdir("dd") != 0) {
    لکھو("%s: chdir dd failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("dd/../../dd") != 0) {
    لکھو("%s: chdir dd/../../dd failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("dd/../../../dd") != 0) {
    لکھو("%s: chdir dd/../../../dd failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("./..") != 0) {
    لکھو("%s: chdir ./.. failed\n", s);
    باہر_نکلو(1);
  }

  fd = open("dd/dd/ffff", 0);
  اگر (fd < 0) {
    لکھو("%s: open dd/dd/ffff failed\n", s);
    باہر_نکلو(1);
  }
  اگر (اقرأ(fd, buf, حجم(buf)) != 2) {
    لکھو("%s: read dd/dd/ffff wrong len\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (open("dd/dd/ff", O_RDONLY) >= 0) {
    لکھو("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    باہر_نکلو(1);
  }

  اگر (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    لکھو("%s: create dd/ff/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    لکھو("%s: create dd/xx/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (open("dd", O_CREATE) >= 0) {
    لکھو("%s: create dd succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (open("dd", O_RDWR) >= 0) {
    لکھو("%s: open dd rdwr succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (open("dd", O_WRONLY) >= 0) {
    لکھو("%s: open dd wronly succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (link("dd/ff/ff", "dd/dd/xx") == 0) {
    لکھو("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (link("dd/xx/ff", "dd/dd/xx") == 0) {
    لکھو("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (link("dd/ff", "dd/dd/ffff") == 0) {
    لکھو("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("dd/ff/ff") == 0) {
    لکھو("%s: mkdir dd/ff/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("dd/xx/ff") == 0) {
    لکھو("%s: mkdir dd/xx/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("dd/dd/ffff") == 0) {
    لکھو("%s: mkdir dd/dd/ffff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd/xx/ff") == 0) {
    لکھو("%s: unlink dd/xx/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd/ff/ff") == 0) {
    لکھو("%s: unlink dd/ff/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("dd/ff") == 0) {
    لکھو("%s: chdir dd/ff succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("dd/xx") == 0) {
    لکھو("%s: chdir dd/xx succeeded!\n", s);
    باہر_نکلو(1);
  }

  اگر (unlink("dd/dd/ffff") != 0) {
    لکھو("%s: unlink dd/dd/ff failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd/ff") != 0) {
    لکھو("%s: unlink dd/ff failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd") == 0) {
    لکھو("%s: unlink non-empty dd succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd/dd") < 0) {
    لکھو("%s: unlink dd/dd failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dd") < 0) {
    لکھو("%s: unlink dd failed\n", s);
    باہر_نکلو(1);
  }
}

// test writes that are larger than the log.
خالی
bigwrite(حرف *s)
{
  صحیح fd, sz;

  unlink("bigwrite");
  برائے (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    اگر (fd < 0) {
      لکھو("%s: cannot create bigwrite\n", s);
      باہر_نکلو(1);
    }
    صحیح i;
    برائے (i = 0; i < 2; i++) {
      صحیح cc = اكتب(fd, buf, sz);
      اگر (cc != sz) {
        لکھو("%s: write(%d) ret %d\n", s, sz, cc);
        باہر_نکلو(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

خالی
bigfile(حرف *s)
{
  شمار { N = 20, SZ = 600 };
  صحیح fd, i, کل, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: cannot create bigfile", s);
    باہر_نکلو(1);
  }
  برائے (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    اگر (اكتب(fd, buf, SZ) != SZ) {
      لکھو("%s: write bigfile failed\n", s);
      باہر_نکلو(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  اگر (fd < 0) {
    لکھو("%s: cannot open bigfile\n", s);
    باہر_نکلو(1);
  }
  کل = 0;
  برائے (i = 0;; i++) {
    cc = اقرأ(fd, buf, SZ / 2);
    اگر (cc < 0) {
      لکھو("%s: read bigfile failed\n", s);
      باہر_نکلو(1);
    }
    اگر (cc == 0)
      توڑو;
    اگر (cc != SZ / 2) {
      لکھو("%s: short read bigfile\n", s);
      باہر_نکلو(1);
    }
    اگر (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      لکھو("%s: read bigfile wrong data\n", s);
      باہر_نکلو(1);
    }
    کل += cc;
  }
  close(fd);
  اگر (کل != N * SZ) {
    لکھو("%s: read bigfile wrong total\n", s);
    باہر_نکلو(1);
  }
  unlink("bigfile.dat");
}

خالی
fourteen(حرف *s)
{
  صحیح fd;

  // DIRSIZ is 14.

  اگر (mkdir("12345678901234") != 0) {
    لکھو("%s: mkdir 12345678901234 failed\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("12345678901234/123456789012345") != 0) {
    لکھو("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    باہر_نکلو(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  اگر (fd < 0) {
    لکھو(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  اگر (fd < 0) {
    لکھو("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  اگر (mkdir("12345678901234/12345678901234") == 0) {
    لکھو("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("123456789012345/12345678901234") == 0) {
    لکھو("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    باہر_نکلو(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

خالی
rmdot(حرف *s)
{
  اگر (mkdir("dots") != 0) {
    لکھو("%s: mkdir dots failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("dots") != 0) {
    لکھو("%s: chdir dots failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink(".") == 0) {
    لکھو("%s: rm . worked!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("..") == 0) {
    لکھو("%s: rm .. worked!\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("/") != 0) {
    لکھو("%s: chdir / failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dots/.") == 0) {
    لکھو("%s: unlink dots/. worked!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dots/..") == 0) {
    لکھو("%s: unlink dots/.. worked!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dots") != 0) {
    لکھو("%s: unlink dots failed!\n", s);
    باہر_نکلو(1);
  }
}

خالی
dirfile(حرف *s)
{
  صحیح fd;

  fd = open("dirfile", O_CREATE);
  اگر (fd < 0) {
    لکھو("%s: create dirfile failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);
  اگر (chdir("dirfile") == 0) {
    لکھو("%s: chdir dirfile succeeded!\n", s);
    باہر_نکلو(1);
  }
  fd = open("dirfile/xx", 0);
  اگر (fd >= 0) {
    لکھو("%s: create dirfile/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  اگر (fd >= 0) {
    لکھو("%s: create dirfile/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("dirfile/xx") == 0) {
    لکھو("%s: mkdir dirfile/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dirfile/xx") == 0) {
    لکھو("%s: unlink dirfile/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (link("README", "dirfile/xx") == 0) {
    لکھو("%s: link to dirfile/xx succeeded!\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("dirfile") != 0) {
    لکھو("%s: unlink dirfile failed!\n", s);
    باہر_نکلو(1);
  }

  fd = open(".", O_RDWR);
  اگر (fd >= 0) {
    لکھو("%s: open . for writing succeeded!\n", s);
    باہر_نکلو(1);
  }
  fd = open(".", 0);
  اگر (اكتب(fd, "x", 1) > 0) {
    لکھو("%s: write . succeeded!\n", s);
    باہر_نکلو(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
خالی
iref(حرف *s)
{
  صحیح i, fd;

  برائے (i = 0; i < NINODE + 1; i++) {
    اگر (mkdir("irefd") != 0) {
      لکھو("%s: mkdir irefd failed\n", s);
      باہر_نکلو(1);
    }
    اگر (chdir("irefd") != 0) {
      لکھو("%s: chdir irefd failed\n", s);
      باہر_نکلو(1);
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
  برائے (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
خالی
forktest(حرف *s)
{
  شمار { N = 1000 };
  صحیح n, pid;

  برائے (n = 0; n < N; n++) {
    pid = fork();
    اگر (pid < 0)
      توڑو;
    اگر (pid == 0)
      باہر_نکلو(0);
  }

  اگر (n == 0) {
    لکھو("%s: no fork at all!\n", s);
    باہر_نکلو(1);
  }

  اگر (n == N) {
    لکھو("%s: fork claimed to work 1000 times!\n", s);
    باہر_نکلو(1);
  }

  برائے (; n > 0; n--) {
    اگر (wait(0) < 0) {
      لکھو("%s: wait stopped early\n", s);
      باہر_نکلو(1);
    }
  }

  اگر (wait(0) != -1) {
    لکھو("%s: wait got too many\n", s);
    باہر_نکلو(1);
  }
}

خالی
sbrkbasic(حرف *s)
{
  شمار { TOOMUCH = 1024 * 1024 * 1024 };
  صحیح i, pid, xstatus;
  حرف *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  اگر (pid < 0) {
    لکھو("fork failed in sbrkbasic\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    a = sbrk(TOOMUCH);
    اگر (a == (حرف *)SBRK_ERROR) {
      // it's OK if this fails.
      باہر_نکلو(0);
    }

    برائے (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    باہر_نکلو(1);
  }

  wait(&xstatus);
  اگر (xstatus == 1) {
    لکھو("%s: too much memory allocated!\n", s);
    باہر_نکلو(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  برائے (i = 0; i < 5000; i++) {
    b = sbrk(1);
    اگر (b != a) {
      لکھو("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      باہر_نکلو(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: sbrk test fork failed\n", s);
    باہر_نکلو(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  اگر (c != a + 1) {
    لکھو("%s: sbrk test failed post-fork\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0)
    باہر_نکلو(0);
  wait(&xstatus);
  باہر_نکلو(xstatus);
}

خالی
sbrkmuch(حرف *s)
{
  شمار { BIG = 100 * 1024 * 1024 };
  حرف *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  اگر (p != a) {
    لکھو("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    باہر_نکلو(1);
  }

  lastaddr = (حرف *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  اگر (c == (حرف *)SBRK_ERROR) {
    لکھو("%s: sbrk could not deallocate\n", s);
    باہر_نکلو(1);
  }
  c = sbrk(0);
  اگر (c != a - PGSIZE) {
    لکھو("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    باہر_نکلو(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  اگر (c != a || sbrk(0) != a + PGSIZE) {
    لکھو("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    باہر_نکلو(1);
  }
  اگر (*lastaddr == 99) {
    // should be zero
    لکھو("%s: sbrk de-allocation didn't really deallocate\n", s);
    باہر_نکلو(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  اگر (c != a) {
    لکھو("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    باہر_نکلو(1);
  }
}

// can we read the kernel's memory?
خالی
kernmem(حرف *s)
{
  حرف *a;
  صحیح pid;

  برائے (a = (حرف *)(KERNBASE); a < (حرف *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid == 0) {
      لکھو("%s: oops could read %p = %x\n", s, a, *a);
      باہر_نکلو(1);
    }
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != -1) // did kernel kill child?
      باہر_نکلو(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
خالی
MAXVAplus(حرف *s)
{
  متلون uint64 a = MAXVA;
  برائے (; a != 0; a <<= 1) {
    صحیح pid;
    pid = fork();
    اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    اگر (pid == 0) {
      *(حرف *)a = 99;
      لکھو("%s: oops wrote %p\n", s, (خالی *)a);
      باہر_نکلو(1);
    }
    صحیح xstatus;
    wait(&xstatus);
    اگر (xstatus != -1) // did kernel kill child?
      باہر_نکلو(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
خالی
sbrkfail(حرف *s)
{
  شمار { BIG = 100 * 1024 * 1024 };
  صحیح i, xstatus;
  صحیح fds[2];
  حرف scratch;
  حرف *c, *a;
  صحیح pids[10];
  صحیح pid;
  صحیح failed;

  failed = 0;
  اگر (pipe(fds) != 0) {
    لکھو("%s: pipe() failed\n", s);
    باہر_نکلو(1);
  }
  برائے (i = 0; i < حجم(pids) / حجم(pids[0]); i++) {
    اگر ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      اگر (sbrk(BIG - (uint64)sbrk(0)) == (حرف *)SBRK_ERROR)
        اكتب(fds[1], "0", 1);
      ورنہ
        اكتب(fds[1], "1", 1);
      // sit around until killed
      برائے (;;)
        pause(1000);
    }
    اگر (pids[i] != -1) {
      اقرأ(fds[0], &scratch, 1);
      اگر (scratch == '0')
        failed = 1;
    }
  }
  اگر (!failed) {
    لکھو("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  برائے (i = 0; i < حجم(pids) / حجم(pids[0]); i++) {
    اگر (pids[i] == -1)
      جاری;
    kill(pids[i]);
    wait(0);
  }
  اگر (c == (حرف *)SBRK_ERROR) {
    لکھو("%s: failed sbrk leaked memory\n", s);
    باہر_نکلو(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    اگر (a == (حرف *)SBRK_ERROR) {
      باہر_نکلو(0);
    }
    لکھو("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    باہر_نکلو(1);
  }
  wait(&xstatus);
  اگر (xstatus != 0)
    باہر_نکلو(1);
}

// test reads/writes from/to allocated memory
خالی
sbrkarg(حرف *s)
{
  حرف *a;
  صحیح fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  اگر (fd < 0) {
    لکھو("%s: open sbrk failed\n", s);
    باہر_نکلو(1);
  }
  اگر ((n = اكتب(fd, a, PGSIZE)) < 0) {
    لکھو("%s: write sbrk failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  اگر (pipe((صحیح *)a) != 0) {
    لکھو("%s: pipe() failed\n", s);
    باہر_نکلو(1);
  }
}

خالی
validatetest(حرف *s)
{
  صحیح hi;
  uint64 p;

  hi = 1100 * 1024;
  برائے (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    اگر (link("nosuchfile", (حرف *)p) != -1) {
      لکھو("%s: link should not succeed\n", s);
      باہر_نکلو(1);
    }
  }
}

// does uninitialized data start out zero?
حرف uninit[10000];
خالی
bsstest(حرف *s)
{
  صحیح i;

  برائے (i = 0; i < حجم(uninit); i++) {
    اگر (uninit[i] != '\0') {
      لکھو("%s: bss test failed\n", s);
      باہر_نکلو(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
خالی
bigargtest(حرف *s)
{
  صحیح pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  اگر (pid == 0) {
    ساکن حرف *دلائل[MAXARG];
    صحیح i;
    حرف big[400];
    memset(big, ' ', حجم(big));
    big[حجم(big) - 1] = '\0';
    برائے (i = 0; i < MAXARG - 1; i++)
      دلائل[i] = big;
    دلائل[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", دلائل);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    باہر_نکلو(0);
  } ورنہ اگر (pid < 0) {
    لکھو("%s: bigargtest: fork failed\n", s);
    باہر_نکلو(1);
  }

  wait(&xstatus);
  اگر (xstatus != 0)
    باہر_نکلو(xstatus);
  fd = open("bigarg-ok", 0);
  اگر (fd < 0) {
    لکھو("%s: bigarg test failed!\n", s);
    باہر_نکلو(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
خالی
fsfull()
{
  صحیح nfiles;
  صحیح fsblocks = 0;

  لکھو("fsfull test\n");

  برائے (nfiles = 0;; nfiles++) {
    حرف name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    لکھو("writing %s\n", name);
    صحیح fd = open(name, O_CREATE | O_RDWR);
    اگر (fd < 0) {
      لکھو("open %s failed\n", name);
      توڑو;
    }
    صحیح کل = 0;
    جب (1) {
      صحیح cc = اكتب(fd, buf, BSIZE);
      اگر (cc < BSIZE)
        توڑو;
      کل += cc;
      fsblocks++;
    }
    لکھو("wrote %d bytes\n", کل);
    close(fd);
    اگر (کل == 0)
      توڑو;
  }

  جب (nfiles >= 0) {
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

  لکھو("fsfull test finished, %d blocks\n", fsblocks);
}

خالی
argptest(حرف *s)
{
  صحیح fd;
  fd = open("init", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: open failed\n", s);
    باہر_نکلو(1);
  }
  اقرأ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
خالی
stacktest(حرف *s)
{
  صحیح pid;
  صحیح xstatus;

  pid = fork();
  اگر (pid == 0) {
    حرف *sp = (حرف *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    لکھو("%s: stacktest: read below stack %d\n", s, *sp);
    باہر_نکلو(1);
  } ورنہ اگر (pid < 0) {
    لکھو("%s: fork failed\n", s);
    باہر_نکلو(1);
  }
  wait(&xstatus);
  اگر (xstatus == -1) // kernel killed child?
    باہر_نکلو(0);
  ورنہ
    باہر_نکلو(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
خالی
nowrite(حرف *s)
{
  صحیح pid;
  صحیح xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  برائے (صحیح ai = 0; ai < حجم(addrs) / حجم(addrs[0]); ai++) {
    pid = fork();
    اگر (pid == 0) {
      متلون صحیح *addr = (صحیح *)addrs[ai];
      *addr = 10;
      لکھو("%s: write to %p did not fail!\n", s, addr);
      باہر_نکلو(0);
    } ورنہ اگر (pid < 0) {
      لکھو("%s: fork failed\n", s);
      باہر_نکلو(1);
    }
    wait(&xstatus);
    اگر (xstatus == 0) {
      // kernel did not kill child!
      باہر_نکلو(1);
    }
  }
  باہر_نکلو(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
خالی *big = (خالی *)0xeaeb0b5b00002f5e;
خالی
pgbug(حرف *s)
{
  حرف *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  باہر_نکلو(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
خالی
sbrkbugs(حرف *s)
{
  صحیح pid = fork();
  اگر (pid < 0) {
    لکھو("fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    صحیح sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    باہر_نکلو(0);
  }
  wait(0);

  pid = fork();
  اگر (pid < 0) {
    لکھو("fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    صحیح sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    باہر_نکلو(0);
  }
  wait(0);

  pid = fork();
  اگر (pid < 0) {
    لکھو("fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    باہر_نکلو(0);
  }
  wait(0);

  باہر_نکلو(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
خالی
sbrklast(حرف *s)
{
  uint64 چوٹی = (uint64)sbrk(0);
  اگر ((چوٹی % PGSIZE) != 0)
    sbrk(PGSIZE - (چوٹی % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  چوٹی = (uint64)sbrk(0);
  حرف *p = (حرف *)(چوٹی - 64);
  p[0] = 'x';
  p[1] = '\0';
  صحیح fd = open(p, O_RDWR | O_CREATE);
  اكتب(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  اقرأ(fd, p, 1);
  اگر (p[0] != 'x')
    باہر_نکلو(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
خالی
sbrk8000(حرف *s)
{
  sbrk(0x80000004);
  متلون حرف *چوٹی = sbrk(0);
  *(چوٹی - 1) = *(چوٹی - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
خالی
badarg(حرف *s)
{
  برائے (صحیح i = 0; i < 50000; i++) {
    حرف *argv[2];
    argv[0] = (حرف *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  باہر_نکلو(0);
}

#تعریف_کرو REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
خالی
lazy_alloc(حرف *s)
{
  حرف *i, *پچھلا_انتہا, *new_انتہا;

  پچھلا_انتہا = sbrklazy(REGION_SZ);
  اگر (پچھلا_انتہا == (حرف *)SBRK_ERROR) {
    لکھو("sbrklazy() failed\n");
    باہر_نکلو(1);
  }
  new_انتہا = پچھلا_انتہا + REGION_SZ;

  برائے (i = پچھلا_انتہا + PGSIZE; i < new_انتہا; i += 64 * PGSIZE)
    *(حرف **)i = i;

  برائے (i = پچھلا_انتہا + PGSIZE; i < new_انتہا; i += 64 * PGSIZE) {
    اگر (*(حرف **)i != i) {
      لکھو("failed to read value from memory\n");
      باہر_نکلو(1);
    }
  }

  باہر_نکلو(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
خالی
lazy_unmap(حرف *s)
{
  صحیح pid;
  حرف *i, *پچھلا_انتہا, *new_انتہا;

  پچھلا_انتہا = sbrklazy(REGION_SZ);
  اگر (پچھلا_انتہا == (حرف *)SBRK_ERROR) {
    لکھو("sbrklazy() failed\n");
    باہر_نکلو(1);
  }
  new_انتہا = پچھلا_انتہا + REGION_SZ;

  برائے (i = پچھلا_انتہا + PGSIZE; i < new_انتہا; i += PGSIZE * PGSIZE)
    *(حرف **)i = i;

  برائے (i = پچھلا_انتہا + PGSIZE; i < new_انتہا; i += PGSIZE * PGSIZE) {
    pid = fork();
    اگر (pid < 0) {
      لکھو("error forking\n");
      باہر_نکلو(1);
    } ورنہ اگر (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(حرف **)i = i;
      باہر_نکلو(0);
    } ورنہ {
      صحیح حالت_2;
      wait(&حالت_2);
      اگر (حالت_2 == 0) {
        لکھو("memory not unmapped\n");
        باہر_نکلو(1);
      }
    }
  }

  باہر_نکلو(0);
}

خالی
lazy_copy(حرف *s)
{
  // copyinstr on lazy page
  {
    حرف *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    خالی *xx = sbrk(0);
    خالی *ret = sbrk(-(((uint64)xx) + 1));
    اگر (ret != xx) {
      لکھو("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      باہر_نکلو(1);
    }
  }

  // read() and write() to these addresses should fail.
  غیرعلامتی طویل bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  برائے (صحیح i = 0; i < حجم(bad) / حجم(bad[0]); i++) {
    صحیح fd = open("README", 0);
    اگر (fd < 0) {
      لکھو("cannot open README\n");
      باہر_نکلو(1);
    }
    اگر (اقرأ(fd, (حرف *)bad[i], 512) >= 0) {
      لکھو("read succeeded\n");
      باہر_نکلو(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      لکھو("cannot open junk\n");
      باہر_نکلو(1);
    }
    اگر (اكتب(fd, (حرف *)bad[i], 512) >= 0) {
      لکھو("write succeeded\n");
      باہر_نکلو(1);
    }
    close(fd);
  }

  باہر_نکلو(0);
}

خالی
lazy_copyinstr(حرف *s)
{
  حرف *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  اگر ((uint64)p % PGSIZE != 0) {
    لکھو("%s: sbrk did not align\n", s);
    باہر_نکلو(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  صحیح fd = open(&p[4095], O_RDONLY);
  اگر (fd < 0) {
    لکھو("could not open /");
    باہر_نکلو(1);
  }

  ساخت stat st;
  صحیح r = fstat(fd, &st);
  اگر (r < 0) {
    لکھو("could not stat /");
    باہر_نکلو(1);
  }

  اگر (st.type != T_DIR) {
    لکھو("/ is not T_DIR");
    باہر_نکلو(1);
  }

  close(fd);
}

خالی
lazy_sbrk(حرف *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  حرف *p = sbrk(0);
  جب ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    اگر (p < 0) {
      لکھو("sbrklazy(%d) returned %p\n", 1 << 30, p);
      باہر_نکلو(1);
    }

    p = sbrklazy(0);
  }

  صحیح n = TRAPFRAME - PGSIZE - (uint64)p;

  حرف *p1 = sbrklazy(n);
  اگر (p1 < 0 || p1 != p) {
    لکھو("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    باہر_نکلو(1);
  }

  p = sbrk(PGSIZE);
  اگر (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    لکھو("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    باہر_نکلو(1);
  }

  p[0] = 1;
  اگر (p[1] != 0) {
    لکھو("sbrk() returned non-zero-filled memory\n");
    باہر_نکلو(1);
  }

  p = sbrk(1);
  اگر ((uint64)p != -1) {
    لکھو("sbrk(1) returned %p, expected error\n", p);
    باہر_نکلو(1);
  }

  p = sbrklazy(1);
  اگر ((uint64)p != -1) {
    لکھو("sbrklazy(1) returned %p, expected error\n", p);
    باہر_نکلو(1);
  }

  باہر_نکلو(0);
}

خالی
partial_write(حرف *s)
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
    لکھو("%s: cannot create testfile\n", s);
    باہر_نکلو(1);
  }

  صحیح cc = اكتب(fd, "A", 1);
  اگر (cc != 1) {
    لکھو("%s: could not write A\n", s);
    باہر_نکلو(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: cannot re-open testfile\n", s);
    باہر_نکلو(1);
  }

  حرف *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  اگر ((uint64)p % PGSIZE != 0) {
    لکھو("%s: sbrk did not align\n", s);
    باہر_نکلو(1);
  }

  p[-1] = 'X';

  cc = اكتب(fd, p - 1, 2);
  اگر (cc != -1) {
    لکھو("%s: write succeeded, should have failed\n", s);
    باہر_نکلو(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: cannot re-open testfile\n", s);
    باہر_نکلو(1);
  }

  حرف b;
  cc = اقرأ(fd, &b, 1);
  اگر (cc != 1) {
    لکھو("%s: cannot read testfile\n", s);
    باہر_نکلو(1);
  }

  close(fd);

  اگر (b != 'X') {
    لکھو("%s: read returned %c, expected X\n", s, b);
    باہر_نکلو(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  برائے (صحیح i = 0; i < 64; i++) {
    حرف buf[1024];
    memset(buf, 0, حجم(buf));
    cc = اكتب(fd, buf, حجم(buf));
    اگر (cc != حجم(buf)) {
      لکھو("%s: could not write to bigfile\n", s);
      باہر_نکلو(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  اگر (fd < 0) {
    لکھو("%s: cannot re-open testfile\n", s);
    باہر_نکلو(1);
  }

  cc = اقرأ(fd, &b, 1);
  اگر (cc != 1) {
    لکھو("%s: cannot read testfile\n", s);
    باہر_نکلو(1);
  }

  close(fd);

  اگر (b != 'X') {
    لکھو("%s: read returned %c, expected X\n", s, b);
    باہر_نکلو(1);
  }

  unlink("testfile");
}

خالی
unlinkcwd(حرف *s)
{
  اگر (mkdir("/a") < 0) {
    لکھو("%s: mkdir /a failed\n", s);
    باہر_نکلو(1);
  }
  اگر (mkdir("/a/b") < 0) {
    لکھو("%s: mkdir /a/b failed\n", s);
    باہر_نکلو(1);
  }
  اگر (chdir("/a/b") < 0) {
    لکھو("%s: chdir failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("/a/b") < 0) {
    لکھو("%s: unlink /a/b failed\n", s);
    باہر_نکلو(1);
  }
  اگر (unlink("/a") < 0) {
    لکھو("%s: unlink /a failed\n", s);
    باہر_نکلو(1);
  }
  اگر (open("../", O_RDONLY) > 0) {
    لکھو("%s: open ../ non-existing directory\n", s);
  }
  اگر (open("../c", O_CREATE) > 0) {
    لکھو("%s: create ../c non-existing file\n", s);
  }
}

ساخت test {
  خالی (*f)(حرف *);
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
خالی
bigdir(حرف *s)
{
  شمار { N = 500 };
  صحیح i, fd;
  حرف name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  اگر (fd < 0) {
    لکھو("%s: bigdir create failed\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  برائے (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    اگر (link("bd", name) != 0) {
      لکھو("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      باہر_نکلو(1);
    }
  }

  unlink("bd");
  برائے (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    اگر (unlink(name) != 0) {
      لکھو("%s: bigdir unlink failed", s);
      باہر_نکلو(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
خالی
manywrites(حرف *s)
{
  صحیح nchildren = 4;
  صحیح howmany = 30; // increase to look for deadlock

  برائے (صحیح ci = 0; ci < nchildren; ci++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      لکھو("fork failed\n");
      باہر_نکلو(1);
    }

    اگر (pid == 0) {
      حرف name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      برائے (صحیح iters = 0; iters < howmany; iters++) {
        برائے (صحیح i = 0; i < ci + 1; i++) {
          صحیح fd = open(name, O_CREATE | O_RDWR);
          اگر (fd < 0) {
            لکھو("%s: cannot create %s\n", s, name);
            باہر_نکلو(1);
          }
          صحیح sz = حجم(buf);
          صحیح cc = اكتب(fd, buf, sz);
          اگر (cc != sz) {
            لکھو("%s: write(%d) ret %d\n", s, sz, cc);
            باہر_نکلو(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      باہر_نکلو(0);
    }
  }

  برائے (صحیح ci = 0; ci < nchildren; ci++) {
    صحیح st = 0;
    wait(&st);
    اگر (st != 0)
      باہر_نکلو(st);
  }
  باہر_نکلو(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
خالی
badwrite(حرف *s)
{
  صحیح assumed_free = 600;

  unlink("junk");
  برائے (صحیح i = 0; i < assumed_free; i++) {
    صحیح fd = open("junk", O_CREATE | O_WRONLY);
    اگر (fd < 0) {
      لکھو("open junk failed\n");
      باہر_نکلو(1);
    }
    اكتب(fd, (حرف *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  صحیح fd = open("junk", O_CREATE | O_WRONLY);
  اگر (fd < 0) {
    لکھو("open junk failed\n");
    باہر_نکلو(1);
  }
  اگر (اكتب(fd, "x", 1) != 1) {
    لکھو("write failed\n");
    باہر_نکلو(1);
  }
  close(fd);
  unlink("junk");

  باہر_نکلو(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
خالی
execout(حرف *s)
{
  برائے (صحیح avail = 0; avail < 15; avail++) {
    صحیح pid = fork();
    اگر (pid < 0) {
      لکھو("fork failed\n");
      باہر_نکلو(1);
    } ورنہ اگر (pid == 0) {
      // allocate all of memory.
      جب (1) {
        حرف *a = sbrk(PGSIZE);
        اگر (a == SBRK_ERROR)
          توڑو;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      برائے (صحیح i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      حرف *دلائل[] = {"echo", "x", 0};
      exec("echo", دلائل);
      باہر_نکلو(0);
    } ورنہ {
      wait((صحیح *)0);
    }
  }

  باہر_نکلو(0);
}

// can the kernel tolerate running out of disk space?
خالی
diskfull(حرف *s)
{
  صحیح fi;
  صحیح مکمل = 0;

  unlink("diskfulldir");

  برائے (fi = 0; مکمل == 0 && '0' + fi < 0177; fi++) {
    حرف name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      لکھو("%s: could not create file %s\n", s, name);
      مکمل = 1;
      توڑو;
    }
    برائے (صحیح i = 0; i < MAXFILE; i++) {
      حرف buf[BSIZE];
      اگر (اكتب(fd, buf, BSIZE) != BSIZE) {
        مکمل = 1;
        close(fd);
        توڑو;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  صحیح nzz = 128;
  برائے (صحیح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0)
      توڑو;
    close(fd);
  }

  // this mkdir() is expected to fail.
  اگر (mkdir("diskfulldir") == 0)
    لکھو("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  برائے (صحیح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  برائے (صحیح i = 0; '0' + i < 0177; i++) {
    حرف name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

خالی
outofinodes(حرف *s)
{
  صحیح nzz = 32 * 32;
  برائے (صحیح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    صحیح fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    اگر (fd < 0) {
      // failure is eventually expected.
      توڑو;
    }
    close(fd);
  }

  برائے (صحیح i = 0; i < nzz; i++) {
    حرف name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

خالی
linkoverflow(حرف *s)
{
  شمار { ہدف_2 = 32768 };
  شمار { DIRS = 64 };
  ساخت stat st;
  صحیح i;

  unlink("/lof");
  صحیح fd = open("/lof", O_CREATE | O_RDWR);
  اگر (fd < 0) {
    لکھو("%s: cannot create /lof\n", s);
    باہر_نکلو(1);
  }
  close(fd);

  برائے (i = 0; i < ہدف_2; i++) {
    صحیح d = i % DIRS;
    صحیح f = i / DIRS;

    حرف pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    اگر (f == 0 && mkdir(pn) < 0) {
      لکھو("%s: mkdir(%s) failed\n", s, pn);
      باہر_نکلو(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    اگر (link("/lof", pn) < 0) {
      اگر (stat("/lof", &st) < 0) {
        لکھو("%s: stat(/lof) failed\n", s);
        باہر_نکلو(1);
      }
      اگر (st.nlink >= 32767) {
        // overflow check succeeded.
        توڑو;
      }
      لکھو("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      باہر_نکلو(1);
    }

    اگر (i % 100 == 0) {
      لکھو("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  اگر (stat("/lof", &st) < 0) {
    لکھو("%s: stat(/lof) failed\n", s);
    باہر_نکلو(1);
  }

  unlink("/lof");

  اگر (st.nlink < 0) {
    لکھو("%s: negative link count: %d\n", s, st.nlink);
    باہر_نکلو(1);
  }
}

ساخت test slowtests[] = {
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
تسلسل(خالی f(حرف *), حرف *s)
{
  صحیح pid;
  صحیح xstatus;

  لکھو("test %s: ", s);
  اگر ((pid = fork()) < 0) {
    لکھو("runtest: fork error\n");
    باہر_نکلو(1);
  }
  اگر (pid == 0) {
    f(s);
    باہر_نکلو(0);
  } ورنہ {
    wait(&xstatus);
    اگر (xstatus != 0)
      لکھو("FAILED\n");
    ورنہ
      لکھو("OK\n");
    واپس xstatus == 0;
  }
}

صحیح
runtests(ساخت test *tests, حرف *justone, صحیح continuous)
{
  صحیح ntests = 0;
  برائے (ساخت test *t = tests; t->s != 0; t++) {
    اگر ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      اگر (!تسلسل(t->f, t->s)) {
        اگر (continuous != 2) {
          لکھو("SOME TESTS FAILED\n");
          واپس -1;
        }
      }
    }
  }
  واپس ntests;
}

// use sbrk() to count how many free physical memory pages there are.
صحیح
countfree()
{
  صحیح n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  جب (1) {
    حرف *a = sbrk(PGSIZE);
    اگر (a == SBRK_ERROR) {
      توڑو;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  واپس n;
}

صحیح
drivetests(صحیح تیز, صحیح continuous, حرف *justone)
{
  کرو {
    لکھو("usertests starting\n");
    صحیح free0 = countfree();
    صحیح free1 = 0;
    صحیح ntests = 0;
    صحیح n;
    n = runtests(quicktests, justone, continuous);
    اگر (n < 0) {
      اگر (continuous != 2) {
        واپس 1;
      }
    } ورنہ {
      ntests += n;
    }
    اگر (!تیز) {
      اگر (justone == 0)
        لکھو("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      اگر (n < 0) {
        اگر (continuous != 2) {
          واپس 1;
        }
      } ورنہ {
        ntests += n;
      }
    }
    اگر ((free1 = countfree()) < free0) {
      لکھو("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      اگر (continuous != 2) {
        واپس 1;
      }
    }
    اگر (justone != 0 && ntests == 0) {
      لکھو("NO TESTS EXECUTED\n");
      واپس 1;
    }
  } جب (continuous);
  واپس 0;
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح continuous = 0;
  صحیح تیز = 0;
  حرف *justone = 0;

  اگر (argc == 2 && strcmp(argv[1], "-q") == 0) {
    تیز = 1;
  } ورنہ اگر (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ورنہ اگر (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ورنہ اگر (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ورنہ اگر (argc > 1) {
    لکھو("Usage: usertests [-c] [-C] [-q] [testname]\n");
    باہر_نکلو(1);
  }
  اگر (drivetests(تیز, continuous, justone)) {
    باہر_نکلو(1);
  }
  لکھو("ALL TESTS PASSED\n");
  باہر_نکلو(0);
}
