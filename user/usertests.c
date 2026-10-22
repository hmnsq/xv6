#অন্তর্ভাৱ "kernel/param.h"
#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"
#অন্তর্ভাৱ "kernel/fs.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "kernel/syscall.h"
#অন্তর্ভাৱ "kernel/memlayout.h"
#অন্তর্ভাৱ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

ৱর্ণ buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
শূন্য
copyin(ৱর্ণ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  বাবে (পূর্ণাঙ্ক ai = 0; ai < পরিমাণ(addrs) / পরিমাণ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণাঙ্ক fd = open("copyin1", O_CREATE | O_WRONLY);
    যদি (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    পূর্ণাঙ্ক n = লিখ(fd, (শূন্য *)addr, 8192);
    যদি (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (শূন্য *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = লিখ(1, (ৱর্ণ *)addr, 8192);
    যদি (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr, n);
      exit(1);
    }

    পূর্ণাঙ্ক fds[2];
    যদি (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = লিখ(fds[1], (ৱর্ণ *)addr, 8192);
    যদি (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
শূন্য
copyout(ৱর্ণ *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  বাবে (পূর্ণাঙ্ক ai = 0; ai < পরিমাণ(addrs) / পরিমাণ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণাঙ্ক fd = open("README", 0);
    যদি (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    পূর্ণাঙ্ক n = পঠ(fd, (শূন্য *)addr, 8192);
    যদি (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr, n);
      exit(1);
    }
    close(fd);

    পূর্ণাঙ্ক fds[2];
    যদি (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = লিখ(fds[1], "x", 1);
    যদি (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = পঠ(fds[0], (শূন্য *)addr, 8192);
    যদি (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
শূন্য
copyinstr1(ৱর্ণ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  বাবে (পূর্ণাঙ্ক ai = 0; ai < পরিমাণ(addrs) / পরিমাণ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণাঙ্ক fd = open((ৱর্ণ *)addr, O_CREATE | O_WRONLY);
    যদি (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (শূন্য *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
শূন্য
copyinstr2(ৱর্ণ *s)
{
  ৱর্ণ b[MAXPATH + 1];

  বাবে (পূর্ণাঙ্ক i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  পূর্ণাঙ্ক ret = unlink(b);
  যদি (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  পূর্ণাঙ্ক fd = open(b, O_CREATE | O_WRONLY);
  যদি (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  যদি (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ৱর্ণ *args[] = {"xx", 0};
  ret = exec(b, args);
  যদি (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  পূর্ণাঙ্ক pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
    স্থির ৱর্ণ big[PGSIZE + 1];
    বাবে (পূর্ণাঙ্ক i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    ৱর্ণ *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    যদি (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  পূর্ণাঙ্ক st = 0;
  wait(&st);
  যদি (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
শূন্য
copyinstr3(ৱর্ণ *s)
{
  sbrk(8192);
  uint64 শীৰ্ষ = (uint64)sbrk(0);
  যদি ((শীৰ্ষ % PGSIZE) != 0) {
    sbrk(PGSIZE - (শীৰ্ষ % PGSIZE));
  }
  শীৰ্ষ = (uint64)sbrk(0);
  যদি (শীৰ্ষ % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  ৱর্ণ *b = (ৱর্ণ *)(শীৰ্ষ - 1);
  *b = 'x';

  পূর্ণাঙ্ক ret = unlink(b);
  যদি (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  পূর্ণাঙ্ক fd = open(b, O_CREATE | O_WRONLY);
  যদি (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  যদি (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  ৱর্ণ *args[] = {"xx", 0};
  ret = exec(b, args);
  যদি (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
শূন্য
rwsbrk(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, n;

  uint64 a = (uint64)sbrk(8192);

  যদি (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  যদি (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  যদি (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = লিখ(fd, (শূন্য *)(a + PGSIZE), 1024);
  যদি (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (শূন্য *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  যদি (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = পঠ(fd, (শূন্য *)(a + PGSIZE), 10);
  যদি (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (শূন্য *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
শূন্য
truncate1(ৱর্ণ *s)
{
  ৱর্ণ buf[32];

  unlink("truncfile");
  পূর্ণাঙ্ক fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  লিখ(fd1, "abcd", 4);
  close(fd1);

  পূর্ণাঙ্ক fd2 = open("truncfile", O_RDONLY);
  পূর্ণাঙ্ক n = পঠ(fd2, buf, পরিমাণ(buf));
  যদি (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  পূর্ণাঙ্ক fd3 = open("truncfile", O_RDONLY);
  n = পঠ(fd3, buf, পরিমাণ(buf));
  যদি (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = পঠ(fd2, buf, পরিমাণ(buf));
  যদি (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  লিখ(fd1, "abcdef", 6);

  n = পঠ(fd3, buf, পরিমাণ(buf));
  যদি (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = পঠ(fd2, buf, পরিমাণ(buf));
  যদি (n != 2) {
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
শূন্য
truncate2(ৱর্ণ *s)
{
  unlink("truncfile");

  পূর্ণাঙ্ক fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  লিখ(fd1, "abcd", 4);

  পূর্ণাঙ্ক fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  পূর্ণাঙ্ক n = লিখ(fd1, "x", 1);
  যদি (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

শূন্য
truncate3(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  যদি (pid == 0) {
    বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
      ৱর্ণ buf[32];
      পূর্ণাঙ্ক fd = open("truncfile", O_WRONLY);
      যদি (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      পূর্ণাঙ্ক n = লিখ(fd, "1234567890", 10);
      যদি (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      পঠ(fd, buf, পরিমাণ(buf));
      close(fd);
    }
    exit(0);
  }

  বাবে (পূর্ণাঙ্ক i = 0; i < 150; i++) {
    পূর্ণাঙ্ক fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    যদি (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    পূর্ণাঙ্ক n = লিখ(fd, "xxx", 3);
    যদি (n != 3) {
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
শূন্য
iputtest(ৱর্ণ *s)
{
  যদি (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  যদি (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  যদি (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  যদি (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
শূন্য
exitiputtest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, xstatus;

  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    যদি (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    যদি (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    যদি (unlink("../iputdir") < 0) {
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
শূন্য
openiputtest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, xstatus;

  যদি (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণাঙ্ক fd = open("oidir", O_RDWR);
    যদি (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  যদি (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

শূন্য
opentest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd;

  fd = open("echo", 0);
  যদি (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  যদি (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

শূন্য
writetest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd;
  পূর্ণাঙ্ক i;
  গণনা { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  বাবে (i = 0; i < N; i++) {
    যদি (লিখ(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    যদি (লিখ(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = পঠ(fd, buf, N * SZ * 2);
  যদি (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  যদি (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

শূন্য
writebig(ৱর্ণ *s)
{
  পূর্ণাঙ্ক i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  বাবে (i = 0; i < MAXFILE; i++) {
    ((পূর্ণাঙ্ক *)buf)[0] = i;
    যদি (লিখ(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  বাবে (;;) {
    i = পঠ(fd, buf, BSIZE);
    যদি (i == 0) {
      যদি (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      ভঙ্গ;
    } অন্যথা যদি (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    যদি (((পূর্ণাঙ্ক *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((পূর্ণাঙ্ক *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  যদি (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
শূন্য
createtest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক i, fd;
  গণনা { N = 52 };

  ৱর্ণ name[3];
  name[0] = 'a';
  name[2] = '\0';
  বাবে (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  বাবে (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

শূন্য
dirtest(ৱর্ণ *s)
{
  যদি (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  যদি (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  যদি (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  যদি (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

শূন্য
exectest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, xstatus, pid;
  ৱর্ণ *echoargv[] = {"echo", "OK", 0};
  ৱর্ণ buf[3];

  unlink("echo-ok");
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণাঙ্ক errfd = dup(1);
    যদি (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    যদি (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    যদি (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    যদি (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  যদি (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  যদি (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  যদি (পঠ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  যদি (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  অন্যথা {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

শূন্য
pipe1(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fds[2], pid, xstatus;
  পূর্ণাঙ্ক seq, i, n, cc, total;
  গণনা { N = 5, SZ = 1033 };

  যদি (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  যদি (pid == 0) {
    close(fds[0]);
    বাবে (n = 0; n < N; n++) {
      বাবে (i = 0; i < SZ; i++)
        buf[i] = seq++;
      যদি (লিখ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } অন্যথা যদি (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    যেতিয়ালৈ ((n = পঠ(fds[0], buf, cc)) > 0) {
      বাবে (i = 0; i < n; i++) {
        যদি ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ঘূৰাই;
        }
      }
      total += n;
      cc = cc * 2;
      যদি (cc > পরিমাণ(buf))
        cc = পরিমাণ(buf);
    }
    যদি (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } অন্যথা {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
শূন্য
killstatus(ৱর্ণ *s)
{
  পূর্ণাঙ্ক xst;

  বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
    পূর্ণাঙ্ক pid1 = fork();
    যদি (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid1 == 0) {
      যেতিয়ালৈ (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    যদি (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
শূন্য
killzero(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, xst;

  kill(0);

  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    exit(7);
  }
  যদি (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  যদি (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
শূন্য
preempt(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid1, pid2, pid3;
  পূর্ণাঙ্ক pfds[2];

  pid1 = fork();
  যদি (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  যদি (pid1 == 0)
    বাবে (;;)
      ;

  pid2 = fork();
  যদি (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid2 == 0)
    বাবে (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  যদি (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid3 == 0) {
    close(pfds[0]);
    যদি (লিখ(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    বাবে (;;)
      ;
  }

  close(pfds[1]);
  যদি (পঠ(pfds[0], buf, পরিমাণ(buf)) != 1) {
    printf("%s: preempt read error", s);
    ঘূৰাই;
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
শূন্য
exitwait(ৱর্ণ *s)
{
  পূর্ণাঙ্ক i, pid;

  বাবে (i = 0; i < 100; i++) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid) {
      পূর্ণাঙ্ক xstate;
      যদি (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      যদি (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } অন্যথা {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
শূন্য
reparent(ৱর্ণ *s)
{
  পূর্ণাঙ্ক master_pid = getpid();
  বাবে (পূর্ণাঙ্ক i = 0; i < 200; i++) {
    পূর্ণাঙ্ক pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid) {
      যদি (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } অন্যথা {
      পূর্ণাঙ্ক pid2 = fork();
      যদি (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
শূন্য
twochildren(ৱর্ণ *s)
{
  বাবে (পূর্ণাঙ্ক i = 0; i < 1000; i++) {
    পূর্ণাঙ্ক pid1 = fork();
    যদি (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid1 == 0) {
      exit(0);
    } অন্যথা {
      পূর্ণাঙ্ক pid2 = fork();
      যদি (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      যদি (pid2 == 0) {
        exit(0);
      } অন্যথা {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
শূন্য
forkfork(ৱর্ণ *s)
{
  গণনা { N = 2 };

  বাবে (পূর্ণাঙ্ক i = 0; i < N; i++) {
    পূর্ণাঙ্ক pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    যদি (pid == 0) {
      বাবে (পূর্ণাঙ্ক j = 0; j < 200; j++) {
        পূর্ণাঙ্ক pid1 = fork();
        যদি (pid1 < 0) {
          exit(1);
        }
        যদি (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  পূর্ণাঙ্ক xstatus;
  বাবে (পূর্ণাঙ্ক i = 0; i < N; i++) {
    wait(&xstatus);
    যদি (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

শূন্য
forkforkfork(ৱর্ণ *s)
{
  unlink("stopforking");

  পূর্ণাঙ্ক pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  যদি (pid == 0) {
    যেতিয়ালৈ (1) {
      পূর্ণাঙ্ক fd = open("stopforking", 0);
      যদি (fd >= 0) {
        exit(0);
      }
      যদি (fork() < 0) {
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
শূন্য
reparent2(ৱর্ণ *s)
{
  বাবে (পূর্ণাঙ্ক i = 0; i < 800; i++) {
    পূর্ণাঙ্ক pid1 = fork();
    যদি (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    যদি (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
শূন্য
mem(ৱর্ণ *s)
{
  শূন্য *m1, *m2;
  পূর্ণাঙ্ক pid;

  যদি ((pid = fork()) == 0) {
    m1 = 0;
    যেতিয়ালৈ ((m2 = malloc(10001)) != 0) {
      *(ৱর্ণ **)m2 = m1;
      m1 = m2;
    }
    যেতিয়ালৈ (m1) {
      m2 = *(ৱর্ণ **)m1;
      মুক্ত(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    যদি (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    মুক্ত(m1);
    exit(0);
  } অন্যথা {
    পূর্ণাঙ্ক xstatus;
    wait(&xstatus);
    যদি (xstatus == -1) {
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
শূন্য
sharedfd(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, pid, i, n, nc, np;
  গণনা { N = 1000, SZ = 10 };
  ৱর্ণ buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', পরিমাণ(buf));
  বাবে (i = 0; i < N; i++) {
    যদি (লিখ(fd, buf, পরিমাণ(buf)) != পরিমাণ(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  যদি (pid == 0) {
    exit(0);
  } অন্যথা {
    পূর্ণাঙ্ক xstatus;
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  যদি (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  যেতিয়ালৈ ((n = পঠ(fd, buf, পরিমাণ(buf))) > 0) {
    বাবে (i = 0; i < পরিমাণ(buf); i++) {
      যদি (buf[i] == 'c')
        nc++;
      যদি (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  যদি (nc == N * SZ && np == N * SZ) {
    exit(0);
  } অন্যথা {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
শূন্য
fourfiles(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, pid, i, j, n, total, pi;
  ৱর্ণ *names[] = {"f0", "f1", "f2", "f3"};
  ৱর্ণ *fname;
  গণনা { N = 12, NCHILD = 4, SZ = 500 };

  বাবে (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    যদি (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      যদি (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      বাবে (i = 0; i < N; i++) {
        যদি ((n = লিখ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  পূর্ণাঙ্ক xstatus;
  বাবে (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(xstatus);
  }

  বাবে (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    যেতিয়ালৈ ((n = পঠ(fd, buf, পরিমাণ(buf))) > 0) {
      বাবে (j = 0; j < n; j++) {
        যদি (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    যদি (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
শূন্য
createdelete(ৱর্ণ *s)
{
  গণনা { N = 20, NCHILD = 4 };
  পূর্ণাঙ্ক pid, i, fd, pi;
  ৱর্ণ name[32];

  বাবে (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    যদি (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      বাবে (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        যদি (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        যদি (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          যদি (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  পূর্ণাঙ্ক xstatus;
  বাবে (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  বাবে (i = 0; i < N; i++) {
    বাবে (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      যদি ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } অন্যথা যদি ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      যদি (fd >= 0)
        close(fd);
    }
  }

  বাবে (i = 0; i < N; i++) {
    বাবে (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
শূন্য
unlinkread(ৱর্ণ *s)
{
  গণনা { SZ = 5 };
  পূর্ণাঙ্ক fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  লিখ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  যদি (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  যদি (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  লিখ(fd1, "yyy", 3);
  close(fd1);

  যদি (পঠ(fd, buf, পরিমাণ(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  যদি (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  যদি (লিখ(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

শূন্য
linktest(ৱর্ণ *s)
{
  গণনা { SZ = 5 };
  পূর্ণাঙ্ক fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  যদি (লিখ(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  যদি (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  যদি (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  যদি (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  যদি (পঠ(fd, buf, পরিমাণ(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  যদি (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  যদি (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  যদি (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
শূন্য
concreate(ৱর্ণ *s)
{
  গণনা { N = 40 };
  ৱর্ণ file[3];
  পূর্ণাঙ্ক i, pid, n, fd;
  ৱর্ণ fa[N];
  সংরচনা {
    ushort inum;
    ৱর্ণ name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  বাবে (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    যদি (pid && (i % 3) == 1) {
      link("C0", file);
    } অন্যথা যদি (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } অন্যথা {
      fd = open(file, O_CREATE | O_RDWR);
      যদি (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    যদি (pid == 0) {
      exit(0);
    } অন্যথা {
      পূর্ণাঙ্ক xstatus;
      wait(&xstatus);
      যদি (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, পরিমাণ(fa));
  fd = open(".", 0);
  n = 0;
  যেতিয়ালৈ (পঠ(fd, &de, পরিমাণ(de)) > 0) {
    যদি (de.inum == 0)
      অনুৱর্ত;
    যদি (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      যদি (i < 0 || i >= পরিমাণ(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      যদি (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  যদি (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  বাবে (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } অন্যথা {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    যদি (pid == 0)
      exit(0);
    অন্যথা
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
শূন্য
linkunlink(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, i;

  unlink("x");
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned পূর্ণাঙ্ক x = (pid ? 1 : 97);
  বাবে (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    যদি ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } অন্যথা যদি ((x % 3) == 1) {
      link("cat", "x");
    } অন্যথা {
      unlink("x");
    }
  }

  যদি (pid)
    wait(0);
  অন্যথা
    exit(0);
}

শূন্য
subdir(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, cc;

  unlink("ff");
  যদি (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  লিখ(fd, "ff", 2);
  close(fd);

  যদি (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  যদি (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  লিখ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  যদি (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = পঠ(fd, buf, পরিমাণ(buf));
  যদি (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  যদি (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  যদি (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  যদি (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  যদি (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  যদি (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  যদি (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  যদি (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  যদি (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  যদি (পঠ(fd, buf, পরিমাণ(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  যদি (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  যদি (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  যদি (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  যদি (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  যদি (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  যদি (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  যদি (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  যদি (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  যদি (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  যদি (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  যদি (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  যদি (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  যদি (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  যদি (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  যদি (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  যদি (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  যদি (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  যদি (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  যদি (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  যদি (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  যদি (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
শূন্য
bigwrite(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd, sz;

  unlink("bigwrite");
  বাবে (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    যদি (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    পূর্ণাঙ্ক i;
    বাবে (i = 0; i < 2; i++) {
      পূর্ণাঙ্ক cc = লিখ(fd, buf, sz);
      যদি (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

শূন্য
bigfile(ৱর্ণ *s)
{
  গণনা { N = 20, SZ = 600 };
  পূর্ণাঙ্ক fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  বাবে (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    যদি (লিখ(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  যদি (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  বাবে (i = 0;; i++) {
    cc = পঠ(fd, buf, SZ / 2);
    যদি (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    যদি (cc == 0)
      ভঙ্গ;
    যদি (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    যদি (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  যদি (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

শূন্য
fourteen(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd;

  // DIRSIZ is 14.

  যদি (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  যদি (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  যদি (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  যদি (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  যদি (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  যদি (mkdir("123456789012345/12345678901234") == 0) {
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

শূন্য
rmdot(ৱর্ণ *s)
{
  যদি (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  যদি (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  যদি (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  যদি (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  যদি (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  যদি (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  যদি (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  যদি (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

শূন্য
dirfile(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd;

  fd = open("dirfile", O_CREATE);
  যদি (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  যদি (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  যদি (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  যদি (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  যদি (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  যদি (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  যদি (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  যদি (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  যদি (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  যদি (লিখ(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
শূন্য
iref(ৱর্ণ *s)
{
  পূর্ণাঙ্ক i, fd;

  বাবে (i = 0; i < NINODE + 1; i++) {
    যদি (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    যদি (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    যদি (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    যদি (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  বাবে (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
শূন্য
forktest(ৱর্ণ *s)
{
  গণনা { N = 1000 };
  পূর্ণাঙ্ক n, pid;

  বাবে (n = 0; n < N; n++) {
    pid = fork();
    যদি (pid < 0)
      ভঙ্গ;
    যদি (pid == 0)
      exit(0);
  }

  যদি (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  যদি (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  বাবে (; n > 0; n--) {
    যদি (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  যদি (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

শূন্য
sbrkbasic(ৱর্ণ *s)
{
  গণনা { TOOMUCH = 1024 * 1024 * 1024 };
  পূর্ণাঙ্ক i, pid, xstatus;
  ৱর্ণ *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  যদি (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  যদি (pid == 0) {
    a = sbrk(TOOMUCH);
    যদি (a == (ৱর্ণ *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    বাবে (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  যদি (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  বাবে (i = 0; i < 5000; i++) {
    b = sbrk(1);
    যদি (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  যদি (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  যদি (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  যদি (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

শূন্য
sbrkmuch(ৱর্ণ *s)
{
  গণনা { BIG = 100 * 1024 * 1024 };
  ৱর্ণ *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  যদি (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (ৱর্ণ *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  যদি (c == (ৱর্ণ *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  যদি (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  যদি (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  যদি (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  যদি (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
শূন্য
kernmem(ৱর্ণ *s)
{
  ৱর্ণ *a;
  পূর্ণাঙ্ক pid;

  বাবে (a = (ৱর্ণ *)(KERNBASE); a < (ৱর্ণ *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    পূর্ণাঙ্ক xstatus;
    wait(&xstatus);
    যদি (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
শূন্য
MAXVAplus(ৱর্ণ *s)
{
  চঞ্চল uint64 a = MAXVA;
  বাবে (; a != 0; a <<= 1) {
    পূর্ণাঙ্ক pid;
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid == 0) {
      *(ৱর্ণ *)a = 99;
      printf("%s: oops wrote %p\n", s, (শূন্য *)a);
      exit(1);
    }
    পূর্ণাঙ্ক xstatus;
    wait(&xstatus);
    যদি (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
শূন্য
sbrkfail(ৱর্ণ *s)
{
  গণনা { BIG = 100 * 1024 * 1024 };
  পূর্ণাঙ্ক i, xstatus;
  পূর্ণাঙ্ক fds[2];
  ৱর্ণ scratch;
  ৱর্ণ *c, *a;
  পূর্ণাঙ্ক pids[10];
  পূর্ণাঙ্ক pid;
  পূর্ণাঙ্ক failed;

  failed = 0;
  যদি (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  বাবে (i = 0; i < পরিমাণ(pids) / পরিমাণ(pids[0]); i++) {
    যদি ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      যদি (sbrk(BIG - (uint64)sbrk(0)) == (ৱর্ণ *)SBRK_ERROR)
        লিখ(fds[1], "0", 1);
      অন্যথা
        লিখ(fds[1], "1", 1);
      // sit around until killed
      বাবে (;;)
        pause(1000);
    }
    যদি (pids[i] != -1) {
      পঠ(fds[0], &scratch, 1);
      যদি (scratch == '0')
        failed = 1;
    }
  }
  যদি (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  বাবে (i = 0; i < পরিমাণ(pids) / পরিমাণ(pids[0]); i++) {
    যদি (pids[i] == -1)
      অনুৱর্ত;
    kill(pids[i]);
    wait(0);
  }
  যদি (c == (ৱর্ণ *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    যদি (a == (ৱর্ণ *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  যদি (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
শূন্য
sbrkarg(ৱর্ণ *s)
{
  ৱর্ণ *a;
  পূর্ণাঙ্ক fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  যদি (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  যদি ((n = লিখ(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  যদি (pipe((পূর্ণাঙ্ক *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

শূন্য
validatetest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক hi;
  uint64 p;

  hi = 1100 * 1024;
  বাবে (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    যদি (link("nosuchfile", (ৱর্ণ *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
ৱর্ণ uninit[10000];
শূন্য
bsstest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক i;

  বাবে (i = 0; i < পরিমাণ(uninit); i++) {
    যদি (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
শূন্য
bigargtest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  যদি (pid == 0) {
    স্থির ৱর্ণ *args[MAXARG];
    পূর্ণাঙ্ক i;
    ৱর্ণ big[400];
    memset(big, ' ', পরিমাণ(big));
    big[পরিমাণ(big) - 1] = '\0';
    বাবে (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } অন্যথা যদি (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  যদি (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  যদি (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
শূন্য
fsfull()
{
  পূর্ণাঙ্ক nfiles;
  পূর্ণাঙ্ক fsblocks = 0;

  printf("fsfull test\n");

  বাবে (nfiles = 0;; nfiles++) {
    ৱর্ণ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    পূর্ণাঙ্ক fd = open(name, O_CREATE | O_RDWR);
    যদি (fd < 0) {
      printf("open %s failed\n", name);
      ভঙ্গ;
    }
    পূর্ণাঙ্ক total = 0;
    যেতিয়ালৈ (1) {
      পূর্ণাঙ্ক cc = লিখ(fd, buf, BSIZE);
      যদি (cc < BSIZE)
        ভঙ্গ;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    যদি (total == 0)
      ভঙ্গ;
  }

  যেতিয়ালৈ (nfiles >= 0) {
    ৱর্ণ name[64];
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

শূন্য
argptest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fd;
  fd = open("init", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  পঠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
শূন্য
stacktest(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid;
  পূর্ণাঙ্ক xstatus;

  pid = fork();
  যদি (pid == 0) {
    ৱর্ণ *sp = (ৱর্ণ *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } অন্যথা যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  যদি (xstatus == -1) // kernel killed child?
    exit(0);
  অন্যথা
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
শূন্য
nowrite(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid;
  পূর্ণাঙ্ক xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  বাবে (পূর্ণাঙ্ক ai = 0; ai < পরিমাণ(addrs) / পরিমাণ(addrs[0]); ai++) {
    pid = fork();
    যদি (pid == 0) {
      চঞ্চল পূর্ণাঙ্ক *addr = (পূর্ণাঙ্ক *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } অন্যথা যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    যদি (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
শূন্য *big = (শূন্য *)0xeaeb0b5b00002f5e;
শূন্য
pgbug(ৱর্ণ *s)
{
  ৱর্ণ *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
শূন্য
sbrkbugs(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণাঙ্ক sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণাঙ্ক sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
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
শূন্য
sbrklast(ৱর্ণ *s)
{
  uint64 শীৰ্ষ = (uint64)sbrk(0);
  যদি ((শীৰ্ষ % PGSIZE) != 0)
    sbrk(PGSIZE - (শীৰ্ষ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  শীৰ্ষ = (uint64)sbrk(0);
  ৱর্ণ *p = (ৱর্ণ *)(শীৰ্ষ - 64);
  p[0] = 'x';
  p[1] = '\0';
  পূর্ণাঙ্ক fd = open(p, O_RDWR | O_CREATE);
  লিখ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  পঠ(fd, p, 1);
  যদি (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
শূন্য
sbrk8000(ৱর্ণ *s)
{
  sbrk(0x80000004);
  চঞ্চল ৱর্ণ *শীৰ্ষ = sbrk(0);
  *(শীৰ্ষ - 1) = *(শীৰ্ষ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
শূন্য
badarg(ৱর্ণ *s)
{
  বাবে (পূর্ণাঙ্ক i = 0; i < 50000; i++) {
    ৱর্ণ *argv[2];
    argv[0] = (ৱর্ণ *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
শূন্য
lazy_alloc(ৱর্ণ *s)
{
  ৱর্ণ *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  যদি (prev_end == (ৱর্ণ *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  বাবে (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(ৱর্ণ **)i = i;

  বাবে (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    যদি (*(ৱর্ণ **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
শূন্য
lazy_unmap(ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid;
  ৱর্ণ *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  যদি (prev_end == (ৱর্ণ *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  বাবে (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(ৱর্ণ **)i = i;

  বাবে (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    যদি (pid < 0) {
      printf("error forking\n");
      exit(1);
    } অন্যথা যদি (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(ৱর্ণ **)i = i;
      exit(0);
    } অন্যথা {
      পূর্ণাঙ্ক status;
      wait(&status);
      যদি (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

শূন্য
lazy_copy(ৱর্ণ *s)
{
  // copyinstr on lazy page
  {
    ৱর্ণ *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    শূন্য *xx = sbrk(0);
    শূন্য *ret = sbrk(-(((uint64)xx) + 1));
    যদি (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  বাবে (পূর্ণাঙ্ক i = 0; i < পরিমাণ(bad) / পরিমাণ(bad[0]); i++) {
    পূর্ণাঙ্ক fd = open("README", 0);
    যদি (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    যদি (পঠ(fd, (ৱর্ণ *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    যদি (লিখ(fd, (ৱর্ণ *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

শূন্য
lazy_copyinstr(ৱর্ণ *s)
{
  ৱর্ণ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  যদি ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  পূর্ণাঙ্ক fd = open(&p[4095], O_RDONLY);
  যদি (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  সংরচনা stat st;
  পূর্ণাঙ্ক r = fstat(fd, &st);
  যদি (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  যদি (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

শূন্য
lazy_sbrk(ৱর্ণ *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  ৱর্ণ *p = sbrk(0);
  যেতিয়ালৈ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    যদি (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  পূর্ণাঙ্ক n = TRAPFRAME - PGSIZE - (uint64)p;

  ৱর্ণ *p1 = sbrklazy(n);
  যদি (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  যদি (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  যদি (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  যদি ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  যদি ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

শূন্য
partial_write(ৱর্ণ *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  পূর্ণাঙ্ক fd = open("testfile", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  পূর্ণাঙ্ক cc = লিখ(fd, "A", 1);
  যদি (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ৱর্ণ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  যদি ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = লিখ(fd, p - 1, 2);
  যদি (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  ৱর্ণ b;
  cc = পঠ(fd, &b, 1);
  যদি (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  যদি (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  বাবে (পূর্ণাঙ্ক i = 0; i < 64; i++) {
    ৱর্ণ buf[1024];
    memset(buf, 0, পরিমাণ(buf));
    cc = লিখ(fd, buf, পরিমাণ(buf));
    যদি (cc != পরিমাণ(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  যদি (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = পঠ(fd, &b, 1);
  যদি (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  যদি (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

শূন্য
unlinkcwd(ৱর্ণ *s)
{
  যদি (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  যদি (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  যদি (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  যদি (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  যদি (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  যদি (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  যদি (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

সংরচনা test {
  শূন্য (*f)(ৱর্ণ *);
  ৱর্ণ *s;
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
শূন্য
bigdir(ৱর্ণ *s)
{
  গণনা { N = 500 };
  পূর্ণাঙ্ক i, fd;
  ৱর্ণ name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  যদি (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  বাবে (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    যদি (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  বাবে (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    যদি (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
শূন্য
manywrites(ৱর্ণ *s)
{
  পূর্ণাঙ্ক nchildren = 4;
  পূর্ণাঙ্ক howmany = 30; // increase to look for deadlock

  বাবে (পূর্ণাঙ্ক ci = 0; ci < nchildren; ci++) {
    পূর্ণাঙ্ক pid = fork();
    যদি (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    যদি (pid == 0) {
      ৱর্ণ name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      বাবে (পূর্ণাঙ্ক iters = 0; iters < howmany; iters++) {
        বাবে (পূর্ণাঙ্ক i = 0; i < ci + 1; i++) {
          পূর্ণাঙ্ক fd = open(name, O_CREATE | O_RDWR);
          যদি (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          পূর্ণাঙ্ক sz = পরিমাণ(buf);
          পূর্ণাঙ্ক cc = লিখ(fd, buf, sz);
          যদি (cc != sz) {
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

  বাবে (পূর্ণাঙ্ক ci = 0; ci < nchildren; ci++) {
    পূর্ণাঙ্ক st = 0;
    wait(&st);
    যদি (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
শূন্য
badwrite(ৱর্ণ *s)
{
  পূর্ণাঙ্ক assumed_free = 600;

  unlink("junk");
  বাবে (পূর্ণাঙ্ক i = 0; i < assumed_free; i++) {
    পূর্ণাঙ্ক fd = open("junk", O_CREATE | O_WRONLY);
    যদি (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    লিখ(fd, (ৱর্ণ *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  পূর্ণাঙ্ক fd = open("junk", O_CREATE | O_WRONLY);
  যদি (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  যদি (লিখ(fd, "x", 1) != 1) {
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
শূন্য
execout(ৱর্ণ *s)
{
  বাবে (পূর্ণাঙ্ক avail = 0; avail < 15; avail++) {
    পূর্ণাঙ্ক pid = fork();
    যদি (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } অন্যথা যদি (pid == 0) {
      // allocate all of memory.
      যেতিয়ালৈ (1) {
        ৱর্ণ *a = sbrk(PGSIZE);
        যদি (a == SBRK_ERROR)
          ভঙ্গ;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      বাবে (পূর্ণাঙ্ক i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      ৱর্ণ *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } অন্যথা {
      wait((পূর্ণাঙ্ক *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
শূন্য
diskfull(ৱর্ণ *s)
{
  পূর্ণাঙ্ক fi;
  পূর্ণাঙ্ক done = 0;

  unlink("diskfulldir");

  বাবে (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    ৱর্ণ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    পূর্ণাঙ্ক fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      ভঙ্গ;
    }
    বাবে (পূর্ণাঙ্ক i = 0; i < MAXFILE; i++) {
      ৱর্ণ buf[BSIZE];
      যদি (লিখ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        ভঙ্গ;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  পূর্ণাঙ্ক nzz = 128;
  বাবে (পূর্ণাঙ্ক i = 0; i < nzz; i++) {
    ৱর্ণ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    পূর্ণাঙ্ক fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0)
      ভঙ্গ;
    close(fd);
  }

  // this mkdir() is expected to fail.
  যদি (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  বাবে (পূর্ণাঙ্ক i = 0; i < nzz; i++) {
    ৱর্ণ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  বাবে (পূর্ণাঙ্ক i = 0; '0' + i < 0177; i++) {
    ৱর্ণ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

শূন্য
outofinodes(ৱর্ণ *s)
{
  পূর্ণাঙ্ক nzz = 32 * 32;
  বাবে (পূর্ণাঙ্ক i = 0; i < nzz; i++) {
    ৱর্ণ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    পূর্ণাঙ্ক fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      // failure is eventually expected.
      ভঙ্গ;
    }
    close(fd);
  }

  বাবে (পূর্ণাঙ্ক i = 0; i < nzz; i++) {
    ৱর্ণ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

শূন্য
linkoverflow(ৱর্ণ *s)
{
  গণনা { TARGET = 32768 };
  গণনা { DIRS = 64 };
  সংরচনা stat st;
  পূর্ণাঙ্ক i;

  unlink("/lof");
  পূর্ণাঙ্ক fd = open("/lof", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  বাবে (i = 0; i < TARGET; i++) {
    পূর্ণাঙ্ক d = i % DIRS;
    পূর্ণাঙ্ক f = i / DIRS;

    ৱর্ণ pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    যদি (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    যদি (link("/lof", pn) < 0) {
      যদি (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      যদি (st.nlink >= 32767) {
        // overflow check succeeded.
        ভঙ্গ;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    যদি (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  যদি (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  যদি (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

সংরচনা test slowtests[] = {
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
পূর্ণাঙ্ক
run(শূন্য f(ৱর্ণ *), ৱর্ণ *s)
{
  পূর্ণাঙ্ক pid;
  পূর্ণাঙ্ক xstatus;

  printf("test %s: ", s);
  যদি ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  যদি (pid == 0) {
    f(s);
    exit(0);
  } অন্যথা {
    wait(&xstatus);
    যদি (xstatus != 0)
      printf("FAILED\n");
    অন্যথা
      printf("OK\n");
    ঘূৰাই xstatus == 0;
  }
}

পূর্ণাঙ্ক
runtests(সংরচনা test *tests, ৱর্ণ *justone, পূর্ণাঙ্ক continuous)
{
  পূর্ণাঙ্ক ntests = 0;
  বাবে (সংরচনা test *t = tests; t->s != 0; t++) {
    যদি ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      যদি (!run(t->f, t->s)) {
        যদি (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ঘূৰাই -1;
        }
      }
    }
  }
  ঘূৰাই ntests;
}

// use sbrk() to count how many free physical memory pages there are.
পূর্ণাঙ্ক
countfree()
{
  পূর্ণাঙ্ক n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  যেতিয়ালৈ (1) {
    ৱর্ণ *a = sbrk(PGSIZE);
    যদি (a == SBRK_ERROR) {
      ভঙ্গ;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ঘূৰাই n;
}

পূর্ণাঙ্ক
drivetests(পূর্ণাঙ্ক quick, পূর্ণাঙ্ক continuous, ৱর্ণ *justone)
{
  কৰা {
    printf("usertests starting\n");
    পূর্ণাঙ্ক free0 = countfree();
    পূর্ণাঙ্ক free1 = 0;
    পূর্ণাঙ্ক ntests = 0;
    পূর্ণাঙ্ক n;
    n = runtests(quicktests, justone, continuous);
    যদি (n < 0) {
      যদি (continuous != 2) {
        ঘূৰাই 1;
      }
    } অন্যথা {
      ntests += n;
    }
    যদি (!quick) {
      যদি (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      যদি (n < 0) {
        যদি (continuous != 2) {
          ঘূৰাই 1;
        }
      } অন্যথা {
        ntests += n;
      }
    }
    যদি ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      যদি (continuous != 2) {
        ঘূৰাই 1;
      }
    }
    যদি (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ঘূৰাই 1;
    }
  } যেতিয়ালৈ (continuous);
  ঘূৰাই 0;
}

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক continuous = 0;
  পূর্ণাঙ্ক quick = 0;
  ৱর্ণ *justone = 0;

  যদি (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } অন্যথা যদি (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } অন্যথা যদি (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } অন্যথা যদি (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } অন্যথা যদি (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  যদি (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
