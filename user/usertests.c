#అన్తర్భావ "kernel/param.h"
#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "kernel/syscall.h"
#అన్తర్భావ "kernel/memlayout.h"
#అన్తర్భావ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#నిర్వచించు BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

అక్షరం buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
శూన్య
copyin(అక్షరం *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  కొరకు (పూర్ణాంకం ai = 0; ai < పరిమాణం(addrs) / పరిమాణం(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    పూర్ణాంకం fd = open("copyin1", O_CREATE | O_WRONLY);
    ఒకవేళ (fd < 0) {
      రాయి("open(copyin1) failed\n");
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం n = లిఖ(fd, (శూన్య *)addr, 8192);
    ఒకవేళ (n >= 0) {
      రాయి("write(fd, %p, 8192) returned %d, not -1\n", (శూన్య *)addr, n);
      నిష్క్రమించు(1);
    }
    close(fd);
    unlink("copyin1");

    n = లిఖ(1, (అక్షరం *)addr, 8192);
    ఒకవేళ (n > 0) {
      రాయి("write(1, %p, 8192) returned %d, not -1 or 0\n", (శూన్య *)addr, n);
      నిష్క్రమించు(1);
    }

    పూర్ణాంకం fds[2];
    ఒకవేళ (pipe(fds) < 0) {
      రాయి("pipe() failed\n");
      నిష్క్రమించు(1);
    }
    n = లిఖ(fds[1], (అక్షరం *)addr, 8192);
    ఒకవేళ (n > 0) {
      రాయి("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (శూన్య *)addr,
             n);
      నిష్క్రమించు(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
శూన్య
copyout(అక్షరం *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  కొరకు (పూర్ణాంకం ai = 0; ai < పరిమాణం(addrs) / పరిమాణం(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    పూర్ణాంకం fd = open("README", 0);
    ఒకవేళ (fd < 0) {
      రాయి("open(README) failed\n");
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం n = పఠ(fd, (శూన్య *)addr, 8192);
    ఒకవేళ (n > 0) {
      రాయి("read(fd, %p, 8192) returned %d, not -1 or 0\n", (శూన్య *)addr, n);
      నిష్క్రమించు(1);
    }
    close(fd);

    పూర్ణాంకం fds[2];
    ఒకవేళ (pipe(fds) < 0) {
      రాయి("pipe() failed\n");
      నిష్క్రమించు(1);
    }
    n = లిఖ(fds[1], "x", 1);
    ఒకవేళ (n != 1) {
      రాయి("pipe write failed\n");
      నిష్క్రమించు(1);
    }
    n = పఠ(fds[0], (శూన్య *)addr, 8192);
    ఒకవేళ (n > 0) {
      రాయి("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (శూన్య *)addr,
             n);
      నిష్క్రమించు(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
శూన్య
copyinstr1(అక్షరం *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  కొరకు (పూర్ణాంకం ai = 0; ai < పరిమాణం(addrs) / పరిమాణం(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    పూర్ణాంకం fd = open((అక్షరం *)addr, O_CREATE | O_WRONLY);
    ఒకవేళ (fd >= 0) {
      రాయి("open(%p) returned %d, not -1\n", (శూన్య *)addr, fd);
      నిష్క్రమించు(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
శూన్య
copyinstr2(అక్షరం *s)
{
  అక్షరం b[MAXPATH + 1];

  కొరకు (పూర్ణాంకం i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  పూర్ణాంకం ret = unlink(b);
  ఒకవేళ (ret != -1) {
    రాయి("unlink(%s) returned %d, not -1\n", b, ret);
    నిష్క్రమించు(1);
  }

  పూర్ణాంకం fd = open(b, O_CREATE | O_WRONLY);
  ఒకవేళ (fd != -1) {
    రాయి("open(%s) returned %d, not -1\n", b, fd);
    నిష్క్రమించు(1);
  }

  ret = link(b, b);
  ఒకవేళ (ret != -1) {
    రాయి("link(%s, %s) returned %d, not -1\n", b, b, ret);
    నిష్క్రమించు(1);
  }

  అక్షరం *వాదనలు[] = {"xx", 0};
  ret = exec(b, వాదనలు);
  ఒకవేళ (ret != -1) {
    రాయి("exec(%s) returned %d, not -1\n", b, fd);
    నిష్క్రమించు(1);
  }

  పూర్ణాంకం pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    స్థిర అక్షరం big[PGSIZE + 1];
    కొరకు (పూర్ణాంకం i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    అక్షరం *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ఒకవేళ (ret != -1) {
      రాయి("exec(echo, BIG) returned %d, not -1\n", fd);
      నిష్క్రమించు(1);
    }
    నిష్క్రమించు(747); // OK
  }

  పూర్ణాంకం st = 0;
  wait(&st);
  ఒకవేళ (st != 747) {
    రాయి("exec(echo, BIG) succeeded, should have failed\n");
    నిష్క్రమించు(1);
  }
}

// what if a string argument crosses over the end of last user page?
శూన్య
copyinstr3(అక్షరం *s)
{
  sbrk(8192);
  uint64 శిఖరం = (uint64)sbrk(0);
  ఒకవేళ ((శిఖరం % PGSIZE) != 0) {
    sbrk(PGSIZE - (శిఖరం % PGSIZE));
  }
  శిఖరం = (uint64)sbrk(0);
  ఒకవేళ (శిఖరం % PGSIZE) {
    రాయి("oops\n");
    నిష్క్రమించు(1);
  }

  అక్షరం *b = (అక్షరం *)(శిఖరం - 1);
  *b = 'x';

  పూర్ణాంకం ret = unlink(b);
  ఒకవేళ (ret != -1) {
    రాయి("unlink(%s) returned %d, not -1\n", b, ret);
    నిష్క్రమించు(1);
  }

  పూర్ణాంకం fd = open(b, O_CREATE | O_WRONLY);
  ఒకవేళ (fd != -1) {
    రాయి("open(%s) returned %d, not -1\n", b, fd);
    నిష్క్రమించు(1);
  }

  ret = link(b, b);
  ఒకవేళ (ret != -1) {
    రాయి("link(%s, %s) returned %d, not -1\n", b, b, ret);
    నిష్క్రమించు(1);
  }

  అక్షరం *వాదనలు[] = {"xx", 0};
  ret = exec(b, వాదనలు);
  ఒకవేళ (ret != -1) {
    రాయి("exec(%s) returned %d, not -1\n", b, fd);
    నిష్క్రమించు(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
శూన్య
rwsbrk(అక్షరం *s)
{
  పూర్ణాంకం fd, n;

  uint64 a = (uint64)sbrk(8192);

  ఒకవేళ (a == (uint64)SBRK_ERROR) {
    రాయి("sbrk(rwsbrk) failed\n");
    నిష్క్రమించు(1);
  }

  ఒకవేళ (sbrk(-8192) == SBRK_ERROR) {
    రాయి("sbrk(rwsbrk) shrink failed\n");
    నిష్క్రమించు(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ఒకవేళ (fd < 0) {
    రాయి("open(rwsbrk) failed\n");
    నిష్క్రమించు(1);
  }
  n = లిఖ(fd, (శూన్య *)(a + PGSIZE), 1024);
  ఒకవేళ (n >= 0) {
    రాయి("write(fd, %p, 1024) returned %d, not -1\n", (శూన్య *)a + PGSIZE, n);
    నిష్క్రమించు(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("open(README) failed\n");
    నిష్క్రమించు(1);
  }
  n = పఠ(fd, (శూన్య *)(a + PGSIZE), 10);
  ఒకవేళ (n >= 0) {
    రాయి("read(fd, %p, 10) returned %d, not -1\n", (శూన్య *)a + PGSIZE, n);
    నిష్క్రమించు(1);
  }
  close(fd);

  నిష్క్రమించు(0);
}

// test O_TRUNC.
శూన్య
truncate1(అక్షరం *s)
{
  అక్షరం buf[32];

  unlink("truncfile");
  పూర్ణాంకం fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  లిఖ(fd1, "abcd", 4);
  close(fd1);

  పూర్ణాంకం fd2 = open("truncfile", O_RDONLY);
  పూర్ణాంకం n = పఠ(fd2, buf, పరిమాణం(buf));
  ఒకవేళ (n != 4) {
    రాయి("%s: read %d bytes, wanted 4\n", s, n);
    నిష్క్రమించు(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  పూర్ణాంకం fd3 = open("truncfile", O_RDONLY);
  n = పఠ(fd3, buf, పరిమాణం(buf));
  ఒకవేళ (n != 0) {
    రాయి("aaa fd3=%d\n", fd3);
    రాయి("%s: read %d bytes, wanted 0\n", s, n);
    నిష్క్రమించు(1);
  }

  n = పఠ(fd2, buf, పరిమాణం(buf));
  ఒకవేళ (n != 0) {
    రాయి("bbb fd2=%d\n", fd2);
    రాయి("%s: read %d bytes, wanted 0\n", s, n);
    నిష్క్రమించు(1);
  }

  లిఖ(fd1, "abcdef", 6);

  n = పఠ(fd3, buf, పరిమాణం(buf));
  ఒకవేళ (n != 6) {
    రాయి("%s: read %d bytes, wanted 6\n", s, n);
    నిష్క్రమించు(1);
  }

  n = పఠ(fd2, buf, పరిమాణం(buf));
  ఒకవేళ (n != 2) {
    రాయి("%s: read %d bytes, wanted 2\n", s, n);
    నిష్క్రమించు(1);
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
శూన్య
truncate2(అక్షరం *s)
{
  unlink("truncfile");

  పూర్ణాంకం fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  లిఖ(fd1, "abcd", 4);

  పూర్ణాంకం fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  పూర్ణాంకం n = లిఖ(fd1, "x", 1);
  ఒకవేళ (n != -1) {
    రాయి("%s: write returned %d, expected -1\n", s, n);
    నిష్క్రమించు(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

శూన్య
truncate3(అక్షరం *s)
{
  పూర్ణాంకం pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (pid == 0) {
    కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
      అక్షరం buf[32];
      పూర్ణాంకం fd = open("truncfile", O_WRONLY);
      ఒకవేళ (fd < 0) {
        రాయి("%s: open failed\n", s);
        నిష్క్రమించు(1);
      }
      పూర్ణాంకం n = లిఖ(fd, "1234567890", 10);
      ఒకవేళ (n != 10) {
        రాయి("%s: write got %d, expected 10\n", s, n);
        నిష్క్రమించు(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      పఠ(fd, buf, పరిమాణం(buf));
      close(fd);
    }
    నిష్క్రమించు(0);
  }

  కొరకు (పూర్ణాంకం i = 0; i < 150; i++) {
    పూర్ణాంకం fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ఒకవేళ (fd < 0) {
      రాయి("%s: open failed\n", s);
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం n = లిఖ(fd, "xxx", 3);
    ఒకవేళ (n != 3) {
      రాయి("%s: write got %d, expected 3\n", s, n);
      నిష్క్రమించు(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  నిష్క్రమించు(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
శూన్య
iputtest(అక్షరం *s)
{
  ఒకవేళ (mkdir("iputdir") < 0) {
    రాయి("%s: mkdir failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("iputdir") < 0) {
    రాయి("%s: chdir iputdir failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("../iputdir") < 0) {
    రాయి("%s: unlink ../iputdir failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("/") < 0) {
    రాయి("%s: chdir / failed\n", s);
    నిష్క్రమించు(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
శూన్య
exitiputtest(అక్షరం *s)
{
  పూర్ణాంకం pid, xstatus;

  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    ఒకవేళ (mkdir("iputdir") < 0) {
      రాయి("%s: mkdir failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (chdir("iputdir") < 0) {
      రాయి("%s: child chdir failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (unlink("../iputdir") < 0) {
      రాయి("%s: unlink ../iputdir failed\n", s);
      నిష్క్రమించు(1);
    }
    నిష్క్రమించు(0);
  }
  wait(&xstatus);
  నిష్క్రమించు(xstatus);
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
శూన్య
openiputtest(అక్షరం *s)
{
  పూర్ణాంకం pid, xstatus;

  ఒకవేళ (mkdir("oidir") < 0) {
    రాయి("%s: mkdir oidir failed\n", s);
    నిష్క్రమించు(1);
  }
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    పూర్ణాంకం fd = open("oidir", O_RDWR);
    ఒకవేళ (fd >= 0) {
      రాయి("%s: open directory for write succeeded\n", s);
      నిష్క్రమించు(1);
    }
    నిష్క్రమించు(0);
  }
  pause(1);
  ఒకవేళ (unlink("oidir") != 0) {
    రాయి("%s: unlink failed\n", s);
    నిష్క్రమించు(1);
  }
  wait(&xstatus);
  నిష్క్రమించు(xstatus);
}

// simple file system tests

శూన్య
opentest(అక్షరం *s)
{
  పూర్ణాంకం fd;

  fd = open("echo", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open echo failed!\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ఒకవేళ (fd >= 0) {
    రాయి("%s: open doesnotexist succeeded!\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
writetest(అక్షరం *s)
{
  పూర్ణాంకం fd;
  పూర్ణాంకం i;
  గణన { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: error: creat small failed!\n", s);
    నిష్క్రమించు(1);
  }
  కొరకు (i = 0; i < N; i++) {
    ఒకవేళ (లిఖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      రాయి("%s: error: write aa %d new file failed\n", s, i);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (లిఖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      రాయి("%s: error: write bb %d new file failed\n", s, i);
      నిష్క్రమించు(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: error: open small failed!\n", s);
    నిష్క్రమించు(1);
  }
  i = పఠ(fd, buf, N * SZ * 2);
  ఒకవేళ (i != N * SZ * 2) {
    రాయి("%s: read failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (unlink("small") < 0) {
    రాయి("%s: unlink small failed\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
writebig(అక్షరం *s)
{
  పూర్ణాంకం i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: error: creat big failed!\n", s);
    నిష్క్రమించు(1);
  }

  కొరకు (i = 0; i < MAXFILE; i++) {
    ((పూర్ణాంకం *)buf)[0] = i;
    ఒకవేళ (లిఖ(fd, buf, BSIZE) != BSIZE) {
      రాయి("%s: error: write big file failed i=%d\n", s, i);
      నిష్క్రమించు(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: error: open big failed!\n", s);
    నిష్క్రమించు(1);
  }

  n = 0;
  కొరకు (;;) {
    i = పఠ(fd, buf, BSIZE);
    ఒకవేళ (i == 0) {
      ఒకవేళ (n != MAXFILE) {
        రాయి("%s: read only %d blocks from big", s, n);
        నిష్క్రమించు(1);
      }
      ఆపు;
    } లేకపోతే ఒకవేళ (i != BSIZE) {
      రాయి("%s: read failed %d\n", s, i);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (((పూర్ణాంకం *)buf)[0] != n) {
      రాయి("%s: read content of block %d is %d\n", s, n, ((పూర్ణాంకం *)buf)[0]);
      నిష్క్రమించు(1);
    }
    n++;
  }
  close(fd);
  ఒకవేళ (unlink("big") < 0) {
    రాయి("%s: unlink big failed\n", s);
    నిష్క్రమించు(1);
  }
}

// many creates, followed by unlink test
శూన్య
createtest(అక్షరం *s)
{
  పూర్ణాంకం i, fd;
  గణన { N = 52 };

  అక్షరం name[3];
  name[0] = 'a';
  name[2] = '\0';
  కొరకు (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  కొరకు (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

శూన్య
dirtest(అక్షరం *s)
{
  ఒకవేళ (mkdir("dir0") < 0) {
    రాయి("%s: mkdir failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (chdir("dir0") < 0) {
    రాయి("%s: chdir dir0 failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (chdir("..") < 0) {
    రాయి("%s: chdir .. failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (unlink("dir0") < 0) {
    రాయి("%s: unlink dir0 failed\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
exectest(అక్షరం *s)
{
  పూర్ణాంకం fd, xstatus, pid;
  అక్షరం *echoargv[] = {"echo", "OK", 0};
  అక్షరం buf[3];

  unlink("echo-ok");
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    పూర్ణాంకం errfd = dup(1);
    ఒకవేళ (errfd < 0) {
      రాయి("%s: dup failed\n", s);
      నిష్క్రమించు(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ఒకవేళ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      నిష్క్రమించు(1);
    }
    // won't get to here
  }
  ఒకవేళ (wait(&xstatus) != pid) {
    రాయి("%s: wait failed!\n", s);
  }
  ఒకవేళ (xstatus != 0) {
    రాయి("%s: nonzero wait status %d\n", s, xstatus);
    నిష్క్రమించు(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (పఠ(fd, buf, 2) != 2) {
    రాయి("%s: read failed\n", s);
    నిష్క్రమించు(1);
  }
  unlink("echo-ok");
  ఒకవేళ (buf[0] == 'O' && buf[1] == 'K')
    నిష్క్రమించు(0);
  లేకపోతే {
    రాయి("%s: wrong output\n", s);
    నిష్క్రమించు(1);
  }
}

// simple fork and pipe read/write

శూన్య
pipe1(అక్షరం *s)
{
  పూర్ణాంకం fds[2], pid, xstatus;
  పూర్ణాంకం seq, i, n, cc, మొత్తం;
  గణన { N = 5, SZ = 1033 };

  ఒకవేళ (pipe(fds) != 0) {
    రాయి("%s: pipe() failed\n", s);
    నిష్క్రమించు(1);
  }
  pid = fork();
  seq = 0;
  ఒకవేళ (pid == 0) {
    close(fds[0]);
    కొరకు (n = 0; n < N; n++) {
      కొరకు (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ఒకవేళ (లిఖ(fds[1], buf, SZ) != SZ) {
        రాయి("%s: pipe1 oops 1\n", s);
        నిష్క్రమించు(1);
      }
    }
    నిష్క్రమించు(0);
  } లేకపోతే ఒకవేళ (pid > 0) {
    close(fds[1]);
    మొత్తం = 0;
    cc = 1;
    అయితే ((n = పఠ(fds[0], buf, cc)) > 0) {
      కొరకు (i = 0; i < n; i++) {
        ఒకవేళ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          రాయి("%s: pipe1 oops 2\n", s);
          తిరిగి;
        }
      }
      మొత్తం += n;
      cc = cc * 2;
      ఒకవేళ (cc > పరిమాణం(buf))
        cc = పరిమాణం(buf);
    }
    ఒకవేళ (మొత్తం != N * SZ) {
      రాయి("%s: pipe1 oops 3 total %d\n", s, మొత్తం);
      నిష్క్రమించు(1);
    }
    close(fds[0]);
    wait(&xstatus);
    నిష్క్రమించు(xstatus);
  } లేకపోతే {
    రాయి("%s: fork() failed\n", s);
    నిష్క్రమించు(1);
  }
}

// test if child is killed (status = -1)
శూన్య
killstatus(అక్షరం *s)
{
  పూర్ణాంకం xst;

  కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
    పూర్ణాంకం pid1 = fork();
    ఒకవేళ (pid1 < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid1 == 0) {
      అయితే (1) {
        getpid();
      }
      నిష్క్రమించు(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ఒకవేళ (xst != -1) {
      రాయి("%s: status should be -1\n", s);
      నిష్క్రమించు(1);
    }
  }
  నిష్క్రమించు(0);
}

// test that kill on unused process 0 doesn't set the killed flag
శూన్య
killzero(అక్షరం *s)
{
  పూర్ణాంకం pid, xst;

  kill(0);

  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    నిష్క్రమించు(7);
  }
  ఒకవేళ (wait(&xst) != pid) {
    రాయి("%s: wait wrong pid\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (xst != 7) {
    రాయి("%s: child exited with status %d, expected 7\n", s, xst);
    నిష్క్రమించు(1);
  }
  నిష్క్రమించు(0);
}

// meant to be run w/ at most two CPUs
శూన్య
preempt(అక్షరం *s)
{
  పూర్ణాంకం pid1, pid2, pid3;
  పూర్ణాంకం pfds[2];

  pid1 = fork();
  ఒకవేళ (pid1 < 0) {
    రాయి("%s: fork failed", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid1 == 0)
    కొరకు (;;)
      ;

  pid2 = fork();
  ఒకవేళ (pid2 < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid2 == 0)
    కొరకు (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ఒకవేళ (pid3 < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid3 == 0) {
    close(pfds[0]);
    ఒకవేళ (లిఖ(pfds[1], "x", 1) != 1)
      రాయి("%s: preempt write error", s);
    close(pfds[1]);
    కొరకు (;;)
      ;
  }

  close(pfds[1]);
  ఒకవేళ (పఠ(pfds[0], buf, పరిమాణం(buf)) != 1) {
    రాయి("%s: preempt read error", s);
    తిరిగి;
  }
  close(pfds[0]);
  రాయి("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  రాయి("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
శూన్య
exitwait(అక్షరం *s)
{
  పూర్ణాంకం i, pid;

  కొరకు (i = 0; i < 100; i++) {
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid) {
      పూర్ణాంకం xstate;
      ఒకవేళ (wait(&xstate) != pid) {
        రాయి("%s: wait wrong pid\n", s);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (i != xstate) {
        రాయి("%s: wait wrong exit status\n", s);
        నిష్క్రమించు(1);
      }
    } లేకపోతే {
      నిష్క్రమించు(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
శూన్య
reparent(అక్షరం *s)
{
  పూర్ణాంకం master_pid = getpid();
  కొరకు (పూర్ణాంకం i = 0; i < 200; i++) {
    పూర్ణాంకం pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid) {
      ఒకవేళ (wait(0) != pid) {
        రాయి("%s: wait wrong pid\n", s);
        నిష్క్రమించు(1);
      }
    } లేకపోతే {
      పూర్ణాంకం pid2 = fork();
      ఒకవేళ (pid2 < 0) {
        kill(master_pid);
        నిష్క్రమించు(1);
      }
      నిష్క్రమించు(0);
    }
  }
  నిష్క్రమించు(0);
}

// what if two children exit() at the same time?
శూన్య
twochildren(అక్షరం *s)
{
  కొరకు (పూర్ణాంకం i = 0; i < 1000; i++) {
    పూర్ణాంకం pid1 = fork();
    ఒకవేళ (pid1 < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid1 == 0) {
      నిష్క్రమించు(0);
    } లేకపోతే {
      పూర్ణాంకం pid2 = fork();
      ఒకవేళ (pid2 < 0) {
        రాయి("%s: fork failed\n", s);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (pid2 == 0) {
        నిష్క్రమించు(0);
      } లేకపోతే {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
శూన్య
forkfork(అక్షరం *s)
{
  గణన { N = 2 };

  కొరకు (పూర్ణాంకం i = 0; i < N; i++) {
    పూర్ణాంకం pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid == 0) {
      కొరకు (పూర్ణాంకం j = 0; j < 200; j++) {
        పూర్ణాంకం pid1 = fork();
        ఒకవేళ (pid1 < 0) {
          నిష్క్రమించు(1);
        }
        ఒకవేళ (pid1 == 0) {
          నిష్క్రమించు(0);
        }
        wait(0);
      }
      నిష్క్రమించు(0);
    }
  }

  పూర్ణాంకం xstatus;
  కొరకు (పూర్ణాంకం i = 0; i < N; i++) {
    wait(&xstatus);
    ఒకవేళ (xstatus != 0) {
      రాయి("%s: fork in child failed", s);
      నిష్క్రమించు(1);
    }
  }
}

శూన్య
forkforkfork(అక్షరం *s)
{
  unlink("stopforking");

  పూర్ణాంకం pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    అయితే (1) {
      పూర్ణాంకం fd = open("stopforking", 0);
      ఒకవేళ (fd >= 0) {
        నిష్క్రమించు(0);
      }
      ఒకవేళ (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    నిష్క్రమించు(0);
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
శూన్య
reparent2(అక్షరం *s)
{
  కొరకు (పూర్ణాంకం i = 0; i < 800; i++) {
    పూర్ణాంకం pid1 = fork();
    ఒకవేళ (pid1 < 0) {
      రాయి("fork failed\n");
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid1 == 0) {
      fork();
      fork();
      నిష్క్రమించు(0);
    }
    wait(0);
  }

  నిష్క్రమించు(0);
}

// allocate all mem, free it, and allocate again
శూన్య
mem(అక్షరం *s)
{
  శూన్య *m1, *m2;
  పూర్ణాంకం pid;

  ఒకవేళ ((pid = fork()) == 0) {
    m1 = 0;
    అయితే ((m2 = జ్ఞాపకం_ఇవ్వు(10001)) != 0) {
      *(అక్షరం **)m2 = m1;
      m1 = m2;
    }
    అయితే (m1) {
      m2 = *(అక్షరం **)m1;
      విడుదల(m1);
      m1 = m2;
    }
    m1 = జ్ఞాపకం_ఇవ్వు(1024 * 20);
    ఒకవేళ (m1 == 0) {
      రాయి("%s: couldn't allocate mem?!!\n", s);
      నిష్క్రమించు(1);
    }
    విడుదల(m1);
    నిష్క్రమించు(0);
  } లేకపోతే {
    పూర్ణాంకం xstatus;
    wait(&xstatus);
    ఒకవేళ (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      నిష్క్రమించు(0);
    }
    నిష్క్రమించు(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
శూన్య
sharedfd(అక్షరం *s)
{
  పూర్ణాంకం fd, pid, i, n, nc, np;
  గణన { N = 1000, SZ = 10 };
  అక్షరం buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot open sharedfd for writing", s);
    నిష్క్రమించు(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', పరిమాణం(buf));
  కొరకు (i = 0; i < N; i++) {
    ఒకవేళ (లిఖ(fd, buf, పరిమాణం(buf)) != పరిమాణం(buf)) {
      రాయి("%s: write sharedfd failed\n", s);
      నిష్క్రమించు(1);
    }
  }
  ఒకవేళ (pid == 0) {
    నిష్క్రమించు(0);
  } లేకపోతే {
    పూర్ణాంకం xstatus;
    wait(&xstatus);
    ఒకవేళ (xstatus != 0)
      నిష్క్రమించు(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot open sharedfd for reading\n", s);
    నిష్క్రమించు(1);
  }
  nc = np = 0;
  అయితే ((n = పఠ(fd, buf, పరిమాణం(buf))) > 0) {
    కొరకు (i = 0; i < పరిమాణం(buf); i++) {
      ఒకవేళ (buf[i] == 'c')
        nc++;
      ఒకవేళ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ఒకవేళ (nc == N * SZ && np == N * SZ) {
    నిష్క్రమించు(0);
  } లేకపోతే {
    రాయి("%s: nc/np test fails\n", s);
    నిష్క్రమించు(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
శూన్య
fourfiles(అక్షరం *s)
{
  పూర్ణాంకం fd, pid, i, j, n, మొత్తం, pi;
  అక్షరం *పేర్లు[] = {"f0", "f1", "f2", "f3"};
  అక్షరం *fname;
  గణన { N = 12, NCHILD = 4, SZ = 500 };

  కొరకు (pi = 0; pi < NCHILD; pi++) {
    fname = పేర్లు[pi];
    unlink(fname);

    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }

    ఒకవేళ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ఒకవేళ (fd < 0) {
        రాయి("%s: create failed\n", s);
        నిష్క్రమించు(1);
      }

      memset(buf, '0' + pi, SZ);
      కొరకు (i = 0; i < N; i++) {
        ఒకవేళ ((n = లిఖ(fd, buf, SZ)) != SZ) {
          రాయి("write failed %d\n", n);
          నిష్క్రమించు(1);
        }
      }
      నిష్క్రమించు(0);
    }
  }

  పూర్ణాంకం xstatus;
  కొరకు (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ఒకవేళ (xstatus != 0)
      నిష్క్రమించు(xstatus);
  }

  కొరకు (i = 0; i < NCHILD; i++) {
    fname = పేర్లు[i];
    fd = open(fname, 0);
    మొత్తం = 0;
    అయితే ((n = పఠ(fd, buf, పరిమాణం(buf))) > 0) {
      కొరకు (j = 0; j < n; j++) {
        ఒకవేళ (buf[j] != '0' + i) {
          రాయి("%s: wrong char\n", s);
          నిష్క్రమించు(1);
        }
      }
      మొత్తం += n;
    }
    close(fd);
    ఒకవేళ (మొత్తం != N * SZ) {
      రాయి("wrong length %d\n", మొత్తం);
      నిష్క్రమించు(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
శూన్య
createdelete(అక్షరం *s)
{
  గణన { N = 20, NCHILD = 4 };
  పూర్ణాంకం pid, i, fd, pi;
  అక్షరం name[32];

  కొరకు (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }

    ఒకవేళ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      కొరకు (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ఒకవేళ (fd < 0) {
          రాయి("%s: create failed\n", s);
          నిష్క్రమించు(1);
        }
        close(fd);
        ఒకవేళ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ఒకవేళ (unlink(name) < 0) {
            రాయి("%s: unlink failed\n", s);
            నిష్క్రమించు(1);
          }
        }
      }
      నిష్క్రమించు(0);
    }
  }

  పూర్ణాంకం xstatus;
  కొరకు (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ఒకవేళ (xstatus != 0)
      నిష్క్రమించు(1);
  }

  name[0] = name[1] = name[2] = 0;
  కొరకు (i = 0; i < N; i++) {
    కొరకు (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ఒకవేళ ((i == 0 || i >= N / 2) && fd < 0) {
        రాయి("%s: oops createdelete %s didn't exist\n", s, name);
        నిష్క్రమించు(1);
      } లేకపోతే ఒకవేళ ((i >= 1 && i < N / 2) && fd >= 0) {
        రాయి("%s: oops createdelete %s did exist\n", s, name);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (fd >= 0)
        close(fd);
    }
  }

  కొరకు (i = 0; i < N; i++) {
    కొరకు (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
శూన్య
unlinkread(అక్షరం *s)
{
  గణన { SZ = 5 };
  పూర్ణాంకం fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: create unlinkread failed\n", s);
    నిష్క్రమించు(1);
  }
  లిఖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open unlinkread failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("unlinkread") != 0) {
    రాయి("%s: unlink unlinkread failed\n", s);
    నిష్క్రమించు(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  లిఖ(fd1, "yyy", 3);
  close(fd1);

  ఒకవేళ (పఠ(fd, buf, పరిమాణం(buf)) != SZ) {
    రాయి("%s: unlinkread read failed", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (buf[0] != 'h') {
    రాయి("%s: unlinkread wrong data\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (లిఖ(fd, buf, 10) != 10) {
    రాయి("%s: unlinkread write failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
  unlink("unlinkread");
}

శూన్య
linktest(అక్షరం *s)
{
  గణన { SZ = 5 };
  పూర్ణాంకం fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: create lf1 failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (లిఖ(fd, "hello", SZ) != SZ) {
    రాయి("%s: write lf1 failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (link("lf1", "lf2") < 0) {
    రాయి("%s: link lf1 lf2 failed\n", s);
    నిష్క్రమించు(1);
  }
  unlink("lf1");

  ఒకవేళ (open("lf1", 0) >= 0) {
    రాయి("%s: unlinked lf1 but it is still there!\n", s);
    నిష్క్రమించు(1);
  }

  fd = open("lf2", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open lf2 failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (పఠ(fd, buf, పరిమాణం(buf)) != SZ) {
    రాయి("%s: read lf2 failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (link("lf2", "lf2") >= 0) {
    రాయి("%s: link lf2 lf2 succeeded! oops\n", s);
    నిష్క్రమించు(1);
  }

  unlink("lf2");
  ఒకవేళ (link("lf2", "lf1") >= 0) {
    రాయి("%s: link non-existent succeeded! oops\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (link(".", "lf1") >= 0) {
    రాయి("%s: link . lf1 succeeded! oops\n", s);
    నిష్క్రమించు(1);
  }
}

// test concurrent create/link/unlink of the same file
శూన్య
concreate(అక్షరం *s)
{
  గణన { N = 40 };
  అక్షరం file[3];
  పూర్ణాంకం i, pid, n, fd;
  అక్షరం fa[N];
  నిర్మాణం {
    ushort inum;
    అక్షరం name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  కొరకు (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ఒకవేళ (pid && (i % 3) == 1) {
      link("C0", file);
    } లేకపోతే ఒకవేళ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } లేకపోతే {
      fd = open(file, O_CREATE | O_RDWR);
      ఒకవేళ (fd < 0) {
        రాయి("concreate create %s failed\n", file);
        నిష్క్రమించు(1);
      }
      close(fd);
    }
    ఒకవేళ (pid == 0) {
      నిష్క్రమించు(0);
    } లేకపోతే {
      పూర్ణాంకం xstatus;
      wait(&xstatus);
      ఒకవేళ (xstatus != 0)
        నిష్క్రమించు(1);
    }
  }

  memset(fa, 0, పరిమాణం(fa));
  fd = open(".", 0);
  n = 0;
  అయితే (పఠ(fd, &de, పరిమాణం(de)) > 0) {
    ఒకవేళ (de.inum == 0)
      కొనసాగు;
    ఒకవేళ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ఒకవేళ (i < 0 || i >= పరిమాణం(fa)) {
        రాయి("%s: concreate weird file %s\n", s, de.name);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (fa[i]) {
        రాయి("%s: concreate duplicate file %s\n", s, de.name);
        నిష్క్రమించు(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ఒకవేళ (n != N) {
    రాయి("%s: concreate not enough files in directory listing\n", s);
    నిష్క్రమించు(1);
  }

  కొరకు (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } లేకపోతే {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ఒకవేళ (pid == 0)
      నిష్క్రమించు(0);
    లేకపోతే
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
శూన్య
linkunlink(అక్షరం *s)
{
  పూర్ణాంకం pid, i;

  unlink("x");
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }

  గుర్తులేని పూర్ణాంకం x = (pid ? 1 : 97);
  కొరకు (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ఒకవేళ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } లేకపోతే ఒకవేళ ((x % 3) == 1) {
      link("cat", "x");
    } లేకపోతే {
      unlink("x");
    }
  }

  ఒకవేళ (pid)
    wait(0);
  లేకపోతే
    నిష్క్రమించు(0);
}

శూన్య
subdir(అక్షరం *s)
{
  పూర్ణాంకం fd, cc;

  unlink("ff");
  ఒకవేళ (mkdir("dd") != 0) {
    రాయి("%s: mkdir dd failed\n", s);
    నిష్క్రమించు(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: create dd/ff failed\n", s);
    నిష్క్రమించు(1);
  }
  లిఖ(fd, "ff", 2);
  close(fd);

  ఒకవేళ (unlink("dd") >= 0) {
    రాయి("%s: unlink dd (non-empty dir) succeeded!\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (mkdir("/dd/dd") != 0) {
    రాయి("%s: subdir mkdir dd/dd failed\n", s);
    నిష్క్రమించు(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: create dd/dd/ff failed\n", s);
    నిష్క్రమించు(1);
  }
  లిఖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open dd/dd/../ff failed\n", s);
    నిష్క్రమించు(1);
  }
  cc = పఠ(fd, buf, పరిమాణం(buf));
  ఒకవేళ (cc != 2 || buf[0] != 'f') {
    రాయి("%s: dd/dd/../ff wrong content\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    రాయి("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (unlink("dd/dd/ff") != 0) {
    రాయి("%s: unlink dd/dd/ff failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("dd/dd/ff", O_RDONLY) >= 0) {
    రాయి("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (chdir("dd") != 0) {
    రాయి("%s: chdir dd failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("dd/../../dd") != 0) {
    రాయి("%s: chdir dd/../../dd failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("dd/../../../dd") != 0) {
    రాయి("%s: chdir dd/../../../dd failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("./..") != 0) {
    రాయి("%s: chdir ./.. failed\n", s);
    నిష్క్రమించు(1);
  }

  fd = open("dd/dd/ffff", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open dd/dd/ffff failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (పఠ(fd, buf, పరిమాణం(buf)) != 2) {
    రాయి("%s: read dd/dd/ffff wrong len\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (open("dd/dd/ff", O_RDONLY) >= 0) {
    రాయి("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    రాయి("%s: create dd/ff/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    రాయి("%s: create dd/xx/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("dd", O_CREATE) >= 0) {
    రాయి("%s: create dd succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("dd", O_RDWR) >= 0) {
    రాయి("%s: open dd rdwr succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("dd", O_WRONLY) >= 0) {
    రాయి("%s: open dd wronly succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    రాయి("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    రాయి("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (link("dd/ff", "dd/dd/ffff") == 0) {
    రాయి("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("dd/ff/ff") == 0) {
    రాయి("%s: mkdir dd/ff/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("dd/xx/ff") == 0) {
    రాయి("%s: mkdir dd/xx/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("dd/dd/ffff") == 0) {
    రాయి("%s: mkdir dd/dd/ffff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd/xx/ff") == 0) {
    రాయి("%s: unlink dd/xx/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd/ff/ff") == 0) {
    రాయి("%s: unlink dd/ff/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("dd/ff") == 0) {
    రాయి("%s: chdir dd/ff succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("dd/xx") == 0) {
    రాయి("%s: chdir dd/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (unlink("dd/dd/ffff") != 0) {
    రాయి("%s: unlink dd/dd/ff failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd/ff") != 0) {
    రాయి("%s: unlink dd/ff failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd") == 0) {
    రాయి("%s: unlink non-empty dd succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd/dd") < 0) {
    రాయి("%s: unlink dd/dd failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dd") < 0) {
    రాయి("%s: unlink dd failed\n", s);
    నిష్క్రమించు(1);
  }
}

// test writes that are larger than the log.
శూన్య
bigwrite(అక్షరం *s)
{
  పూర్ణాంకం fd, sz;

  unlink("bigwrite");
  కొరకు (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ఒకవేళ (fd < 0) {
      రాయి("%s: cannot create bigwrite\n", s);
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం i;
    కొరకు (i = 0; i < 2; i++) {
      పూర్ణాంకం cc = లిఖ(fd, buf, sz);
      ఒకవేళ (cc != sz) {
        రాయి("%s: write(%d) ret %d\n", s, sz, cc);
        నిష్క్రమించు(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

శూన్య
bigfile(అక్షరం *s)
{
  గణన { N = 20, SZ = 600 };
  పూర్ణాంకం fd, i, మొత్తం, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot create bigfile", s);
    నిష్క్రమించు(1);
  }
  కొరకు (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ఒకవేళ (లిఖ(fd, buf, SZ) != SZ) {
      రాయి("%s: write bigfile failed\n", s);
      నిష్క్రమించు(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot open bigfile\n", s);
    నిష్క్రమించు(1);
  }
  మొత్తం = 0;
  కొరకు (i = 0;; i++) {
    cc = పఠ(fd, buf, SZ / 2);
    ఒకవేళ (cc < 0) {
      రాయి("%s: read bigfile failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (cc == 0)
      ఆపు;
    ఒకవేళ (cc != SZ / 2) {
      రాయి("%s: short read bigfile\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      రాయి("%s: read bigfile wrong data\n", s);
      నిష్క్రమించు(1);
    }
    మొత్తం += cc;
  }
  close(fd);
  ఒకవేళ (మొత్తం != N * SZ) {
    రాయి("%s: read bigfile wrong total\n", s);
    నిష్క్రమించు(1);
  }
  unlink("bigfile.dat");
}

శూన్య
fourteen(అక్షరం *s)
{
  పూర్ణాంకం fd;

  // DIRSIZ is 14.

  ఒకవేళ (mkdir("12345678901234") != 0) {
    రాయి("%s: mkdir 12345678901234 failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("12345678901234/123456789012345") != 0) {
    రాయి("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    నిష్క్రమించు(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ఒకవేళ (fd < 0) {
    రాయి(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  ఒకవేళ (mkdir("12345678901234/12345678901234") == 0) {
    రాయి("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("123456789012345/12345678901234") == 0) {
    రాయి("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    నిష్క్రమించు(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

శూన్య
rmdot(అక్షరం *s)
{
  ఒకవేళ (mkdir("dots") != 0) {
    రాయి("%s: mkdir dots failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("dots") != 0) {
    రాయి("%s: chdir dots failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink(".") == 0) {
    రాయి("%s: rm . worked!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("..") == 0) {
    రాయి("%s: rm .. worked!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("/") != 0) {
    రాయి("%s: chdir / failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dots/.") == 0) {
    రాయి("%s: unlink dots/. worked!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dots/..") == 0) {
    రాయి("%s: unlink dots/.. worked!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dots") != 0) {
    రాయి("%s: unlink dots failed!\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
dirfile(అక్షరం *s)
{
  పూర్ణాంకం fd;

  fd = open("dirfile", O_CREATE);
  ఒకవేళ (fd < 0) {
    రాయి("%s: create dirfile failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
  ఒకవేళ (chdir("dirfile") == 0) {
    రాయి("%s: chdir dirfile succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  fd = open("dirfile/xx", 0);
  ఒకవేళ (fd >= 0) {
    రాయి("%s: create dirfile/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ఒకవేళ (fd >= 0) {
    రాయి("%s: create dirfile/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("dirfile/xx") == 0) {
    రాయి("%s: mkdir dirfile/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dirfile/xx") == 0) {
    రాయి("%s: unlink dirfile/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (link("README", "dirfile/xx") == 0) {
    రాయి("%s: link to dirfile/xx succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("dirfile") != 0) {
    రాయి("%s: unlink dirfile failed!\n", s);
    నిష్క్రమించు(1);
  }

  fd = open(".", O_RDWR);
  ఒకవేళ (fd >= 0) {
    రాయి("%s: open . for writing succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  fd = open(".", 0);
  ఒకవేళ (లిఖ(fd, "x", 1) > 0) {
    రాయి("%s: write . succeeded!\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
శూన్య
iref(అక్షరం *s)
{
  పూర్ణాంకం i, fd;

  కొరకు (i = 0; i < NINODE + 1; i++) {
    ఒకవేళ (mkdir("irefd") != 0) {
      రాయి("%s: mkdir irefd failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (chdir("irefd") != 0) {
      రాయి("%s: chdir irefd failed\n", s);
      నిష్క్రమించు(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ఒకవేళ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ఒకవేళ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  కొరకు (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
శూన్య
forktest(అక్షరం *s)
{
  గణన { N = 1000 };
  పూర్ణాంకం n, pid;

  కొరకు (n = 0; n < N; n++) {
    pid = fork();
    ఒకవేళ (pid < 0)
      ఆపు;
    ఒకవేళ (pid == 0)
      నిష్క్రమించు(0);
  }

  ఒకవేళ (n == 0) {
    రాయి("%s: no fork at all!\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (n == N) {
    రాయి("%s: fork claimed to work 1000 times!\n", s);
    నిష్క్రమించు(1);
  }

  కొరకు (; n > 0; n--) {
    ఒకవేళ (wait(0) < 0) {
      రాయి("%s: wait stopped early\n", s);
      నిష్క్రమించు(1);
    }
  }

  ఒకవేళ (wait(0) != -1) {
    రాయి("%s: wait got too many\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
sbrkbasic(అక్షరం *s)
{
  గణన { TOOMUCH = 1024 * 1024 * 1024 };
  పూర్ణాంకం i, pid, xstatus;
  అక్షరం *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("fork failed in sbrkbasic\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    a = sbrk(TOOMUCH);
    ఒకవేళ (a == (అక్షరం *)SBRK_ERROR) {
      // it's OK if this fails.
      నిష్క్రమించు(0);
    }

    కొరకు (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    నిష్క్రమించు(1);
  }

  wait(&xstatus);
  ఒకవేళ (xstatus == 1) {
    రాయి("%s: too much memory allocated!\n", s);
    నిష్క్రమించు(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  కొరకు (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ఒకవేళ (b != a) {
      రాయి("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      నిష్క్రమించు(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: sbrk test fork failed\n", s);
    నిష్క్రమించు(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ఒకవేళ (c != a + 1) {
    రాయి("%s: sbrk test failed post-fork\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0)
    నిష్క్రమించు(0);
  wait(&xstatus);
  నిష్క్రమించు(xstatus);
}

శూన్య
sbrkmuch(అక్షరం *s)
{
  గణన { BIG = 100 * 1024 * 1024 };
  అక్షరం *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  ఒకవేళ (p != a) {
    రాయి("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    నిష్క్రమించు(1);
  }

  lastaddr = (అక్షరం *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ఒకవేళ (c == (అక్షరం *)SBRK_ERROR) {
    రాయి("%s: sbrk could not deallocate\n", s);
    నిష్క్రమించు(1);
  }
  c = sbrk(0);
  ఒకవేళ (c != a - PGSIZE) {
    రాయి("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    నిష్క్రమించు(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ఒకవేళ (c != a || sbrk(0) != a + PGSIZE) {
    రాయి("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (*lastaddr == 99) {
    // should be zero
    రాయి("%s: sbrk de-allocation didn't really deallocate\n", s);
    నిష్క్రమించు(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ఒకవేళ (c != a) {
    రాయి("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    నిష్క్రమించు(1);
  }
}

// can we read the kernel's memory?
శూన్య
kernmem(అక్షరం *s)
{
  అక్షరం *a;
  పూర్ణాంకం pid;

  కొరకు (a = (అక్షరం *)(KERNBASE); a < (అక్షరం *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid == 0) {
      రాయి("%s: oops could read %p = %x\n", s, a, *a);
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం xstatus;
    wait(&xstatus);
    ఒకవేళ (xstatus != -1) // did kernel kill child?
      నిష్క్రమించు(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
శూన్య
MAXVAplus(అక్షరం *s)
{
  అస్థిర uint64 a = MAXVA;
  కొరకు (; a != 0; a <<= 1) {
    పూర్ణాంకం pid;
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid == 0) {
      *(అక్షరం *)a = 99;
      రాయి("%s: oops wrote %p\n", s, (శూన్య *)a);
      నిష్క్రమించు(1);
    }
    పూర్ణాంకం xstatus;
    wait(&xstatus);
    ఒకవేళ (xstatus != -1) // did kernel kill child?
      నిష్క్రమించు(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
శూన్య
sbrkfail(అక్షరం *s)
{
  గణన { BIG = 100 * 1024 * 1024 };
  పూర్ణాంకం i, xstatus;
  పూర్ణాంకం fds[2];
  అక్షరం scratch;
  అక్షరం *c, *a;
  పూర్ణాంకం pids[10];
  పూర్ణాంకం pid;
  పూర్ణాంకం failed;

  failed = 0;
  ఒకవేళ (pipe(fds) != 0) {
    రాయి("%s: pipe() failed\n", s);
    నిష్క్రమించు(1);
  }
  కొరకు (i = 0; i < పరిమాణం(pids) / పరిమాణం(pids[0]); i++) {
    ఒకవేళ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ఒకవేళ (sbrk(BIG - (uint64)sbrk(0)) == (అక్షరం *)SBRK_ERROR)
        లిఖ(fds[1], "0", 1);
      లేకపోతే
        లిఖ(fds[1], "1", 1);
      // sit around until killed
      కొరకు (;;)
        pause(1000);
    }
    ఒకవేళ (pids[i] != -1) {
      పఠ(fds[0], &scratch, 1);
      ఒకవేళ (scratch == '0')
        failed = 1;
    }
  }
  ఒకవేళ (!failed) {
    రాయి("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  కొరకు (i = 0; i < పరిమాణం(pids) / పరిమాణం(pids[0]); i++) {
    ఒకవేళ (pids[i] == -1)
      కొనసాగు;
    kill(pids[i]);
    wait(0);
  }
  ఒకవేళ (c == (అక్షరం *)SBRK_ERROR) {
    రాయి("%s: failed sbrk leaked memory\n", s);
    నిష్క్రమించు(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ఒకవేళ (a == (అక్షరం *)SBRK_ERROR) {
      నిష్క్రమించు(0);
    }
    రాయి("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    నిష్క్రమించు(1);
  }
  wait(&xstatus);
  ఒకవేళ (xstatus != 0)
    నిష్క్రమించు(1);
}

// test reads/writes from/to allocated memory
శూన్య
sbrkarg(అక్షరం *s)
{
  అక్షరం *a;
  పూర్ణాంకం fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ఒకవేళ (fd < 0) {
    రాయి("%s: open sbrk failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ ((n = లిఖ(fd, a, PGSIZE)) < 0) {
    రాయి("%s: write sbrk failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ఒకవేళ (pipe((పూర్ణాంకం *)a) != 0) {
    రాయి("%s: pipe() failed\n", s);
    నిష్క్రమించు(1);
  }
}

శూన్య
validatetest(అక్షరం *s)
{
  పూర్ణాంకం hi;
  uint64 p;

  hi = 1100 * 1024;
  కొరకు (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ఒకవేళ (link("nosuchfile", (అక్షరం *)p) != -1) {
      రాయి("%s: link should not succeed\n", s);
      నిష్క్రమించు(1);
    }
  }
}

// does uninitialized data start out zero?
అక్షరం uninit[10000];
శూన్య
bsstest(అక్షరం *s)
{
  పూర్ణాంకం i;

  కొరకు (i = 0; i < పరిమాణం(uninit); i++) {
    ఒకవేళ (uninit[i] != '\0') {
      రాయి("%s: bss test failed\n", s);
      నిష్క్రమించు(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
శూన్య
bigargtest(అక్షరం *s)
{
  పూర్ణాంకం pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ఒకవేళ (pid == 0) {
    స్థిర అక్షరం *వాదనలు[MAXARG];
    పూర్ణాంకం i;
    అక్షరం big[400];
    memset(big, ' ', పరిమాణం(big));
    big[పరిమాణం(big) - 1] = '\0';
    కొరకు (i = 0; i < MAXARG - 1; i++)
      వాదనలు[i] = big;
    వాదనలు[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", వాదనలు);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    నిష్క్రమించు(0);
  } లేకపోతే ఒకవేళ (pid < 0) {
    రాయి("%s: bigargtest: fork failed\n", s);
    నిష్క్రమించు(1);
  }

  wait(&xstatus);
  ఒకవేళ (xstatus != 0)
    నిష్క్రమించు(xstatus);
  fd = open("bigarg-ok", 0);
  ఒకవేళ (fd < 0) {
    రాయి("%s: bigarg test failed!\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
శూన్య
fsfull()
{
  పూర్ణాంకం nfiles;
  పూర్ణాంకం fsblocks = 0;

  రాయి("fsfull test\n");

  కొరకు (nfiles = 0;; nfiles++) {
    అక్షరం name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    రాయి("writing %s\n", name);
    పూర్ణాంకం fd = open(name, O_CREATE | O_RDWR);
    ఒకవేళ (fd < 0) {
      రాయి("open %s failed\n", name);
      ఆపు;
    }
    పూర్ణాంకం మొత్తం = 0;
    అయితే (1) {
      పూర్ణాంకం cc = లిఖ(fd, buf, BSIZE);
      ఒకవేళ (cc < BSIZE)
        ఆపు;
      మొత్తం += cc;
      fsblocks++;
    }
    రాయి("wrote %d bytes\n", మొత్తం);
    close(fd);
    ఒకవేళ (మొత్తం == 0)
      ఆపు;
  }

  అయితే (nfiles >= 0) {
    అక్షరం name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  రాయి("fsfull test finished, %d blocks\n", fsblocks);
}

శూన్య
argptest(అక్షరం *s)
{
  పూర్ణాంకం fd;
  fd = open("init", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: open failed\n", s);
    నిష్క్రమించు(1);
  }
  పఠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
శూన్య
stacktest(అక్షరం *s)
{
  పూర్ణాంకం pid;
  పూర్ణాంకం xstatus;

  pid = fork();
  ఒకవేళ (pid == 0) {
    అక్షరం *sp = (అక్షరం *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    రాయి("%s: stacktest: read below stack %d\n", s, *sp);
    నిష్క్రమించు(1);
  } లేకపోతే ఒకవేళ (pid < 0) {
    రాయి("%s: fork failed\n", s);
    నిష్క్రమించు(1);
  }
  wait(&xstatus);
  ఒకవేళ (xstatus == -1) // kernel killed child?
    నిష్క్రమించు(0);
  లేకపోతే
    నిష్క్రమించు(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
శూన్య
nowrite(అక్షరం *s)
{
  పూర్ణాంకం pid;
  పూర్ణాంకం xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  కొరకు (పూర్ణాంకం ai = 0; ai < పరిమాణం(addrs) / పరిమాణం(addrs[0]); ai++) {
    pid = fork();
    ఒకవేళ (pid == 0) {
      అస్థిర పూర్ణాంకం *addr = (పూర్ణాంకం *)addrs[ai];
      *addr = 10;
      రాయి("%s: write to %p did not fail!\n", s, addr);
      నిష్క్రమించు(0);
    } లేకపోతే ఒకవేళ (pid < 0) {
      రాయి("%s: fork failed\n", s);
      నిష్క్రమించు(1);
    }
    wait(&xstatus);
    ఒకవేళ (xstatus == 0) {
      // kernel did not kill child!
      నిష్క్రమించు(1);
    }
  }
  నిష్క్రమించు(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
శూన్య *big = (శూన్య *)0xeaeb0b5b00002f5e;
శూన్య
pgbug(అక్షరం *s)
{
  అక్షరం *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  నిష్క్రమించు(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
శూన్య
sbrkbugs(అక్షరం *s)
{
  పూర్ణాంకం pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    పూర్ణాంకం sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    నిష్క్రమించు(0);
  }
  wait(0);

  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    పూర్ణాంకం sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    నిష్క్రమించు(0);
  }
  wait(0);

  pid = fork();
  ఒకవేళ (pid < 0) {
    రాయి("fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    నిష్క్రమించు(0);
  }
  wait(0);

  నిష్క్రమించు(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
శూన్య
sbrklast(అక్షరం *s)
{
  uint64 శిఖరం = (uint64)sbrk(0);
  ఒకవేళ ((శిఖరం % PGSIZE) != 0)
    sbrk(PGSIZE - (శిఖరం % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  శిఖరం = (uint64)sbrk(0);
  అక్షరం *p = (అక్షరం *)(శిఖరం - 64);
  p[0] = 'x';
  p[1] = '\0';
  పూర్ణాంకం fd = open(p, O_RDWR | O_CREATE);
  లిఖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  పఠ(fd, p, 1);
  ఒకవేళ (p[0] != 'x')
    నిష్క్రమించు(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
శూన్య
sbrk8000(అక్షరం *s)
{
  sbrk(0x80000004);
  అస్థిర అక్షరం *శిఖరం = sbrk(0);
  *(శిఖరం - 1) = *(శిఖరం - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
శూన్య
badarg(అక్షరం *s)
{
  కొరకు (పూర్ణాంకం i = 0; i < 50000; i++) {
    అక్షరం *argv[2];
    argv[0] = (అక్షరం *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  నిష్క్రమించు(0);
}

#నిర్వచించు REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
శూన్య
lazy_alloc(అక్షరం *s)
{
  అక్షరం *i, *prev_చివర, *new_చివర;

  prev_చివర = sbrklazy(REGION_SZ);
  ఒకవేళ (prev_చివర == (అక్షరం *)SBRK_ERROR) {
    రాయి("sbrklazy() failed\n");
    నిష్క్రమించు(1);
  }
  new_చివర = prev_చివర + REGION_SZ;

  కొరకు (i = prev_చివర + PGSIZE; i < new_చివర; i += 64 * PGSIZE)
    *(అక్షరం **)i = i;

  కొరకు (i = prev_చివర + PGSIZE; i < new_చివర; i += 64 * PGSIZE) {
    ఒకవేళ (*(అక్షరం **)i != i) {
      రాయి("failed to read value from memory\n");
      నిష్క్రమించు(1);
    }
  }

  నిష్క్రమించు(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
శూన్య
lazy_unmap(అక్షరం *s)
{
  పూర్ణాంకం pid;
  అక్షరం *i, *prev_చివర, *new_చివర;

  prev_చివర = sbrklazy(REGION_SZ);
  ఒకవేళ (prev_చివర == (అక్షరం *)SBRK_ERROR) {
    రాయి("sbrklazy() failed\n");
    నిష్క్రమించు(1);
  }
  new_చివర = prev_చివర + REGION_SZ;

  కొరకు (i = prev_చివర + PGSIZE; i < new_చివర; i += PGSIZE * PGSIZE)
    *(అక్షరం **)i = i;

  కొరకు (i = prev_చివర + PGSIZE; i < new_చివర; i += PGSIZE * PGSIZE) {
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("error forking\n");
      నిష్క్రమించు(1);
    } లేకపోతే ఒకవేళ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(అక్షరం **)i = i;
      నిష్క్రమించు(0);
    } లేకపోతే {
      పూర్ణాంకం స్థితి_2;
      wait(&స్థితి_2);
      ఒకవేళ (స్థితి_2 == 0) {
        రాయి("memory not unmapped\n");
        నిష్క్రమించు(1);
      }
    }
  }

  నిష్క్రమించు(0);
}

శూన్య
lazy_copy(అక్షరం *s)
{
  // copyinstr on lazy page
  {
    అక్షరం *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    శూన్య *xx = sbrk(0);
    శూన్య *ret = sbrk(-(((uint64)xx) + 1));
    ఒకవేళ (ret != xx) {
      రాయి("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      నిష్క్రమించు(1);
    }
  }

  // read() and write() to these addresses should fail.
  గుర్తులేని దీర్ఘ bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  కొరకు (పూర్ణాంకం i = 0; i < పరిమాణం(bad) / పరిమాణం(bad[0]); i++) {
    పూర్ణాంకం fd = open("README", 0);
    ఒకవేళ (fd < 0) {
      రాయి("cannot open README\n");
      నిష్క్రమించు(1);
    }
    ఒకవేళ (పఠ(fd, (అక్షరం *)bad[i], 512) >= 0) {
      రాయి("read succeeded\n");
      నిష్క్రమించు(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ఒకవేళ (fd < 0) {
      రాయి("cannot open junk\n");
      నిష్క్రమించు(1);
    }
    ఒకవేళ (లిఖ(fd, (అక్షరం *)bad[i], 512) >= 0) {
      రాయి("write succeeded\n");
      నిష్క్రమించు(1);
    }
    close(fd);
  }

  నిష్క్రమించు(0);
}

శూన్య
lazy_copyinstr(అక్షరం *s)
{
  అక్షరం *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ఒకవేళ ((uint64)p % PGSIZE != 0) {
    రాయి("%s: sbrk did not align\n", s);
    నిష్క్రమించు(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  పూర్ణాంకం fd = open(&p[4095], O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("could not open /");
    నిష్క్రమించు(1);
  }

  నిర్మాణం stat st;
  పూర్ణాంకం r = fstat(fd, &st);
  ఒకవేళ (r < 0) {
    రాయి("could not stat /");
    నిష్క్రమించు(1);
  }

  ఒకవేళ (st.type != T_DIR) {
    రాయి("/ is not T_DIR");
    నిష్క్రమించు(1);
  }

  close(fd);
}

శూన్య
lazy_sbrk(అక్షరం *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  అక్షరం *p = sbrk(0);
  అయితే ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ఒకవేళ (p < 0) {
      రాయి("sbrklazy(%d) returned %p\n", 1 << 30, p);
      నిష్క్రమించు(1);
    }

    p = sbrklazy(0);
  }

  పూర్ణాంకం n = TRAPFRAME - PGSIZE - (uint64)p;

  అక్షరం *p1 = sbrklazy(n);
  ఒకవేళ (p1 < 0 || p1 != p) {
    రాయి("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    నిష్క్రమించు(1);
  }

  p = sbrk(PGSIZE);
  ఒకవేళ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    రాయి("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    నిష్క్రమించు(1);
  }

  p[0] = 1;
  ఒకవేళ (p[1] != 0) {
    రాయి("sbrk() returned non-zero-filled memory\n");
    నిష్క్రమించు(1);
  }

  p = sbrk(1);
  ఒకవేళ ((uint64)p != -1) {
    రాయి("sbrk(1) returned %p, expected error\n", p);
    నిష్క్రమించు(1);
  }

  p = sbrklazy(1);
  ఒకవేళ ((uint64)p != -1) {
    రాయి("sbrklazy(1) returned %p, expected error\n", p);
    నిష్క్రమించు(1);
  }

  నిష్క్రమించు(0);
}

శూన్య
partial_write(అక్షరం *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  పూర్ణాంకం fd = open("testfile", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot create testfile\n", s);
    నిష్క్రమించు(1);
  }

  పూర్ణాంకం cc = లిఖ(fd, "A", 1);
  ఒకవేళ (cc != 1) {
    రాయి("%s: could not write A\n", s);
    నిష్క్రమించు(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot re-open testfile\n", s);
    నిష్క్రమించు(1);
  }

  అక్షరం *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ఒకవేళ ((uint64)p % PGSIZE != 0) {
    రాయి("%s: sbrk did not align\n", s);
    నిష్క్రమించు(1);
  }

  p[-1] = 'X';

  cc = లిఖ(fd, p - 1, 2);
  ఒకవేళ (cc != -1) {
    రాయి("%s: write succeeded, should have failed\n", s);
    నిష్క్రమించు(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot re-open testfile\n", s);
    నిష్క్రమించు(1);
  }

  అక్షరం b;
  cc = పఠ(fd, &b, 1);
  ఒకవేళ (cc != 1) {
    రాయి("%s: cannot read testfile\n", s);
    నిష్క్రమించు(1);
  }

  close(fd);

  ఒకవేళ (b != 'X') {
    రాయి("%s: read returned %c, expected X\n", s, b);
    నిష్క్రమించు(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  కొరకు (పూర్ణాంకం i = 0; i < 64; i++) {
    అక్షరం buf[1024];
    memset(buf, 0, పరిమాణం(buf));
    cc = లిఖ(fd, buf, పరిమాణం(buf));
    ఒకవేళ (cc != పరిమాణం(buf)) {
      రాయి("%s: could not write to bigfile\n", s);
      నిష్క్రమించు(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot re-open testfile\n", s);
    నిష్క్రమించు(1);
  }

  cc = పఠ(fd, &b, 1);
  ఒకవేళ (cc != 1) {
    రాయి("%s: cannot read testfile\n", s);
    నిష్క్రమించు(1);
  }

  close(fd);

  ఒకవేళ (b != 'X') {
    రాయి("%s: read returned %c, expected X\n", s, b);
    నిష్క్రమించు(1);
  }

  unlink("testfile");
}

శూన్య
unlinkcwd(అక్షరం *s)
{
  ఒకవేళ (mkdir("/a") < 0) {
    రాయి("%s: mkdir /a failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (mkdir("/a/b") < 0) {
    రాయి("%s: mkdir /a/b failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (chdir("/a/b") < 0) {
    రాయి("%s: chdir failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("/a/b") < 0) {
    రాయి("%s: unlink /a/b failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink("/a") < 0) {
    రాయి("%s: unlink /a failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open("../", O_RDONLY) > 0) {
    రాయి("%s: open ../ non-existing directory\n", s);
  }
  ఒకవేళ (open("../c", O_CREATE) > 0) {
    రాయి("%s: create ../c non-existing file\n", s);
  }
}

నిర్మాణం test {
  శూన్య (*f)(అక్షరం *);
  అక్షరం *s;
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
శూన్య
bigdir(అక్షరం *s)
{
  గణన { N = 500 };
  పూర్ణాంకం i, fd;
  అక్షరం name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ఒకవేళ (fd < 0) {
    రాయి("%s: bigdir create failed\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  కొరకు (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ఒకవేళ (link("bd", name) != 0) {
      రాయి("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      నిష్క్రమించు(1);
    }
  }

  unlink("bd");
  కొరకు (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ఒకవేళ (unlink(name) != 0) {
      రాయి("%s: bigdir unlink failed", s);
      నిష్క్రమించు(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
శూన్య
manywrites(అక్షరం *s)
{
  పూర్ణాంకం nchildren = 4;
  పూర్ణాంకం howmany = 30; // increase to look for deadlock

  కొరకు (పూర్ణాంకం ci = 0; ci < nchildren; ci++) {
    పూర్ణాంకం pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("fork failed\n");
      నిష్క్రమించు(1);
    }

    ఒకవేళ (pid == 0) {
      అక్షరం name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      కొరకు (పూర్ణాంకం iters = 0; iters < howmany; iters++) {
        కొరకు (పూర్ణాంకం i = 0; i < ci + 1; i++) {
          పూర్ణాంకం fd = open(name, O_CREATE | O_RDWR);
          ఒకవేళ (fd < 0) {
            రాయి("%s: cannot create %s\n", s, name);
            నిష్క్రమించు(1);
          }
          పూర్ణాంకం sz = పరిమాణం(buf);
          పూర్ణాంకం cc = లిఖ(fd, buf, sz);
          ఒకవేళ (cc != sz) {
            రాయి("%s: write(%d) ret %d\n", s, sz, cc);
            నిష్క్రమించు(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      నిష్క్రమించు(0);
    }
  }

  కొరకు (పూర్ణాంకం ci = 0; ci < nchildren; ci++) {
    పూర్ణాంకం st = 0;
    wait(&st);
    ఒకవేళ (st != 0)
      నిష్క్రమించు(st);
  }
  నిష్క్రమించు(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
శూన్య
badwrite(అక్షరం *s)
{
  పూర్ణాంకం assumed_free = 600;

  unlink("junk");
  కొరకు (పూర్ణాంకం i = 0; i < assumed_free; i++) {
    పూర్ణాంకం fd = open("junk", O_CREATE | O_WRONLY);
    ఒకవేళ (fd < 0) {
      రాయి("open junk failed\n");
      నిష్క్రమించు(1);
    }
    లిఖ(fd, (అక్షరం *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  పూర్ణాంకం fd = open("junk", O_CREATE | O_WRONLY);
  ఒకవేళ (fd < 0) {
    రాయి("open junk failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (లిఖ(fd, "x", 1) != 1) {
    రాయి("write failed\n");
    నిష్క్రమించు(1);
  }
  close(fd);
  unlink("junk");

  నిష్క్రమించు(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
శూన్య
execout(అక్షరం *s)
{
  కొరకు (పూర్ణాంకం avail = 0; avail < 15; avail++) {
    పూర్ణాంకం pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("fork failed\n");
      నిష్క్రమించు(1);
    } లేకపోతే ఒకవేళ (pid == 0) {
      // allocate all of memory.
      అయితే (1) {
        అక్షరం *a = sbrk(PGSIZE);
        ఒకవేళ (a == SBRK_ERROR)
          ఆపు;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      కొరకు (పూర్ణాంకం i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      అక్షరం *వాదనలు[] = {"echo", "x", 0};
      exec("echo", వాదనలు);
      నిష్క్రమించు(0);
    } లేకపోతే {
      wait((పూర్ణాంకం *)0);
    }
  }

  నిష్క్రమించు(0);
}

// can the kernel tolerate running out of disk space?
శూన్య
diskfull(అక్షరం *s)
{
  పూర్ణాంకం fi;
  పూర్ణాంకం పూర్తి = 0;

  unlink("diskfulldir");

  కొరకు (fi = 0; పూర్తి == 0 && '0' + fi < 0177; fi++) {
    అక్షరం name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    పూర్ణాంకం fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ఒకవేళ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      రాయి("%s: could not create file %s\n", s, name);
      పూర్తి = 1;
      ఆపు;
    }
    కొరకు (పూర్ణాంకం i = 0; i < MAXFILE; i++) {
      అక్షరం buf[BSIZE];
      ఒకవేళ (లిఖ(fd, buf, BSIZE) != BSIZE) {
        పూర్తి = 1;
        close(fd);
        ఆపు;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  పూర్ణాంకం nzz = 128;
  కొరకు (పూర్ణాంకం i = 0; i < nzz; i++) {
    అక్షరం name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    పూర్ణాంకం fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ఒకవేళ (fd < 0)
      ఆపు;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ఒకవేళ (mkdir("diskfulldir") == 0)
    రాయి("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  కొరకు (పూర్ణాంకం i = 0; i < nzz; i++) {
    అక్షరం name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  కొరకు (పూర్ణాంకం i = 0; '0' + i < 0177; i++) {
    అక్షరం name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

శూన్య
outofinodes(అక్షరం *s)
{
  పూర్ణాంకం nzz = 32 * 32;
  కొరకు (పూర్ణాంకం i = 0; i < nzz; i++) {
    అక్షరం name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    పూర్ణాంకం fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ఒకవేళ (fd < 0) {
      // failure is eventually expected.
      ఆపు;
    }
    close(fd);
  }

  కొరకు (పూర్ణాంకం i = 0; i < nzz; i++) {
    అక్షరం name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

శూన్య
linkoverflow(అక్షరం *s)
{
  గణన { లక్ష్యం_2 = 32768 };
  గణన { DIRS = 64 };
  నిర్మాణం stat st;
  పూర్ణాంకం i;

  unlink("/lof");
  పూర్ణాంకం fd = open("/lof", O_CREATE | O_RDWR);
  ఒకవేళ (fd < 0) {
    రాయి("%s: cannot create /lof\n", s);
    నిష్క్రమించు(1);
  }
  close(fd);

  కొరకు (i = 0; i < లక్ష్యం_2; i++) {
    పూర్ణాంకం d = i % DIRS;
    పూర్ణాంకం f = i / DIRS;

    అక్షరం pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ఒకవేళ (f == 0 && mkdir(pn) < 0) {
      రాయి("%s: mkdir(%s) failed\n", s, pn);
      నిష్క్రమించు(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ఒకవేళ (link("/lof", pn) < 0) {
      ఒకవేళ (stat("/lof", &st) < 0) {
        రాయి("%s: stat(/lof) failed\n", s);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (st.nlink >= 32767) {
        // overflow check succeeded.
        ఆపు;
      }
      రాయి("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      నిష్క్రమించు(1);
    }

    ఒకవేళ (i % 100 == 0) {
      రాయి("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ఒకవేళ (stat("/lof", &st) < 0) {
    రాయి("%s: stat(/lof) failed\n", s);
    నిష్క్రమించు(1);
  }

  unlink("/lof");

  ఒకవేళ (st.nlink < 0) {
    రాయి("%s: negative link count: %d\n", s, st.nlink);
    నిష్క్రమించు(1);
  }
}

నిర్మాణం test slowtests[] = {
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
పూర్ణాంకం
నిరంతరం(శూన్య f(అక్షరం *), అక్షరం *s)
{
  పూర్ణాంకం pid;
  పూర్ణాంకం xstatus;

  రాయి("test %s: ", s);
  ఒకవేళ ((pid = fork()) < 0) {
    రాయి("runtest: fork error\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid == 0) {
    f(s);
    నిష్క్రమించు(0);
  } లేకపోతే {
    wait(&xstatus);
    ఒకవేళ (xstatus != 0)
      రాయి("FAILED\n");
    లేకపోతే
      రాయి("OK\n");
    తిరిగి xstatus == 0;
  }
}

పూర్ణాంకం
runtests(నిర్మాణం test *tests, అక్షరం *justone, పూర్ణాంకం continuous)
{
  పూర్ణాంకం ntests = 0;
  కొరకు (నిర్మాణం test *t = tests; t->s != 0; t++) {
    ఒకవేళ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ఒకవేళ (!నిరంతరం(t->f, t->s)) {
        ఒకవేళ (continuous != 2) {
          రాయి("SOME TESTS FAILED\n");
          తిరిగి -1;
        }
      }
    }
  }
  తిరిగి ntests;
}

// use sbrk() to count how many free physical memory pages there are.
పూర్ణాంకం
countfree()
{
  పూర్ణాంకం n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  అయితే (1) {
    అక్షరం *a = sbrk(PGSIZE);
    ఒకవేళ (a == SBRK_ERROR) {
      ఆపు;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  తిరిగి n;
}

పూర్ణాంకం
drivetests(పూర్ణాంకం శీఘ్ర, పూర్ణాంకం continuous, అక్షరం *justone)
{
  చేయి {
    రాయి("usertests starting\n");
    పూర్ణాంకం free0 = countfree();
    పూర్ణాంకం free1 = 0;
    పూర్ణాంకం ntests = 0;
    పూర్ణాంకం n;
    n = runtests(quicktests, justone, continuous);
    ఒకవేళ (n < 0) {
      ఒకవేళ (continuous != 2) {
        తిరిగి 1;
      }
    } లేకపోతే {
      ntests += n;
    }
    ఒకవేళ (!శీఘ్ర) {
      ఒకవేళ (justone == 0)
        రాయి("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ఒకవేళ (n < 0) {
        ఒకవేళ (continuous != 2) {
          తిరిగి 1;
        }
      } లేకపోతే {
        ntests += n;
      }
    }
    ఒకవేళ ((free1 = countfree()) < free0) {
      రాయి("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ఒకవేళ (continuous != 2) {
        తిరిగి 1;
      }
    }
    ఒకవేళ (justone != 0 && ntests == 0) {
      రాయి("NO TESTS EXECUTED\n");
      తిరిగి 1;
    }
  } అయితే (continuous);
  తిరిగి 0;
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం continuous = 0;
  పూర్ణాంకం శీఘ్ర = 0;
  అక్షరం *justone = 0;

  ఒకవేళ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    శీఘ్ర = 1;
  } లేకపోతే ఒకవేళ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } లేకపోతే ఒకవేళ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } లేకపోతే ఒకవేళ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } లేకపోతే ఒకవేళ (argc > 1) {
    రాయి("Usage: usertests [-c] [-C] [-q] [testname]\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (drivetests(శీఘ్ర, continuous, justone)) {
    నిష్క్రమించు(1);
  }
  రాయి("ALL TESTS PASSED\n");
  నిష్క్రమించు(0);
}
