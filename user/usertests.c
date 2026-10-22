#অন্তর্ভাব "kernel/param.h"
#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "kernel/syscall.h"
#অন্তর্ভাব "kernel/memlayout.h"
#অন্তর্ভাব "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

অক্ষর buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
শূন্য
copyin(অক্ষর *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  জন্য (পূর্ণসংখ্যা ai = 0; ai < আকার(addrs) / আকার(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণসংখ্যা fd = open("copyin1", O_CREATE | O_WRONLY);
    যদি (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    পূর্ণসংখ্যা n = লিখ(fd, (শূন্য *)addr, 8192);
    যদি (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (শূন্য *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = লিখ(1, (অক্ষর *)addr, 8192);
    যদি (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr, n);
      exit(1);
    }

    পূর্ণসংখ্যা fds[2];
    যদি (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = লিখ(fds[1], (অক্ষর *)addr, 8192);
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
copyout(অক্ষর *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  জন্য (পূর্ণসংখ্যা ai = 0; ai < আকার(addrs) / আকার(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণসংখ্যা fd = open("README", 0);
    যদি (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    পূর্ণসংখ্যা n = পঠ(fd, (শূন্য *)addr, 8192);
    যদি (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (শূন্য *)addr, n);
      exit(1);
    }
    close(fd);

    পূর্ণসংখ্যা fds[2];
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
copyinstr1(অক্ষর *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  জন্য (পূর্ণসংখ্যা ai = 0; ai < আকার(addrs) / আকার(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    পূর্ণসংখ্যা fd = open((অক্ষর *)addr, O_CREATE | O_WRONLY);
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
copyinstr2(অক্ষর *s)
{
  অক্ষর b[MAXPATH + 1];

  জন্য (পূর্ণসংখ্যা i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  পূর্ণসংখ্যা ret = unlink(b);
  যদি (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  পূর্ণসংখ্যা fd = open(b, O_CREATE | O_WRONLY);
  যদি (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  যদি (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  অক্ষর *args[] = {"xx", 0};
  ret = exec(b, args);
  যদি (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  পূর্ণসংখ্যা pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
    স্থির অক্ষর big[PGSIZE + 1];
    জন্য (পূর্ণসংখ্যা i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    অক্ষর *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    যদি (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  পূর্ণসংখ্যা st = 0;
  wait(&st);
  যদি (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
শূন্য
copyinstr3(অক্ষর *s)
{
  sbrk(8192);
  uint64 শীর্ষ_2 = (uint64)sbrk(0);
  যদি ((শীর্ষ_2 % PGSIZE) != 0) {
    sbrk(PGSIZE - (শীর্ষ_2 % PGSIZE));
  }
  শীর্ষ_2 = (uint64)sbrk(0);
  যদি (শীর্ষ_2 % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  অক্ষর *b = (অক্ষর *)(শীর্ষ_2 - 1);
  *b = 'x';

  পূর্ণসংখ্যা ret = unlink(b);
  যদি (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  পূর্ণসংখ্যা fd = open(b, O_CREATE | O_WRONLY);
  যদি (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  যদি (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  অক্ষর *args[] = {"xx", 0};
  ret = exec(b, args);
  যদি (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
শূন্য
rwsbrk(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, n;

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
truncate1(অক্ষর *s)
{
  অক্ষর buf[32];

  unlink("truncfile");
  পূর্ণসংখ্যা fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  লিখ(fd1, "abcd", 4);
  close(fd1);

  পূর্ণসংখ্যা fd2 = open("truncfile", O_RDONLY);
  পূর্ণসংখ্যা n = পঠ(fd2, buf, আকার(buf));
  যদি (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  পূর্ণসংখ্যা fd3 = open("truncfile", O_RDONLY);
  n = পঠ(fd3, buf, আকার(buf));
  যদি (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = পঠ(fd2, buf, আকার(buf));
  যদি (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  লিখ(fd1, "abcdef", 6);

  n = পঠ(fd3, buf, আকার(buf));
  যদি (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = পঠ(fd2, buf, আকার(buf));
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
truncate2(অক্ষর *s)
{
  unlink("truncfile");

  পূর্ণসংখ্যা fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  লিখ(fd1, "abcd", 4);

  পূর্ণসংখ্যা fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  পূর্ণসংখ্যা n = লিখ(fd1, "x", 1);
  যদি (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

শূন্য
truncate3(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  যদি (pid == 0) {
    জন্য (পূর্ণসংখ্যা i = 0; i < 100; i++) {
      অক্ষর buf[32];
      পূর্ণসংখ্যা fd = open("truncfile", O_WRONLY);
      যদি (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      পূর্ণসংখ্যা n = লিখ(fd, "1234567890", 10);
      যদি (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      পঠ(fd, buf, আকার(buf));
      close(fd);
    }
    exit(0);
  }

  জন্য (পূর্ণসংখ্যা i = 0; i < 150; i++) {
    পূর্ণসংখ্যা fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    যদি (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    পূর্ণসংখ্যা n = লিখ(fd, "xxx", 3);
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
iputtest(অক্ষর *s)
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
exitiputtest(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, xstatus;

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
openiputtest(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, xstatus;

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
    পূর্ণসংখ্যা fd = open("oidir", O_RDWR);
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
opentest(অক্ষর *s)
{
  পূর্ণসংখ্যা fd;

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
writetest(অক্ষর *s)
{
  পূর্ণসংখ্যা fd;
  পূর্ণসংখ্যা i;
  গণনা { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  জন্য (i = 0; i < N; i++) {
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
writebig(অক্ষর *s)
{
  পূর্ণসংখ্যা i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  জন্য (i = 0; i < MAXFILE; i++) {
    ((পূর্ণসংখ্যা *)buf)[0] = i;
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
  জন্য (;;) {
    i = পঠ(fd, buf, BSIZE);
    যদি (i == 0) {
      যদি (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      বিরতি;
    } অন্যথা যদি (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    যদি (((পূর্ণসংখ্যা *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((পূর্ণসংখ্যা *)buf)[0]);
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
createtest(অক্ষর *s)
{
  পূর্ণসংখ্যা i, fd;
  গণনা { N = 52 };

  অক্ষর name[3];
  name[0] = 'a';
  name[2] = '\0';
  জন্য (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  জন্য (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

শূন্য
dirtest(অক্ষর *s)
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
exectest(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, xstatus, pid;
  অক্ষর *echoargv[] = {"echo", "OK", 0};
  অক্ষর buf[3];

  unlink("echo-ok");
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণসংখ্যা errfd = dup(1);
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
pipe1(অক্ষর *s)
{
  পূর্ণসংখ্যা fds[2], pid, xstatus;
  পূর্ণসংখ্যা seq, i, n, cc, যোগ;
  গণনা { N = 5, SZ = 1033 };

  যদি (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  যদি (pid == 0) {
    close(fds[0]);
    জন্য (n = 0; n < N; n++) {
      জন্য (i = 0; i < SZ; i++)
        buf[i] = seq++;
      যদি (লিখ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } অন্যথা যদি (pid > 0) {
    close(fds[1]);
    যোগ = 0;
    cc = 1;
    যখন ((n = পঠ(fds[0], buf, cc)) > 0) {
      জন্য (i = 0; i < n; i++) {
        যদি ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ফেরত;
        }
      }
      যোগ += n;
      cc = cc * 2;
      যদি (cc > আকার(buf))
        cc = আকার(buf);
    }
    যদি (যোগ != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, যোগ);
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
killstatus(অক্ষর *s)
{
  পূর্ণসংখ্যা xst;

  জন্য (পূর্ণসংখ্যা i = 0; i < 100; i++) {
    পূর্ণসংখ্যা pid1 = fork();
    যদি (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid1 == 0) {
      যখন (1) {
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
killzero(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, xst;

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
preempt(অক্ষর *s)
{
  পূর্ণসংখ্যা pid1, pid2, pid3;
  পূর্ণসংখ্যা pfds[2];

  pid1 = fork();
  যদি (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  যদি (pid1 == 0)
    জন্য (;;)
      ;

  pid2 = fork();
  যদি (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  যদি (pid2 == 0)
    জন্য (;;)
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
    জন্য (;;)
      ;
  }

  close(pfds[1]);
  যদি (পঠ(pfds[0], buf, আকার(buf)) != 1) {
    printf("%s: preempt read error", s);
    ফেরত;
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
exitwait(অক্ষর *s)
{
  পূর্ণসংখ্যা i, pid;

  জন্য (i = 0; i < 100; i++) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid) {
      পূর্ণসংখ্যা xstate;
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
reparent(অক্ষর *s)
{
  পূর্ণসংখ্যা master_pid = getpid();
  জন্য (পূর্ণসংখ্যা i = 0; i < 200; i++) {
    পূর্ণসংখ্যা pid = fork();
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
      পূর্ণসংখ্যা pid2 = fork();
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
twochildren(অক্ষর *s)
{
  জন্য (পূর্ণসংখ্যা i = 0; i < 1000; i++) {
    পূর্ণসংখ্যা pid1 = fork();
    যদি (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid1 == 0) {
      exit(0);
    } অন্যথা {
      পূর্ণসংখ্যা pid2 = fork();
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
forkfork(অক্ষর *s)
{
  গণনা { N = 2 };

  জন্য (পূর্ণসংখ্যা i = 0; i < N; i++) {
    পূর্ণসংখ্যা pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    যদি (pid == 0) {
      জন্য (পূর্ণসংখ্যা j = 0; j < 200; j++) {
        পূর্ণসংখ্যা pid1 = fork();
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

  পূর্ণসংখ্যা xstatus;
  জন্য (পূর্ণসংখ্যা i = 0; i < N; i++) {
    wait(&xstatus);
    যদি (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

শূন্য
forkforkfork(অক্ষর *s)
{
  unlink("stopforking");

  পূর্ণসংখ্যা pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  যদি (pid == 0) {
    যখন (1) {
      পূর্ণসংখ্যা fd = open("stopforking", 0);
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
reparent2(অক্ষর *s)
{
  জন্য (পূর্ণসংখ্যা i = 0; i < 800; i++) {
    পূর্ণসংখ্যা pid1 = fork();
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
mem(অক্ষর *s)
{
  শূন্য *m1, *m2;
  পূর্ণসংখ্যা pid;

  যদি ((pid = fork()) == 0) {
    m1 = 0;
    যখন ((m2 = malloc(10001)) != 0) {
      *(অক্ষর **)m2 = m1;
      m1 = m2;
    }
    যখন (m1) {
      m2 = *(অক্ষর **)m1;
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
    পূর্ণসংখ্যা xstatus;
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
sharedfd(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, pid, i, n, nc, np;
  গণনা { N = 1000, SZ = 10 };
  অক্ষর buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', আকার(buf));
  জন্য (i = 0; i < N; i++) {
    যদি (লিখ(fd, buf, আকার(buf)) != আকার(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  যদি (pid == 0) {
    exit(0);
  } অন্যথা {
    পূর্ণসংখ্যা xstatus;
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
  যখন ((n = পঠ(fd, buf, আকার(buf))) > 0) {
    জন্য (i = 0; i < আকার(buf); i++) {
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
fourfiles(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, pid, i, j, n, যোগ, pi;
  অক্ষর *names[] = {"f0", "f1", "f2", "f3"};
  অক্ষর *fname;
  গণনা { N = 12, NCHILD = 4, SZ = 500 };

  জন্য (pi = 0; pi < NCHILD; pi++) {
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
      জন্য (i = 0; i < N; i++) {
        যদি ((n = লিখ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  পূর্ণসংখ্যা xstatus;
  জন্য (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(xstatus);
  }

  জন্য (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    যোগ = 0;
    যখন ((n = পঠ(fd, buf, আকার(buf))) > 0) {
      জন্য (j = 0; j < n; j++) {
        যদি (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      যোগ += n;
    }
    close(fd);
    যদি (যোগ != N * SZ) {
      printf("wrong length %d\n", যোগ);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
শূন্য
createdelete(অক্ষর *s)
{
  গণনা { N = 20, NCHILD = 4 };
  পূর্ণসংখ্যা pid, i, fd, pi;
  অক্ষর name[32];

  জন্য (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    যদি (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      জন্য (i = 0; i < N; i++) {
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

  পূর্ণসংখ্যা xstatus;
  জন্য (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  জন্য (i = 0; i < N; i++) {
    জন্য (pi = 0; pi < NCHILD; pi++) {
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

  জন্য (i = 0; i < N; i++) {
    জন্য (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
শূন্য
unlinkread(অক্ষর *s)
{
  গণনা { SZ = 5 };
  পূর্ণসংখ্যা fd, fd1;

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

  যদি (পঠ(fd, buf, আকার(buf)) != SZ) {
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
linktest(অক্ষর *s)
{
  গণনা { SZ = 5 };
  পূর্ণসংখ্যা fd;

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
  যদি (পঠ(fd, buf, আকার(buf)) != SZ) {
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
concreate(অক্ষর *s)
{
  গণনা { N = 40 };
  অক্ষর file[3];
  পূর্ণসংখ্যা i, pid, n, fd;
  অক্ষর fa[N];
  কাঠামো {
    ushort inum;
    অক্ষর name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  জন্য (i = 0; i < N; i++) {
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
      পূর্ণসংখ্যা xstatus;
      wait(&xstatus);
      যদি (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, আকার(fa));
  fd = open(".", 0);
  n = 0;
  যখন (পঠ(fd, &de, আকার(de)) > 0) {
    যদি (de.inum == 0)
      চালু;
    যদি (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      যদি (i < 0 || i >= আকার(fa)) {
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

  জন্য (i = 0; i < N; i++) {
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
linkunlink(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, i;

  unlink("x");
  pid = fork();
  যদি (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned পূর্ণসংখ্যা x = (pid ? 1 : 97);
  জন্য (i = 0; i < 100; i++) {
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
subdir(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, cc;

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
  cc = পঠ(fd, buf, আকার(buf));
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
  যদি (পঠ(fd, buf, আকার(buf)) != 2) {
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
bigwrite(অক্ষর *s)
{
  পূর্ণসংখ্যা fd, sz;

  unlink("bigwrite");
  জন্য (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    যদি (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    পূর্ণসংখ্যা i;
    জন্য (i = 0; i < 2; i++) {
      পূর্ণসংখ্যা cc = লিখ(fd, buf, sz);
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
bigfile(অক্ষর *s)
{
  গণনা { N = 20, SZ = 600 };
  পূর্ণসংখ্যা fd, i, যোগ, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  জন্য (i = 0; i < N; i++) {
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
  যোগ = 0;
  জন্য (i = 0;; i++) {
    cc = পঠ(fd, buf, SZ / 2);
    যদি (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    যদি (cc == 0)
      বিরতি;
    যদি (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    যদি (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    যোগ += cc;
  }
  close(fd);
  যদি (যোগ != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

শূন্য
fourteen(অক্ষর *s)
{
  পূর্ণসংখ্যা fd;

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
rmdot(অক্ষর *s)
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
dirfile(অক্ষর *s)
{
  পূর্ণসংখ্যা fd;

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
iref(অক্ষর *s)
{
  পূর্ণসংখ্যা i, fd;

  জন্য (i = 0; i < NINODE + 1; i++) {
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
  জন্য (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
শূন্য
forktest(অক্ষর *s)
{
  গণনা { N = 1000 };
  পূর্ণসংখ্যা n, pid;

  জন্য (n = 0; n < N; n++) {
    pid = fork();
    যদি (pid < 0)
      বিরতি;
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

  জন্য (; n > 0; n--) {
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
sbrkbasic(অক্ষর *s)
{
  গণনা { TOOMUCH = 1024 * 1024 * 1024 };
  পূর্ণসংখ্যা i, pid, xstatus;
  অক্ষর *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  যদি (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  যদি (pid == 0) {
    a = sbrk(TOOMUCH);
    যদি (a == (অক্ষর *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    জন্য (b = a; b < a + TOOMUCH; b += PGSIZE) {
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
  জন্য (i = 0; i < 5000; i++) {
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
sbrkmuch(অক্ষর *s)
{
  গণনা { BIG = 100 * 1024 * 1024 };
  অক্ষর *c, *oldbrk, *a, *lastaddr, *p;
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

  lastaddr = (অক্ষর *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  যদি (c == (অক্ষর *)SBRK_ERROR) {
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
kernmem(অক্ষর *s)
{
  অক্ষর *a;
  পূর্ণসংখ্যা pid;

  জন্য (a = (অক্ষর *)(KERNBASE); a < (অক্ষর *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    পূর্ণসংখ্যা xstatus;
    wait(&xstatus);
    যদি (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
শূন্য
MAXVAplus(অক্ষর *s)
{
  অস্থির uint64 a = MAXVA;
  জন্য (; a != 0; a <<= 1) {
    পূর্ণসংখ্যা pid;
    pid = fork();
    যদি (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    যদি (pid == 0) {
      *(অক্ষর *)a = 99;
      printf("%s: oops wrote %p\n", s, (শূন্য *)a);
      exit(1);
    }
    পূর্ণসংখ্যা xstatus;
    wait(&xstatus);
    যদি (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
শূন্য
sbrkfail(অক্ষর *s)
{
  গণনা { BIG = 100 * 1024 * 1024 };
  পূর্ণসংখ্যা i, xstatus;
  পূর্ণসংখ্যা fds[2];
  অক্ষর scratch;
  অক্ষর *c, *a;
  পূর্ণসংখ্যা pids[10];
  পূর্ণসংখ্যা pid;
  পূর্ণসংখ্যা failed;

  failed = 0;
  যদি (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  জন্য (i = 0; i < আকার(pids) / আকার(pids[0]); i++) {
    যদি ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      যদি (sbrk(BIG - (uint64)sbrk(0)) == (অক্ষর *)SBRK_ERROR)
        লিখ(fds[1], "0", 1);
      অন্যথা
        লিখ(fds[1], "1", 1);
      // sit around until killed
      জন্য (;;)
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
  জন্য (i = 0; i < আকার(pids) / আকার(pids[0]); i++) {
    যদি (pids[i] == -1)
      চালু;
    kill(pids[i]);
    wait(0);
  }
  যদি (c == (অক্ষর *)SBRK_ERROR) {
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
    যদি (a == (অক্ষর *)SBRK_ERROR) {
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
sbrkarg(অক্ষর *s)
{
  অক্ষর *a;
  পূর্ণসংখ্যা fd, n;

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
  যদি (pipe((পূর্ণসংখ্যা *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

শূন্য
validatetest(অক্ষর *s)
{
  পূর্ণসংখ্যা hi;
  uint64 p;

  hi = 1100 * 1024;
  জন্য (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    যদি (link("nosuchfile", (অক্ষর *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
অক্ষর uninit[10000];
শূন্য
bsstest(অক্ষর *s)
{
  পূর্ণসংখ্যা i;

  জন্য (i = 0; i < আকার(uninit); i++) {
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
bigargtest(অক্ষর *s)
{
  পূর্ণসংখ্যা pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  যদি (pid == 0) {
    স্থির অক্ষর *args[MAXARG];
    পূর্ণসংখ্যা i;
    অক্ষর big[400];
    memset(big, ' ', আকার(big));
    big[আকার(big) - 1] = '\0';
    জন্য (i = 0; i < MAXARG - 1; i++)
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
  পূর্ণসংখ্যা nfiles;
  পূর্ণসংখ্যা fsblocks = 0;

  printf("fsfull test\n");

  জন্য (nfiles = 0;; nfiles++) {
    অক্ষর name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    পূর্ণসংখ্যা fd = open(name, O_CREATE | O_RDWR);
    যদি (fd < 0) {
      printf("open %s failed\n", name);
      বিরতি;
    }
    পূর্ণসংখ্যা যোগ = 0;
    যখন (1) {
      পূর্ণসংখ্যা cc = লিখ(fd, buf, BSIZE);
      যদি (cc < BSIZE)
        বিরতি;
      যোগ += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", যোগ);
    close(fd);
    যদি (যোগ == 0)
      বিরতি;
  }

  যখন (nfiles >= 0) {
    অক্ষর name[64];
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
argptest(অক্ষর *s)
{
  পূর্ণসংখ্যা fd;
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
stacktest(অক্ষর *s)
{
  পূর্ণসংখ্যা pid;
  পূর্ণসংখ্যা xstatus;

  pid = fork();
  যদি (pid == 0) {
    অক্ষর *sp = (অক্ষর *)r_sp();
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
nowrite(অক্ষর *s)
{
  পূর্ণসংখ্যা pid;
  পূর্ণসংখ্যা xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  জন্য (পূর্ণসংখ্যা ai = 0; ai < আকার(addrs) / আকার(addrs[0]); ai++) {
    pid = fork();
    যদি (pid == 0) {
      অস্থির পূর্ণসংখ্যা *addr = (পূর্ণসংখ্যা *)addrs[ai];
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
pgbug(অক্ষর *s)
{
  অক্ষর *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
শূন্য
sbrkbugs(অক্ষর *s)
{
  পূর্ণসংখ্যা pid = fork();
  যদি (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  যদি (pid == 0) {
    পূর্ণসংখ্যা sz = (uint64)sbrk(0);
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
    পূর্ণসংখ্যা sz = (uint64)sbrk(0);
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
sbrklast(অক্ষর *s)
{
  uint64 শীর্ষ_2 = (uint64)sbrk(0);
  যদি ((শীর্ষ_2 % PGSIZE) != 0)
    sbrk(PGSIZE - (শীর্ষ_2 % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  শীর্ষ_2 = (uint64)sbrk(0);
  অক্ষর *p = (অক্ষর *)(শীর্ষ_2 - 64);
  p[0] = 'x';
  p[1] = '\0';
  পূর্ণসংখ্যা fd = open(p, O_RDWR | O_CREATE);
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
sbrk8000(অক্ষর *s)
{
  sbrk(0x80000004);
  অস্থির অক্ষর *শীর্ষ_2 = sbrk(0);
  *(শীর্ষ_2 - 1) = *(শীর্ষ_2 - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
শূন্য
badarg(অক্ষর *s)
{
  জন্য (পূর্ণসংখ্যা i = 0; i < 50000; i++) {
    অক্ষর *argv[2];
    argv[0] = (অক্ষর *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
শূন্য
lazy_alloc(অক্ষর *s)
{
  অক্ষর *i, *পূর্ব_end, *new_end;

  পূর্ব_end = sbrklazy(REGION_SZ);
  যদি (পূর্ব_end == (অক্ষর *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = পূর্ব_end + REGION_SZ;

  জন্য (i = পূর্ব_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(অক্ষর **)i = i;

  জন্য (i = পূর্ব_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    যদি (*(অক্ষর **)i != i) {
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
lazy_unmap(অক্ষর *s)
{
  পূর্ণসংখ্যা pid;
  অক্ষর *i, *পূর্ব_end, *new_end;

  পূর্ব_end = sbrklazy(REGION_SZ);
  যদি (পূর্ব_end == (অক্ষর *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = পূর্ব_end + REGION_SZ;

  জন্য (i = পূর্ব_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(অক্ষর **)i = i;

  জন্য (i = পূর্ব_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    যদি (pid < 0) {
      printf("error forking\n");
      exit(1);
    } অন্যথা যদি (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(অক্ষর **)i = i;
      exit(0);
    } অন্যথা {
      পূর্ণসংখ্যা status;
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
lazy_copy(অক্ষর *s)
{
  // copyinstr on lazy page
  {
    অক্ষর *p = sbrk(0);
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
  জন্য (পূর্ণসংখ্যা i = 0; i < আকার(bad) / আকার(bad[0]); i++) {
    পূর্ণসংখ্যা fd = open("README", 0);
    যদি (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    যদি (পঠ(fd, (অক্ষর *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    যদি (লিখ(fd, (অক্ষর *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

শূন্য
lazy_copyinstr(অক্ষর *s)
{
  অক্ষর *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  যদি ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  পূর্ণসংখ্যা fd = open(&p[4095], O_RDONLY);
  যদি (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  কাঠামো stat st;
  পূর্ণসংখ্যা r = fstat(fd, &st);
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
lazy_sbrk(অক্ষর *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  অক্ষর *p = sbrk(0);
  যখন ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    যদি (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  পূর্ণসংখ্যা n = TRAPFRAME - PGSIZE - (uint64)p;

  অক্ষর *p1 = sbrklazy(n);
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
partial_write(অক্ষর *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  পূর্ণসংখ্যা fd = open("testfile", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  পূর্ণসংখ্যা cc = লিখ(fd, "A", 1);
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

  অক্ষর *p = sbrk(0);
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

  অক্ষর b;
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
  জন্য (পূর্ণসংখ্যা i = 0; i < 64; i++) {
    অক্ষর buf[1024];
    memset(buf, 0, আকার(buf));
    cc = লিখ(fd, buf, আকার(buf));
    যদি (cc != আকার(buf)) {
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
unlinkcwd(অক্ষর *s)
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

কাঠামো test {
  শূন্য (*f)(অক্ষর *);
  অক্ষর *s;
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
bigdir(অক্ষর *s)
{
  গণনা { N = 500 };
  পূর্ণসংখ্যা i, fd;
  অক্ষর name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  যদি (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  জন্য (i = 0; i < N; i++) {
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
  জন্য (i = 0; i < N; i++) {
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
manywrites(অক্ষর *s)
{
  পূর্ণসংখ্যা nchildren = 4;
  পূর্ণসংখ্যা howmany = 30; // increase to look for deadlock

  জন্য (পূর্ণসংখ্যা ci = 0; ci < nchildren; ci++) {
    পূর্ণসংখ্যা pid = fork();
    যদি (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    যদি (pid == 0) {
      অক্ষর name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      জন্য (পূর্ণসংখ্যা iters = 0; iters < howmany; iters++) {
        জন্য (পূর্ণসংখ্যা i = 0; i < ci + 1; i++) {
          পূর্ণসংখ্যা fd = open(name, O_CREATE | O_RDWR);
          যদি (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          পূর্ণসংখ্যা sz = আকার(buf);
          পূর্ণসংখ্যা cc = লিখ(fd, buf, sz);
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

  জন্য (পূর্ণসংখ্যা ci = 0; ci < nchildren; ci++) {
    পূর্ণসংখ্যা st = 0;
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
badwrite(অক্ষর *s)
{
  পূর্ণসংখ্যা assumed_free = 600;

  unlink("junk");
  জন্য (পূর্ণসংখ্যা i = 0; i < assumed_free; i++) {
    পূর্ণসংখ্যা fd = open("junk", O_CREATE | O_WRONLY);
    যদি (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    লিখ(fd, (অক্ষর *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  পূর্ণসংখ্যা fd = open("junk", O_CREATE | O_WRONLY);
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
execout(অক্ষর *s)
{
  জন্য (পূর্ণসংখ্যা avail = 0; avail < 15; avail++) {
    পূর্ণসংখ্যা pid = fork();
    যদি (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } অন্যথা যদি (pid == 0) {
      // allocate all of memory.
      যখন (1) {
        অক্ষর *a = sbrk(PGSIZE);
        যদি (a == SBRK_ERROR)
          বিরতি;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      জন্য (পূর্ণসংখ্যা i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      অক্ষর *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } অন্যথা {
      wait((পূর্ণসংখ্যা *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
শূন্য
diskfull(অক্ষর *s)
{
  পূর্ণসংখ্যা fi;
  পূর্ণসংখ্যা done = 0;

  unlink("diskfulldir");

  জন্য (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    অক্ষর name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    পূর্ণসংখ্যা fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      বিরতি;
    }
    জন্য (পূর্ণসংখ্যা i = 0; i < MAXFILE; i++) {
      অক্ষর buf[BSIZE];
      যদি (লিখ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        বিরতি;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  পূর্ণসংখ্যা nzz = 128;
  জন্য (পূর্ণসংখ্যা i = 0; i < nzz; i++) {
    অক্ষর name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    পূর্ণসংখ্যা fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0)
      বিরতি;
    close(fd);
  }

  // this mkdir() is expected to fail.
  যদি (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  জন্য (পূর্ণসংখ্যা i = 0; i < nzz; i++) {
    অক্ষর name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  জন্য (পূর্ণসংখ্যা i = 0; '0' + i < 0177; i++) {
    অক্ষর name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

শূন্য
outofinodes(অক্ষর *s)
{
  পূর্ণসংখ্যা nzz = 32 * 32;
  জন্য (পূর্ণসংখ্যা i = 0; i < nzz; i++) {
    অক্ষর name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    পূর্ণসংখ্যা fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    যদি (fd < 0) {
      // failure is eventually expected.
      বিরতি;
    }
    close(fd);
  }

  জন্য (পূর্ণসংখ্যা i = 0; i < nzz; i++) {
    অক্ষর name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

শূন্য
linkoverflow(অক্ষর *s)
{
  গণনা { TARGET = 32768 };
  গণনা { DIRS = 64 };
  কাঠামো stat st;
  পূর্ণসংখ্যা i;

  unlink("/lof");
  পূর্ণসংখ্যা fd = open("/lof", O_CREATE | O_RDWR);
  যদি (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  জন্য (i = 0; i < TARGET; i++) {
    পূর্ণসংখ্যা d = i % DIRS;
    পূর্ণসংখ্যা f = i / DIRS;

    অক্ষর pn[16];
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
        বিরতি;
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

কাঠামো test slowtests[] = {
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
পূর্ণসংখ্যা
run(শূন্য f(অক্ষর *), অক্ষর *s)
{
  পূর্ণসংখ্যা pid;
  পূর্ণসংখ্যা xstatus;

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
    ফেরত xstatus == 0;
  }
}

পূর্ণসংখ্যা
runtests(কাঠামো test *tests, অক্ষর *justone, পূর্ণসংখ্যা continuous)
{
  পূর্ণসংখ্যা ntests = 0;
  জন্য (কাঠামো test *t = tests; t->s != 0; t++) {
    যদি ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      যদি (!run(t->f, t->s)) {
        যদি (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ফেরত -1;
        }
      }
    }
  }
  ফেরত ntests;
}

// use sbrk() to count how many free physical memory pages there are.
পূর্ণসংখ্যা
countfree()
{
  পূর্ণসংখ্যা n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  যখন (1) {
    অক্ষর *a = sbrk(PGSIZE);
    যদি (a == SBRK_ERROR) {
      বিরতি;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ফেরত n;
}

পূর্ণসংখ্যা
drivetests(পূর্ণসংখ্যা quick, পূর্ণসংখ্যা continuous, অক্ষর *justone)
{
  করো {
    printf("usertests starting\n");
    পূর্ণসংখ্যা free0 = countfree();
    পূর্ণসংখ্যা free1 = 0;
    পূর্ণসংখ্যা ntests = 0;
    পূর্ণসংখ্যা n;
    n = runtests(quicktests, justone, continuous);
    যদি (n < 0) {
      যদি (continuous != 2) {
        ফেরত 1;
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
          ফেরত 1;
        }
      } অন্যথা {
        ntests += n;
      }
    }
    যদি ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      যদি (continuous != 2) {
        ফেরত 1;
      }
    }
    যদি (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ফেরত 1;
    }
  } যখন (continuous);
  ফেরত 0;
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা continuous = 0;
  পূর্ণসংখ্যা quick = 0;
  অক্ষর *justone = 0;

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
