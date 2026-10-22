#अन्तर्भाव "kernel/param.h"
#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "kernel/syscall.h"
#अन्तर्भाव "kernel/memlayout.h"
#अन्तर्भाव "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

वर्ण buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
शून्य
copyin(वर्ण *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  अत्थाय (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = open("copyin1", O_CREATE | O_WRONLY);
    सचे (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    पूर्णाङ्क n = लिख(fd, (शून्य *)addr, 8192);
    सचे (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (शून्य *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = लिख(1, (वर्ण *)addr, 8192);
    सचे (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      exit(1);
    }

    पूर्णाङ्क fds[2];
    सचे (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = लिख(fds[1], (वर्ण *)addr, 8192);
    सचे (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
शून्य
copyout(वर्ण *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  अत्थाय (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = open("README", 0);
    सचे (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    पूर्णाङ्क n = पठ(fd, (शून्य *)addr, 8192);
    सचे (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      exit(1);
    }
    close(fd);

    पूर्णाङ्क fds[2];
    सचे (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = लिख(fds[1], "x", 1);
    सचे (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = पठ(fds[0], (शून्य *)addr, 8192);
    सचे (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
शून्य
copyinstr1(वर्ण *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  अत्थाय (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = open((वर्ण *)addr, O_CREATE | O_WRONLY);
    सचे (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (शून्य *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
शून्य
copyinstr2(वर्ण *s)
{
  वर्ण b[MAXPATH + 1];

  अत्थाय (पूर्णाङ्क i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  पूर्णाङ्क ret = unlink(b);
  सचे (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  पूर्णाङ्क fd = open(b, O_CREATE | O_WRONLY);
  सचे (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  सचे (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  वर्ण *args[] = {"xx", 0};
  ret = exec(b, args);
  सचे (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  पूर्णाङ्क pid = fork();
  सचे (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  सचे (pid == 0) {
    स्थिर वर्ण big[PGSIZE + 1];
    अत्थाय (पूर्णाङ्क i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    वर्ण *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    सचे (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  पूर्णाङ्क st = 0;
  wait(&st);
  सचे (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
शून्य
copyinstr3(वर्ण *s)
{
  sbrk(8192);
  uint64 शिखरम् = (uint64)sbrk(0);
  सचे ((शिखरम् % PGSIZE) != 0) {
    sbrk(PGSIZE - (शिखरम् % PGSIZE));
  }
  शिखरम् = (uint64)sbrk(0);
  सचे (शिखरम् % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  वर्ण *b = (वर्ण *)(शिखरम् - 1);
  *b = 'x';

  पूर्णाङ्क ret = unlink(b);
  सचे (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  पूर्णाङ्क fd = open(b, O_CREATE | O_WRONLY);
  सचे (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  सचे (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  वर्ण *args[] = {"xx", 0};
  ret = exec(b, args);
  सचे (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
शून्य
rwsbrk(वर्ण *s)
{
  पूर्णाङ्क fd, n;

  uint64 a = (uint64)sbrk(8192);

  सचे (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  सचे (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  सचे (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = लिख(fd, (शून्य *)(a + PGSIZE), 1024);
  सचे (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  सचे (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = पठ(fd, (शून्य *)(a + PGSIZE), 10);
  सचे (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
शून्य
truncate1(वर्ण *s)
{
  वर्ण buf[32];

  unlink("truncfile");
  पूर्णाङ्क fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  लिख(fd1, "abcd", 4);
  close(fd1);

  पूर्णाङ्क fd2 = open("truncfile", O_RDONLY);
  पूर्णाङ्क n = पठ(fd2, buf, परिमाण(buf));
  सचे (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  पूर्णाङ्क fd3 = open("truncfile", O_RDONLY);
  n = पठ(fd3, buf, परिमाण(buf));
  सचे (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = पठ(fd2, buf, परिमाण(buf));
  सचे (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  लिख(fd1, "abcdef", 6);

  n = पठ(fd3, buf, परिमाण(buf));
  सचे (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = पठ(fd2, buf, परिमाण(buf));
  सचे (n != 2) {
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
शून्य
truncate2(वर्ण *s)
{
  unlink("truncfile");

  पूर्णाङ्क fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  लिख(fd1, "abcd", 4);

  पूर्णाङ्क fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  पूर्णाङ्क n = लिख(fd1, "x", 1);
  सचे (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

शून्य
truncate3(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  सचे (pid == 0) {
    अत्थाय (पूर्णाङ्क i = 0; i < 100; i++) {
      वर्ण buf[32];
      पूर्णाङ्क fd = open("truncfile", O_WRONLY);
      सचे (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      पूर्णाङ्क n = लिख(fd, "1234567890", 10);
      सचे (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      पठ(fd, buf, परिमाण(buf));
      close(fd);
    }
    exit(0);
  }

  अत्थाय (पूर्णाङ्क i = 0; i < 150; i++) {
    पूर्णाङ्क fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    सचे (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    पूर्णाङ्क n = लिख(fd, "xxx", 3);
    सचे (n != 3) {
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
शून्य
iputtest(वर्ण *s)
{
  सचे (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  सचे (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  सचे (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  सचे (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
शून्य
exitiputtest(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid == 0) {
    सचे (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    सचे (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    सचे (unlink("../iputdir") < 0) {
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
शून्य
openiputtest(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  सचे (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid == 0) {
    पूर्णाङ्क fd = open("oidir", O_RDWR);
    सचे (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  सचे (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

शून्य
opentest(वर्ण *s)
{
  पूर्णाङ्क fd;

  fd = open("echo", 0);
  सचे (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  सचे (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

शून्य
writetest(वर्ण *s)
{
  पूर्णाङ्क fd;
  पूर्णाङ्क i;
  गणना { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  अत्थाय (i = 0; i < N; i++) {
    सचे (लिख(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    सचे (लिख(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = पठ(fd, buf, N * SZ * 2);
  सचे (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  सचे (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

शून्य
writebig(वर्ण *s)
{
  पूर्णाङ्क i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  अत्थाय (i = 0; i < MAXFILE; i++) {
    ((पूर्णाङ्क *)buf)[0] = i;
    सचे (लिख(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  अत्थाय (;;) {
    i = पठ(fd, buf, BSIZE);
    सचे (i == 0) {
      सचे (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      भङ्ग;
    } अञ्ञथा सचे (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    सचे (((पूर्णाङ्क *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((पूर्णाङ्क *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  सचे (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
शून्य
createtest(वर्ण *s)
{
  पूर्णाङ्क i, fd;
  गणना { N = 52 };

  वर्ण name[3];
  name[0] = 'a';
  name[2] = '\0';
  अत्थाय (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  अत्थाय (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

शून्य
dirtest(वर्ण *s)
{
  सचे (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  सचे (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  सचे (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  सचे (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

शून्य
exectest(वर्ण *s)
{
  पूर्णाङ्क fd, xstatus, pid;
  वर्ण *echoargv[] = {"echo", "OK", 0};
  वर्ण buf[3];

  unlink("echo-ok");
  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid == 0) {
    पूर्णाङ्क errfd = dup(1);
    सचे (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    सचे (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    सचे (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    सचे (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  सचे (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  सचे (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  सचे (पठ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  सचे (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  अञ्ञथा {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

शून्य
pipe1(वर्ण *s)
{
  पूर्णाङ्क fds[2], pid, xstatus;
  पूर्णाङ्क seq, i, n, cc, total;
  गणना { N = 5, SZ = 1033 };

  सचे (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  सचे (pid == 0) {
    close(fds[0]);
    अत्थाय (n = 0; n < N; n++) {
      अत्थाय (i = 0; i < SZ; i++)
        buf[i] = seq++;
      सचे (लिख(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } अञ्ञथा सचे (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    याव ((n = पठ(fds[0], buf, cc)) > 0) {
      अत्थाय (i = 0; i < n; i++) {
        सचे ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          पटिनिवत्त;
        }
      }
      total += n;
      cc = cc * 2;
      सचे (cc > परिमाण(buf))
        cc = परिमाण(buf);
    }
    सचे (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } अञ्ञथा {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
शून्य
killstatus(वर्ण *s)
{
  पूर्णाङ्क xst;

  अत्थाय (पूर्णाङ्क i = 0; i < 100; i++) {
    पूर्णाङ्क pid1 = fork();
    सचे (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid1 == 0) {
      याव (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    सचे (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
शून्य
killzero(वर्ण *s)
{
  पूर्णाङ्क pid, xst;

  kill(0);

  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid == 0) {
    exit(7);
  }
  सचे (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  सचे (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
शून्य
preempt(वर्ण *s)
{
  पूर्णाङ्क pid1, pid2, pid3;
  पूर्णाङ्क pfds[2];

  pid1 = fork();
  सचे (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  सचे (pid1 == 0)
    अत्थाय (;;)
      ;

  pid2 = fork();
  सचे (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid2 == 0)
    अत्थाय (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  सचे (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid3 == 0) {
    close(pfds[0]);
    सचे (लिख(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    अत्थाय (;;)
      ;
  }

  close(pfds[1]);
  सचे (पठ(pfds[0], buf, परिमाण(buf)) != 1) {
    printf("%s: preempt read error", s);
    पटिनिवत्त;
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
शून्य
exitwait(वर्ण *s)
{
  पूर्णाङ्क i, pid;

  अत्थाय (i = 0; i < 100; i++) {
    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid) {
      पूर्णाङ्क xstate;
      सचे (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      सचे (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } अञ्ञथा {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
शून्य
reparent(वर्ण *s)
{
  पूर्णाङ्क master_pid = getpid();
  अत्थाय (पूर्णाङ्क i = 0; i < 200; i++) {
    पूर्णाङ्क pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid) {
      सचे (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } अञ्ञथा {
      पूर्णाङ्क pid2 = fork();
      सचे (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
शून्य
twochildren(वर्ण *s)
{
  अत्थाय (पूर्णाङ्क i = 0; i < 1000; i++) {
    पूर्णाङ्क pid1 = fork();
    सचे (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid1 == 0) {
      exit(0);
    } अञ्ञथा {
      पूर्णाङ्क pid2 = fork();
      सचे (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      सचे (pid2 == 0) {
        exit(0);
      } अञ्ञथा {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
शून्य
forkfork(वर्ण *s)
{
  गणना { N = 2 };

  अत्थाय (पूर्णाङ्क i = 0; i < N; i++) {
    पूर्णाङ्क pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    सचे (pid == 0) {
      अत्थाय (पूर्णाङ्क j = 0; j < 200; j++) {
        पूर्णाङ्क pid1 = fork();
        सचे (pid1 < 0) {
          exit(1);
        }
        सचे (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  पूर्णाङ्क xstatus;
  अत्थाय (पूर्णाङ्क i = 0; i < N; i++) {
    wait(&xstatus);
    सचे (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

शून्य
forkforkfork(वर्ण *s)
{
  unlink("stopforking");

  पूर्णाङ्क pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  सचे (pid == 0) {
    याव (1) {
      पूर्णाङ्क fd = open("stopforking", 0);
      सचे (fd >= 0) {
        exit(0);
      }
      सचे (fork() < 0) {
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
शून्य
reparent2(वर्ण *s)
{
  अत्थाय (पूर्णाङ्क i = 0; i < 800; i++) {
    पूर्णाङ्क pid1 = fork();
    सचे (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    सचे (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
शून्य
mem(वर्ण *s)
{
  शून्य *m1, *m2;
  पूर्णाङ्क pid;

  सचे ((pid = fork()) == 0) {
    m1 = 0;
    याव ((m2 = malloc(10001)) != 0) {
      *(वर्ण **)m2 = m1;
      m1 = m2;
    }
    याव (m1) {
      m2 = *(वर्ण **)m1;
      मुक्त(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    सचे (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    मुक्त(m1);
    exit(0);
  } अञ्ञथा {
    पूर्णाङ्क xstatus;
    wait(&xstatus);
    सचे (xstatus == -1) {
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
शून्य
sharedfd(वर्ण *s)
{
  पूर्णाङ्क fd, pid, i, n, nc, np;
  गणना { N = 1000, SZ = 10 };
  वर्ण buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', परिमाण(buf));
  अत्थाय (i = 0; i < N; i++) {
    सचे (लिख(fd, buf, परिमाण(buf)) != परिमाण(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  सचे (pid == 0) {
    exit(0);
  } अञ्ञथा {
    पूर्णाङ्क xstatus;
    wait(&xstatus);
    सचे (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  सचे (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  याव ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    अत्थाय (i = 0; i < परिमाण(buf); i++) {
      सचे (buf[i] == 'c')
        nc++;
      सचे (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  सचे (nc == N * SZ && np == N * SZ) {
    exit(0);
  } अञ्ञथा {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
शून्य
fourfiles(वर्ण *s)
{
  पूर्णाङ्क fd, pid, i, j, n, total, pi;
  वर्ण *names[] = {"f0", "f1", "f2", "f3"};
  वर्ण *fname;
  गणना { N = 12, NCHILD = 4, SZ = 500 };

  अत्थाय (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    सचे (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      सचे (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      अत्थाय (i = 0; i < N; i++) {
        सचे ((n = लिख(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  पूर्णाङ्क xstatus;
  अत्थाय (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    सचे (xstatus != 0)
      exit(xstatus);
  }

  अत्थाय (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    याव ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
      अत्थाय (j = 0; j < n; j++) {
        सचे (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    सचे (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
शून्य
createdelete(वर्ण *s)
{
  गणना { N = 20, NCHILD = 4 };
  पूर्णाङ्क pid, i, fd, pi;
  वर्ण name[32];

  अत्थाय (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    सचे (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      अत्थाय (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        सचे (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        सचे (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          सचे (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  पूर्णाङ्क xstatus;
  अत्थाय (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    सचे (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  अत्थाय (i = 0; i < N; i++) {
    अत्थाय (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      सचे ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } अञ्ञथा सचे ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      सचे (fd >= 0)
        close(fd);
    }
  }

  अत्थाय (i = 0; i < N; i++) {
    अत्थाय (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
शून्य
unlinkread(वर्ण *s)
{
  गणना { SZ = 5 };
  पूर्णाङ्क fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  लिख(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  सचे (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  सचे (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  लिख(fd1, "yyy", 3);
  close(fd1);

  सचे (पठ(fd, buf, परिमाण(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  सचे (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  सचे (लिख(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

शून्य
linktest(वर्ण *s)
{
  गणना { SZ = 5 };
  पूर्णाङ्क fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  सचे (लिख(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  सचे (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  सचे (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  सचे (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  सचे (पठ(fd, buf, परिमाण(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  सचे (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  सचे (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  सचे (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
शून्य
concreate(वर्ण *s)
{
  गणना { N = 40 };
  वर्ण file[3];
  पूर्णाङ्क i, pid, n, fd;
  वर्ण fa[N];
  संरचना {
    ushort inum;
    वर्ण name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  अत्थाय (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    सचे (pid && (i % 3) == 1) {
      link("C0", file);
    } अञ्ञथा सचे (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } अञ्ञथा {
      fd = open(file, O_CREATE | O_RDWR);
      सचे (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    सचे (pid == 0) {
      exit(0);
    } अञ्ञथा {
      पूर्णाङ्क xstatus;
      wait(&xstatus);
      सचे (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, परिमाण(fa));
  fd = open(".", 0);
  n = 0;
  याव (पठ(fd, &de, परिमाण(de)) > 0) {
    सचे (de.inum == 0)
      अनुवर्त;
    सचे (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      सचे (i < 0 || i >= परिमाण(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      सचे (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  सचे (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  अत्थाय (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } अञ्ञथा {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    सचे (pid == 0)
      exit(0);
    अञ्ञथा
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
शून्य
linkunlink(वर्ण *s)
{
  पूर्णाङ्क pid, i;

  unlink("x");
  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned पूर्णाङ्क x = (pid ? 1 : 97);
  अत्थाय (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    सचे ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } अञ्ञथा सचे ((x % 3) == 1) {
      link("cat", "x");
    } अञ्ञथा {
      unlink("x");
    }
  }

  सचे (pid)
    wait(0);
  अञ्ञथा
    exit(0);
}

शून्य
subdir(वर्ण *s)
{
  पूर्णाङ्क fd, cc;

  unlink("ff");
  सचे (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  लिख(fd, "ff", 2);
  close(fd);

  सचे (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  सचे (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  लिख(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  सचे (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = पठ(fd, buf, परिमाण(buf));
  सचे (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  सचे (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  सचे (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  सचे (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  सचे (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  सचे (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  सचे (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  सचे (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  सचे (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  सचे (पठ(fd, buf, परिमाण(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  सचे (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  सचे (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  सचे (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  सचे (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  सचे (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  सचे (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  सचे (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  सचे (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  सचे (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  सचे (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  सचे (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  सचे (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  सचे (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  सचे (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  सचे (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  सचे (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  सचे (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  सचे (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  सचे (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  सचे (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  सचे (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
शून्य
bigwrite(वर्ण *s)
{
  पूर्णाङ्क fd, sz;

  unlink("bigwrite");
  अत्थाय (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    सचे (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    पूर्णाङ्क i;
    अत्थाय (i = 0; i < 2; i++) {
      पूर्णाङ्क cc = लिख(fd, buf, sz);
      सचे (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

शून्य
bigfile(वर्ण *s)
{
  गणना { N = 20, SZ = 600 };
  पूर्णाङ्क fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  अत्थाय (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    सचे (लिख(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  सचे (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  अत्थाय (i = 0;; i++) {
    cc = पठ(fd, buf, SZ / 2);
    सचे (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    सचे (cc == 0)
      भङ्ग;
    सचे (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    सचे (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  सचे (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

शून्य
fourteen(वर्ण *s)
{
  पूर्णाङ्क fd;

  // DIRSIZ is 14.

  सचे (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  सचे (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  सचे (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  सचे (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  सचे (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  सचे (mkdir("123456789012345/12345678901234") == 0) {
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

शून्य
rmdot(वर्ण *s)
{
  सचे (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  सचे (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  सचे (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  सचे (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  सचे (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  सचे (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  सचे (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  सचे (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

शून्य
dirfile(वर्ण *s)
{
  पूर्णाङ्क fd;

  fd = open("dirfile", O_CREATE);
  सचे (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  सचे (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  सचे (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  सचे (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  सचे (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  सचे (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  सचे (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  सचे (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  सचे (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  सचे (लिख(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
शून्य
iref(वर्ण *s)
{
  पूर्णाङ्क i, fd;

  अत्थाय (i = 0; i < NINODE + 1; i++) {
    सचे (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    सचे (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    सचे (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    सचे (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  अत्थाय (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
शून्य
forktest(वर्ण *s)
{
  गणना { N = 1000 };
  पूर्णाङ्क n, pid;

  अत्थाय (n = 0; n < N; n++) {
    pid = fork();
    सचे (pid < 0)
      भङ्ग;
    सचे (pid == 0)
      exit(0);
  }

  सचे (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  सचे (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  अत्थाय (; n > 0; n--) {
    सचे (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  सचे (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

शून्य
sbrkbasic(वर्ण *s)
{
  गणना { TOOMUCH = 1024 * 1024 * 1024 };
  पूर्णाङ्क i, pid, xstatus;
  वर्ण *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  सचे (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  सचे (pid == 0) {
    a = sbrk(TOOMUCH);
    सचे (a == (वर्ण *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    अत्थाय (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  सचे (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  अत्थाय (i = 0; i < 5000; i++) {
    b = sbrk(1);
    सचे (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  सचे (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  सचे (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  सचे (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

शून्य
sbrkmuch(वर्ण *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  वर्ण *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  सचे (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (वर्ण *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  सचे (c == (वर्ण *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  सचे (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  सचे (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  सचे (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  सचे (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
शून्य
kernmem(वर्ण *s)
{
  वर्ण *a;
  पूर्णाङ्क pid;

  अत्थाय (a = (वर्ण *)(KERNBASE); a < (वर्ण *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    पूर्णाङ्क xstatus;
    wait(&xstatus);
    सचे (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
शून्य
MAXVAplus(वर्ण *s)
{
  चञ्चल uint64 a = MAXVA;
  अत्थाय (; a != 0; a <<= 1) {
    पूर्णाङ्क pid;
    pid = fork();
    सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    सचे (pid == 0) {
      *(वर्ण *)a = 99;
      printf("%s: oops wrote %p\n", s, (शून्य *)a);
      exit(1);
    }
    पूर्णाङ्क xstatus;
    wait(&xstatus);
    सचे (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
शून्य
sbrkfail(वर्ण *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  पूर्णाङ्क i, xstatus;
  पूर्णाङ्क fds[2];
  वर्ण scratch;
  वर्ण *c, *a;
  पूर्णाङ्क pids[10];
  पूर्णाङ्क pid;
  पूर्णाङ्क failed;

  failed = 0;
  सचे (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  अत्थाय (i = 0; i < परिमाण(pids) / परिमाण(pids[0]); i++) {
    सचे ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      सचे (sbrk(BIG - (uint64)sbrk(0)) == (वर्ण *)SBRK_ERROR)
        लिख(fds[1], "0", 1);
      अञ्ञथा
        लिख(fds[1], "1", 1);
      // sit around until killed
      अत्थाय (;;)
        pause(1000);
    }
    सचे (pids[i] != -1) {
      पठ(fds[0], &scratch, 1);
      सचे (scratch == '0')
        failed = 1;
    }
  }
  सचे (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  अत्थाय (i = 0; i < परिमाण(pids) / परिमाण(pids[0]); i++) {
    सचे (pids[i] == -1)
      अनुवर्त;
    kill(pids[i]);
    wait(0);
  }
  सचे (c == (वर्ण *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  सचे (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    सचे (a == (वर्ण *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  सचे (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
शून्य
sbrkarg(वर्ण *s)
{
  वर्ण *a;
  पूर्णाङ्क fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  सचे (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  सचे ((n = लिख(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  सचे (pipe((पूर्णाङ्क *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

शून्य
validatetest(वर्ण *s)
{
  पूर्णाङ्क hi;
  uint64 p;

  hi = 1100 * 1024;
  अत्थाय (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    सचे (link("nosuchfile", (वर्ण *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
वर्ण uninit[10000];
शून्य
bsstest(वर्ण *s)
{
  पूर्णाङ्क i;

  अत्थाय (i = 0; i < परिमाण(uninit); i++) {
    सचे (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
शून्य
bigargtest(वर्ण *s)
{
  पूर्णाङ्क pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  सचे (pid == 0) {
    स्थिर वर्ण *args[MAXARG];
    पूर्णाङ्क i;
    वर्ण big[400];
    memset(big, ' ', परिमाण(big));
    big[परिमाण(big) - 1] = '\0';
    अत्थाय (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } अञ्ञथा सचे (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  सचे (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  सचे (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
शून्य
fsfull()
{
  पूर्णाङ्क nfiles;
  पूर्णाङ्क fsblocks = 0;

  printf("fsfull test\n");

  अत्थाय (nfiles = 0;; nfiles++) {
    वर्ण name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    पूर्णाङ्क fd = open(name, O_CREATE | O_RDWR);
    सचे (fd < 0) {
      printf("open %s failed\n", name);
      भङ्ग;
    }
    पूर्णाङ्क total = 0;
    याव (1) {
      पूर्णाङ्क cc = लिख(fd, buf, BSIZE);
      सचे (cc < BSIZE)
        भङ्ग;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    सचे (total == 0)
      भङ्ग;
  }

  याव (nfiles >= 0) {
    वर्ण name[64];
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

शून्य
argptest(वर्ण *s)
{
  पूर्णाङ्क fd;
  fd = open("init", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  पठ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
शून्य
stacktest(वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;

  pid = fork();
  सचे (pid == 0) {
    वर्ण *sp = (वर्ण *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } अञ्ञथा सचे (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  सचे (xstatus == -1) // kernel killed child?
    exit(0);
  अञ्ञथा
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
शून्य
nowrite(वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  अत्थाय (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    pid = fork();
    सचे (pid == 0) {
      चञ्चल पूर्णाङ्क *addr = (पूर्णाङ्क *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } अञ्ञथा सचे (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    सचे (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
शून्य *big = (शून्य *)0xeaeb0b5b00002f5e;
शून्य
pgbug(वर्ण *s)
{
  वर्ण *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
शून्य
sbrkbugs(वर्ण *s)
{
  पूर्णाङ्क pid = fork();
  सचे (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  सचे (pid == 0) {
    पूर्णाङ्क sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  सचे (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  सचे (pid == 0) {
    पूर्णाङ्क sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  सचे (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  सचे (pid == 0) {
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
शून्य
sbrklast(वर्ण *s)
{
  uint64 शिखरम् = (uint64)sbrk(0);
  सचे ((शिखरम् % PGSIZE) != 0)
    sbrk(PGSIZE - (शिखरम् % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  शिखरम् = (uint64)sbrk(0);
  वर्ण *p = (वर्ण *)(शिखरम् - 64);
  p[0] = 'x';
  p[1] = '\0';
  पूर्णाङ्क fd = open(p, O_RDWR | O_CREATE);
  लिख(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  पठ(fd, p, 1);
  सचे (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
शून्य
sbrk8000(वर्ण *s)
{
  sbrk(0x80000004);
  चञ्चल वर्ण *शिखरम् = sbrk(0);
  *(शिखरम् - 1) = *(शिखरम् - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
शून्य
badarg(वर्ण *s)
{
  अत्थाय (पूर्णाङ्क i = 0; i < 50000; i++) {
    वर्ण *argv[2];
    argv[0] = (वर्ण *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
शून्य
lazy_alloc(वर्ण *s)
{
  वर्ण *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  सचे (prev_end == (वर्ण *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  अत्थाय (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(वर्ण **)i = i;

  अत्थाय (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    सचे (*(वर्ण **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
शून्य
lazy_unmap(वर्ण *s)
{
  पूर्णाङ्क pid;
  वर्ण *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  सचे (prev_end == (वर्ण *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  अत्थाय (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(वर्ण **)i = i;

  अत्थाय (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    सचे (pid < 0) {
      printf("error forking\n");
      exit(1);
    } अञ्ञथा सचे (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(वर्ण **)i = i;
      exit(0);
    } अञ्ञथा {
      पूर्णाङ्क status;
      wait(&status);
      सचे (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

शून्य
lazy_copy(वर्ण *s)
{
  // copyinstr on lazy page
  {
    वर्ण *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    शून्य *xx = sbrk(0);
    शून्य *ret = sbrk(-(((uint64)xx) + 1));
    सचे (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  अत्थाय (पूर्णाङ्क i = 0; i < परिमाण(bad) / परिमाण(bad[0]); i++) {
    पूर्णाङ्क fd = open("README", 0);
    सचे (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    सचे (पठ(fd, (वर्ण *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    सचे (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    सचे (लिख(fd, (वर्ण *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

शून्य
lazy_copyinstr(वर्ण *s)
{
  वर्ण *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  सचे ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  पूर्णाङ्क fd = open(&p[4095], O_RDONLY);
  सचे (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  संरचना stat st;
  पूर्णाङ्क r = fstat(fd, &st);
  सचे (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  सचे (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

शून्य
lazy_sbrk(वर्ण *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  वर्ण *p = sbrk(0);
  याव ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    सचे (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  पूर्णाङ्क n = TRAPFRAME - PGSIZE - (uint64)p;

  वर्ण *p1 = sbrklazy(n);
  सचे (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  सचे (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  सचे (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  सचे ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  सचे ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

शून्य
partial_write(वर्ण *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  पूर्णाङ्क fd = open("testfile", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  पूर्णाङ्क cc = लिख(fd, "A", 1);
  सचे (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  सचे (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  वर्ण *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  सचे ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = लिख(fd, p - 1, 2);
  सचे (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  वर्ण b;
  cc = पठ(fd, &b, 1);
  सचे (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  सचे (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  अत्थाय (पूर्णाङ्क i = 0; i < 64; i++) {
    वर्ण buf[1024];
    memset(buf, 0, परिमाण(buf));
    cc = लिख(fd, buf, परिमाण(buf));
    सचे (cc != परिमाण(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  सचे (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = पठ(fd, &b, 1);
  सचे (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  सचे (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

शून्य
unlinkcwd(वर्ण *s)
{
  सचे (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  सचे (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  सचे (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  सचे (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  सचे (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  सचे (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  सचे (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

संरचना test {
  शून्य (*f)(वर्ण *);
  वर्ण *s;
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
शून्य
bigdir(वर्ण *s)
{
  गणना { N = 500 };
  पूर्णाङ्क i, fd;
  वर्ण name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  सचे (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  अत्थाय (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    सचे (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  अत्थाय (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    सचे (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
शून्य
manywrites(वर्ण *s)
{
  पूर्णाङ्क nchildren = 4;
  पूर्णाङ्क howmany = 30; // increase to look for deadlock

  अत्थाय (पूर्णाङ्क ci = 0; ci < nchildren; ci++) {
    पूर्णाङ्क pid = fork();
    सचे (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    सचे (pid == 0) {
      वर्ण name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      अत्थाय (पूर्णाङ्क iters = 0; iters < howmany; iters++) {
        अत्थाय (पूर्णाङ्क i = 0; i < ci + 1; i++) {
          पूर्णाङ्क fd = open(name, O_CREATE | O_RDWR);
          सचे (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          पूर्णाङ्क sz = परिमाण(buf);
          पूर्णाङ्क cc = लिख(fd, buf, sz);
          सचे (cc != sz) {
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

  अत्थाय (पूर्णाङ्क ci = 0; ci < nchildren; ci++) {
    पूर्णाङ्क st = 0;
    wait(&st);
    सचे (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
शून्य
badwrite(वर्ण *s)
{
  पूर्णाङ्क assumed_free = 600;

  unlink("junk");
  अत्थाय (पूर्णाङ्क i = 0; i < assumed_free; i++) {
    पूर्णाङ्क fd = open("junk", O_CREATE | O_WRONLY);
    सचे (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    लिख(fd, (वर्ण *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  पूर्णाङ्क fd = open("junk", O_CREATE | O_WRONLY);
  सचे (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  सचे (लिख(fd, "x", 1) != 1) {
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
शून्य
execout(वर्ण *s)
{
  अत्थाय (पूर्णाङ्क avail = 0; avail < 15; avail++) {
    पूर्णाङ्क pid = fork();
    सचे (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } अञ्ञथा सचे (pid == 0) {
      // allocate all of memory.
      याव (1) {
        वर्ण *a = sbrk(PGSIZE);
        सचे (a == SBRK_ERROR)
          भङ्ग;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      अत्थाय (पूर्णाङ्क i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      वर्ण *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } अञ्ञथा {
      wait((पूर्णाङ्क *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
शून्य
diskfull(वर्ण *s)
{
  पूर्णाङ्क fi;
  पूर्णाङ्क done = 0;

  unlink("diskfulldir");

  अत्थाय (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    वर्ण name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    सचे (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      भङ्ग;
    }
    अत्थाय (पूर्णाङ्क i = 0; i < MAXFILE; i++) {
      वर्ण buf[BSIZE];
      सचे (लिख(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        भङ्ग;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  पूर्णाङ्क nzz = 128;
  अत्थाय (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    सचे (fd < 0)
      भङ्ग;
    close(fd);
  }

  // this mkdir() is expected to fail.
  सचे (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  अत्थाय (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  अत्थाय (पूर्णाङ्क i = 0; '0' + i < 0177; i++) {
    वर्ण name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
outofinodes(वर्ण *s)
{
  पूर्णाङ्क nzz = 32 * 32;
  अत्थाय (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    सचे (fd < 0) {
      // failure is eventually expected.
      भङ्ग;
    }
    close(fd);
  }

  अत्थाय (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
linkoverflow(वर्ण *s)
{
  गणना { TARGET = 32768 };
  गणना { DIRS = 64 };
  संरचना stat st;
  पूर्णाङ्क i;

  unlink("/lof");
  पूर्णाङ्क fd = open("/lof", O_CREATE | O_RDWR);
  सचे (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  अत्थाय (i = 0; i < TARGET; i++) {
    पूर्णाङ्क d = i % DIRS;
    पूर्णाङ्क f = i / DIRS;

    वर्ण pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    सचे (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    सचे (link("/lof", pn) < 0) {
      सचे (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      सचे (st.nlink >= 32767) {
        // overflow check succeeded.
        भङ्ग;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    सचे (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  सचे (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  सचे (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

संरचना test slowtests[] = {
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
पूर्णाङ्क
run(शून्य f(वर्ण *), वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;

  printf("test %s: ", s);
  सचे ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  सचे (pid == 0) {
    f(s);
    exit(0);
  } अञ्ञथा {
    wait(&xstatus);
    सचे (xstatus != 0)
      printf("FAILED\n");
    अञ्ञथा
      printf("OK\n");
    पटिनिवत्त xstatus == 0;
  }
}

पूर्णाङ्क
runtests(संरचना test *tests, वर्ण *justone, पूर्णाङ्क continuous)
{
  पूर्णाङ्क ntests = 0;
  अत्थाय (संरचना test *t = tests; t->s != 0; t++) {
    सचे ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      सचे (!run(t->f, t->s)) {
        सचे (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          पटिनिवत्त -1;
        }
      }
    }
  }
  पटिनिवत्त ntests;
}

// use sbrk() to count how many free physical memory pages there are.
पूर्णाङ्क
countfree()
{
  पूर्णाङ्क n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  याव (1) {
    वर्ण *a = sbrk(PGSIZE);
    सचे (a == SBRK_ERROR) {
      भङ्ग;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  पटिनिवत्त n;
}

पूर्णाङ्क
drivetests(पूर्णाङ्क quick, पूर्णाङ्क continuous, वर्ण *justone)
{
  करोहि {
    printf("usertests starting\n");
    पूर्णाङ्क free0 = countfree();
    पूर्णाङ्क free1 = 0;
    पूर्णाङ्क ntests = 0;
    पूर्णाङ्क n;
    n = runtests(quicktests, justone, continuous);
    सचे (n < 0) {
      सचे (continuous != 2) {
        पटिनिवत्त 1;
      }
    } अञ्ञथा {
      ntests += n;
    }
    सचे (!quick) {
      सचे (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      सचे (n < 0) {
        सचे (continuous != 2) {
          पटिनिवत्त 1;
        }
      } अञ्ञथा {
        ntests += n;
      }
    }
    सचे ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      सचे (continuous != 2) {
        पटिनिवत्त 1;
      }
    }
    सचे (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      पटिनिवत्त 1;
    }
  } याव (continuous);
  पटिनिवत्त 0;
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क continuous = 0;
  पूर्णाङ्क quick = 0;
  वर्ण *justone = 0;

  सचे (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } अञ्ञथा सचे (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } अञ्ञथा सचे (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } अञ्ञथा सचे (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } अञ्ञथा सचे (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  सचे (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
