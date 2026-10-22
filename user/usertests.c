#அந்தர்பாவ "kernel/param.h"
#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "kernel/syscall.h"
#அந்தர்பாவ "kernel/memlayout.h"
#அந்தர்பாவ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

எழுத்து buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
வெறுமை
copyin(எழுத்து *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  க்கு (முழு ai = 0; ai < அளவு(addrs) / அளவு(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    முழு fd = open("copyin1", O_CREATE | O_WRONLY);
    எனில் (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    முழு n = லிக(fd, (வெறுமை *)addr, 8192);
    எனில் (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (வெறுமை *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = லிக(1, (எழுத்து *)addr, 8192);
    எனில் (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (வெறுமை *)addr, n);
      exit(1);
    }

    முழு fds[2];
    எனில் (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = லிக(fds[1], (எழுத்து *)addr, 8192);
    எனில் (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (வெறுமை *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
வெறுமை
copyout(எழுத்து *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  க்கு (முழு ai = 0; ai < அளவு(addrs) / அளவு(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    முழு fd = open("README", 0);
    எனில் (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    முழு n = பட(fd, (வெறுமை *)addr, 8192);
    எனில் (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (வெறுமை *)addr, n);
      exit(1);
    }
    close(fd);

    முழு fds[2];
    எனில் (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = லிக(fds[1], "x", 1);
    எனில் (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = பட(fds[0], (வெறுமை *)addr, 8192);
    எனில் (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (வெறுமை *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
வெறுமை
copyinstr1(எழுத்து *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  க்கு (முழு ai = 0; ai < அளவு(addrs) / அளவு(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    முழு fd = open((எழுத்து *)addr, O_CREATE | O_WRONLY);
    எனில் (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (வெறுமை *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
வெறுமை
copyinstr2(எழுத்து *s)
{
  எழுத்து b[MAXPATH + 1];

  க்கு (முழு i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  முழு ret = unlink(b);
  எனில் (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  முழு fd = open(b, O_CREATE | O_WRONLY);
  எனில் (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  எனில் (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  எழுத்து *args[] = {"xx", 0};
  ret = exec(b, args);
  எனில் (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  முழு pid = fork();
  எனில் (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  எனில் (pid == 0) {
    நிலையான எழுத்து big[PGSIZE + 1];
    க்கு (முழு i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    எழுத்து *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    எனில் (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  முழு st = 0;
  wait(&st);
  எனில் (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
வெறுமை
copyinstr3(எழுத்து *s)
{
  sbrk(8192);
  uint64 உச்சி = (uint64)sbrk(0);
  எனில் ((உச்சி % PGSIZE) != 0) {
    sbrk(PGSIZE - (உச்சி % PGSIZE));
  }
  உச்சி = (uint64)sbrk(0);
  எனில் (உச்சி % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  எழுத்து *b = (எழுத்து *)(உச்சி - 1);
  *b = 'x';

  முழு ret = unlink(b);
  எனில் (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  முழு fd = open(b, O_CREATE | O_WRONLY);
  எனில் (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  எனில் (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  எழுத்து *args[] = {"xx", 0};
  ret = exec(b, args);
  எனில் (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
வெறுமை
rwsbrk(எழுத்து *s)
{
  முழு fd, n;

  uint64 a = (uint64)sbrk(8192);

  எனில் (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  எனில் (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  எனில் (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = லிக(fd, (வெறுமை *)(a + PGSIZE), 1024);
  எனில் (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (வெறுமை *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  எனில் (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = பட(fd, (வெறுமை *)(a + PGSIZE), 10);
  எனில் (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (வெறுமை *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
வெறுமை
truncate1(எழுத்து *s)
{
  எழுத்து buf[32];

  unlink("truncfile");
  முழு fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  லிக(fd1, "abcd", 4);
  close(fd1);

  முழு fd2 = open("truncfile", O_RDONLY);
  முழு n = பட(fd2, buf, அளவு(buf));
  எனில் (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  முழு fd3 = open("truncfile", O_RDONLY);
  n = பட(fd3, buf, அளவு(buf));
  எனில் (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = பட(fd2, buf, அளவு(buf));
  எனில் (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  லிக(fd1, "abcdef", 6);

  n = பட(fd3, buf, அளவு(buf));
  எனில் (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = பட(fd2, buf, அளவு(buf));
  எனில் (n != 2) {
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
வெறுமை
truncate2(எழுத்து *s)
{
  unlink("truncfile");

  முழு fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  லிக(fd1, "abcd", 4);

  முழு fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  முழு n = லிக(fd1, "x", 1);
  எனில் (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

வெறுமை
truncate3(எழுத்து *s)
{
  முழு pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  எனில் (pid == 0) {
    க்கு (முழு i = 0; i < 100; i++) {
      எழுத்து buf[32];
      முழு fd = open("truncfile", O_WRONLY);
      எனில் (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      முழு n = லிக(fd, "1234567890", 10);
      எனில் (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      பட(fd, buf, அளவு(buf));
      close(fd);
    }
    exit(0);
  }

  க்கு (முழு i = 0; i < 150; i++) {
    முழு fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    எனில் (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    முழு n = லிக(fd, "xxx", 3);
    எனில் (n != 3) {
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
வெறுமை
iputtest(எழுத்து *s)
{
  எனில் (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  எனில் (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  எனில் (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  எனில் (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
வெறுமை
exitiputtest(எழுத்து *s)
{
  முழு pid, xstatus;

  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid == 0) {
    எனில் (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    எனில் (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    எனில் (unlink("../iputdir") < 0) {
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
வெறுமை
openiputtest(எழுத்து *s)
{
  முழு pid, xstatus;

  எனில் (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid == 0) {
    முழு fd = open("oidir", O_RDWR);
    எனில் (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  எனில் (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

வெறுமை
opentest(எழுத்து *s)
{
  முழு fd;

  fd = open("echo", 0);
  எனில் (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  எனில் (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

வெறுமை
writetest(எழுத்து *s)
{
  முழு fd;
  முழு i;
  எண்ணிக்கை { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  க்கு (i = 0; i < N; i++) {
    எனில் (லிக(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    எனில் (லிக(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = பட(fd, buf, N * SZ * 2);
  எனில் (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  எனில் (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

வெறுமை
writebig(எழுத்து *s)
{
  முழு i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  க்கு (i = 0; i < MAXFILE; i++) {
    ((முழு *)buf)[0] = i;
    எனில் (லிக(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  க்கு (;;) {
    i = பட(fd, buf, BSIZE);
    எனில் (i == 0) {
      எனில் (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      நிறுத்து;
    } இல்லையெனில் எனில் (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    எனில் (((முழு *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((முழு *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  எனில் (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
வெறுமை
createtest(எழுத்து *s)
{
  முழு i, fd;
  எண்ணிக்கை { N = 52 };

  எழுத்து name[3];
  name[0] = 'a';
  name[2] = '\0';
  க்கு (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  க்கு (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

வெறுமை
dirtest(எழுத்து *s)
{
  எனில் (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  எனில் (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  எனில் (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  எனில் (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

வெறுமை
exectest(எழுத்து *s)
{
  முழு fd, xstatus, pid;
  எழுத்து *echoargv[] = {"echo", "OK", 0};
  எழுத்து buf[3];

  unlink("echo-ok");
  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid == 0) {
    முழு errfd = dup(1);
    எனில் (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    எனில் (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    எனில் (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    எனில் (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  எனில் (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  எனில் (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  எனில் (பட(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  எனில் (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  இல்லையெனில் {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

வெறுமை
pipe1(எழுத்து *s)
{
  முழு fds[2], pid, xstatus;
  முழு seq, i, n, cc, total;
  எண்ணிக்கை { N = 5, SZ = 1033 };

  எனில் (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  எனில் (pid == 0) {
    close(fds[0]);
    க்கு (n = 0; n < N; n++) {
      க்கு (i = 0; i < SZ; i++)
        buf[i] = seq++;
      எனில் (லிக(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } இல்லையெனில் எனில் (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    போது ((n = பட(fds[0], buf, cc)) > 0) {
      க்கு (i = 0; i < n; i++) {
        எனில் ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          திரும்பு;
        }
      }
      total += n;
      cc = cc * 2;
      எனில் (cc > அளவு(buf))
        cc = அளவு(buf);
    }
    எனில் (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } இல்லையெனில் {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
வெறுமை
killstatus(எழுத்து *s)
{
  முழு xst;

  க்கு (முழு i = 0; i < 100; i++) {
    முழு pid1 = fork();
    எனில் (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid1 == 0) {
      போது (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    எனில் (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
வெறுமை
killzero(எழுத்து *s)
{
  முழு pid, xst;

  kill(0);

  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid == 0) {
    exit(7);
  }
  எனில் (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  எனில் (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
வெறுமை
preempt(எழுத்து *s)
{
  முழு pid1, pid2, pid3;
  முழு pfds[2];

  pid1 = fork();
  எனில் (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  எனில் (pid1 == 0)
    க்கு (;;)
      ;

  pid2 = fork();
  எனில் (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid2 == 0)
    க்கு (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  எனில் (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid3 == 0) {
    close(pfds[0]);
    எனில் (லிக(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    க்கு (;;)
      ;
  }

  close(pfds[1]);
  எனில் (பட(pfds[0], buf, அளவு(buf)) != 1) {
    printf("%s: preempt read error", s);
    திரும்பு;
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
வெறுமை
exitwait(எழுத்து *s)
{
  முழு i, pid;

  க்கு (i = 0; i < 100; i++) {
    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid) {
      முழு xstate;
      எனில் (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      எனில் (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } இல்லையெனில் {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
வெறுமை
reparent(எழுத்து *s)
{
  முழு master_pid = getpid();
  க்கு (முழு i = 0; i < 200; i++) {
    முழு pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid) {
      எனில் (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } இல்லையெனில் {
      முழு pid2 = fork();
      எனில் (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
வெறுமை
twochildren(எழுத்து *s)
{
  க்கு (முழு i = 0; i < 1000; i++) {
    முழு pid1 = fork();
    எனில் (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid1 == 0) {
      exit(0);
    } இல்லையெனில் {
      முழு pid2 = fork();
      எனில் (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      எனில் (pid2 == 0) {
        exit(0);
      } இல்லையெனில் {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
வெறுமை
forkfork(எழுத்து *s)
{
  எண்ணிக்கை { N = 2 };

  க்கு (முழு i = 0; i < N; i++) {
    முழு pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    எனில் (pid == 0) {
      க்கு (முழு j = 0; j < 200; j++) {
        முழு pid1 = fork();
        எனில் (pid1 < 0) {
          exit(1);
        }
        எனில் (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  முழு xstatus;
  க்கு (முழு i = 0; i < N; i++) {
    wait(&xstatus);
    எனில் (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

வெறுமை
forkforkfork(எழுத்து *s)
{
  unlink("stopforking");

  முழு pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  எனில் (pid == 0) {
    போது (1) {
      முழு fd = open("stopforking", 0);
      எனில் (fd >= 0) {
        exit(0);
      }
      எனில் (fork() < 0) {
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
வெறுமை
reparent2(எழுத்து *s)
{
  க்கு (முழு i = 0; i < 800; i++) {
    முழு pid1 = fork();
    எனில் (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    எனில் (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
வெறுமை
mem(எழுத்து *s)
{
  வெறுமை *m1, *m2;
  முழு pid;

  எனில் ((pid = fork()) == 0) {
    m1 = 0;
    போது ((m2 = malloc(10001)) != 0) {
      *(எழுத்து **)m2 = m1;
      m1 = m2;
    }
    போது (m1) {
      m2 = *(எழுத்து **)m1;
      விடுவி(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    எனில் (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    விடுவி(m1);
    exit(0);
  } இல்லையெனில் {
    முழு xstatus;
    wait(&xstatus);
    எனில் (xstatus == -1) {
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
வெறுமை
sharedfd(எழுத்து *s)
{
  முழு fd, pid, i, n, nc, np;
  எண்ணிக்கை { N = 1000, SZ = 10 };
  எழுத்து buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', அளவு(buf));
  க்கு (i = 0; i < N; i++) {
    எனில் (லிக(fd, buf, அளவு(buf)) != அளவு(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  எனில் (pid == 0) {
    exit(0);
  } இல்லையெனில் {
    முழு xstatus;
    wait(&xstatus);
    எனில் (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  எனில் (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  போது ((n = பட(fd, buf, அளவு(buf))) > 0) {
    க்கு (i = 0; i < அளவு(buf); i++) {
      எனில் (buf[i] == 'c')
        nc++;
      எனில் (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  எனில் (nc == N * SZ && np == N * SZ) {
    exit(0);
  } இல்லையெனில் {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
வெறுமை
fourfiles(எழுத்து *s)
{
  முழு fd, pid, i, j, n, total, pi;
  எழுத்து *names[] = {"f0", "f1", "f2", "f3"};
  எழுத்து *fname;
  எண்ணிக்கை { N = 12, NCHILD = 4, SZ = 500 };

  க்கு (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    எனில் (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      எனில் (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      க்கு (i = 0; i < N; i++) {
        எனில் ((n = லிக(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  முழு xstatus;
  க்கு (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    எனில் (xstatus != 0)
      exit(xstatus);
  }

  க்கு (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    போது ((n = பட(fd, buf, அளவு(buf))) > 0) {
      க்கு (j = 0; j < n; j++) {
        எனில் (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    எனில் (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
வெறுமை
createdelete(எழுத்து *s)
{
  எண்ணிக்கை { N = 20, NCHILD = 4 };
  முழு pid, i, fd, pi;
  எழுத்து name[32];

  க்கு (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    எனில் (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      க்கு (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        எனில் (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        எனில் (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          எனில் (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  முழு xstatus;
  க்கு (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    எனில் (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  க்கு (i = 0; i < N; i++) {
    க்கு (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      எனில் ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } இல்லையெனில் எனில் ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      எனில் (fd >= 0)
        close(fd);
    }
  }

  க்கு (i = 0; i < N; i++) {
    க்கு (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
வெறுமை
unlinkread(எழுத்து *s)
{
  எண்ணிக்கை { SZ = 5 };
  முழு fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  லிக(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  எனில் (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  எனில் (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  லிக(fd1, "yyy", 3);
  close(fd1);

  எனில் (பட(fd, buf, அளவு(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  எனில் (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  எனில் (லிக(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

வெறுமை
linktest(எழுத்து *s)
{
  எண்ணிக்கை { SZ = 5 };
  முழு fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  எனில் (லிக(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  எனில் (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  எனில் (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  எனில் (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  எனில் (பட(fd, buf, அளவு(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  எனில் (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  எனில் (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  எனில் (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
வெறுமை
concreate(எழுத்து *s)
{
  எண்ணிக்கை { N = 40 };
  எழுத்து file[3];
  முழு i, pid, n, fd;
  எழுத்து fa[N];
  கட்டமைப்பு {
    ushort inum;
    எழுத்து name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  க்கு (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    எனில் (pid && (i % 3) == 1) {
      link("C0", file);
    } இல்லையெனில் எனில் (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } இல்லையெனில் {
      fd = open(file, O_CREATE | O_RDWR);
      எனில் (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    எனில் (pid == 0) {
      exit(0);
    } இல்லையெனில் {
      முழு xstatus;
      wait(&xstatus);
      எனில் (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, அளவு(fa));
  fd = open(".", 0);
  n = 0;
  போது (பட(fd, &de, அளவு(de)) > 0) {
    எனில் (de.inum == 0)
      தொடர்;
    எனில் (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      எனில் (i < 0 || i >= அளவு(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      எனில் (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  எனில் (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  க்கு (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } இல்லையெனில் {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    எனில் (pid == 0)
      exit(0);
    இல்லையெனில்
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
வெறுமை
linkunlink(எழுத்து *s)
{
  முழு pid, i;

  unlink("x");
  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned முழு x = (pid ? 1 : 97);
  க்கு (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    எனில் ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } இல்லையெனில் எனில் ((x % 3) == 1) {
      link("cat", "x");
    } இல்லையெனில் {
      unlink("x");
    }
  }

  எனில் (pid)
    wait(0);
  இல்லையெனில்
    exit(0);
}

வெறுமை
subdir(எழுத்து *s)
{
  முழு fd, cc;

  unlink("ff");
  எனில் (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  லிக(fd, "ff", 2);
  close(fd);

  எனில் (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  எனில் (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  லிக(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  எனில் (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = பட(fd, buf, அளவு(buf));
  எனில் (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  எனில் (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  எனில் (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  எனில் (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  எனில் (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  எனில் (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  எனில் (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  எனில் (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  எனில் (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  எனில் (பட(fd, buf, அளவு(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  எனில் (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  எனில் (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  எனில் (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  எனில் (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  எனில் (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  எனில் (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  எனில் (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  எனில் (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  எனில் (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  எனில் (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  எனில் (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  எனில் (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  எனில் (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
வெறுமை
bigwrite(எழுத்து *s)
{
  முழு fd, sz;

  unlink("bigwrite");
  க்கு (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    எனில் (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    முழு i;
    க்கு (i = 0; i < 2; i++) {
      முழு cc = லிக(fd, buf, sz);
      எனில் (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

வெறுமை
bigfile(எழுத்து *s)
{
  எண்ணிக்கை { N = 20, SZ = 600 };
  முழு fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  க்கு (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    எனில் (லிக(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  எனில் (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  க்கு (i = 0;; i++) {
    cc = பட(fd, buf, SZ / 2);
    எனில் (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    எனில் (cc == 0)
      நிறுத்து;
    எனில் (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    எனில் (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  எனில் (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

வெறுமை
fourteen(எழுத்து *s)
{
  முழு fd;

  // DIRSIZ is 14.

  எனில் (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  எனில் (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  எனில் (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  எனில் (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  எனில் (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  எனில் (mkdir("123456789012345/12345678901234") == 0) {
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

வெறுமை
rmdot(எழுத்து *s)
{
  எனில் (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  எனில் (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  எனில் (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  எனில் (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  எனில் (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  எனில் (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  எனில் (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  எனில் (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

வெறுமை
dirfile(எழுத்து *s)
{
  முழு fd;

  fd = open("dirfile", O_CREATE);
  எனில் (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  எனில் (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  எனில் (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  எனில் (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  எனில் (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  எனில் (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  எனில் (லிக(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
வெறுமை
iref(எழுத்து *s)
{
  முழு i, fd;

  க்கு (i = 0; i < NINODE + 1; i++) {
    எனில் (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    எனில் (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    எனில் (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    எனில் (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  க்கு (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
வெறுமை
forktest(எழுத்து *s)
{
  எண்ணிக்கை { N = 1000 };
  முழு n, pid;

  க்கு (n = 0; n < N; n++) {
    pid = fork();
    எனில் (pid < 0)
      நிறுத்து;
    எனில் (pid == 0)
      exit(0);
  }

  எனில் (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  எனில் (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  க்கு (; n > 0; n--) {
    எனில் (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  எனில் (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

வெறுமை
sbrkbasic(எழுத்து *s)
{
  எண்ணிக்கை { TOOMUCH = 1024 * 1024 * 1024 };
  முழு i, pid, xstatus;
  எழுத்து *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  எனில் (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  எனில் (pid == 0) {
    a = sbrk(TOOMUCH);
    எனில் (a == (எழுத்து *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    க்கு (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  எனில் (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  க்கு (i = 0; i < 5000; i++) {
    b = sbrk(1);
    எனில் (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  எனில் (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  எனில் (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  எனில் (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

வெறுமை
sbrkmuch(எழுத்து *s)
{
  எண்ணிக்கை { BIG = 100 * 1024 * 1024 };
  எழுத்து *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  எனில் (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (எழுத்து *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  எனில் (c == (எழுத்து *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  எனில் (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  எனில் (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  எனில் (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  எனில் (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
வெறுமை
kernmem(எழுத்து *s)
{
  எழுத்து *a;
  முழு pid;

  க்கு (a = (எழுத்து *)(KERNBASE); a < (எழுத்து *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    முழு xstatus;
    wait(&xstatus);
    எனில் (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
வெறுமை
MAXVAplus(எழுத்து *s)
{
  நிலையற்ற uint64 a = MAXVA;
  க்கு (; a != 0; a <<= 1) {
    முழு pid;
    pid = fork();
    எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    எனில் (pid == 0) {
      *(எழுத்து *)a = 99;
      printf("%s: oops wrote %p\n", s, (வெறுமை *)a);
      exit(1);
    }
    முழு xstatus;
    wait(&xstatus);
    எனில் (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
வெறுமை
sbrkfail(எழுத்து *s)
{
  எண்ணிக்கை { BIG = 100 * 1024 * 1024 };
  முழு i, xstatus;
  முழு fds[2];
  எழுத்து scratch;
  எழுத்து *c, *a;
  முழு pids[10];
  முழு pid;
  முழு failed;

  failed = 0;
  எனில் (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  க்கு (i = 0; i < அளவு(pids) / அளவு(pids[0]); i++) {
    எனில் ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      எனில் (sbrk(BIG - (uint64)sbrk(0)) == (எழுத்து *)SBRK_ERROR)
        லிக(fds[1], "0", 1);
      இல்லையெனில்
        லிக(fds[1], "1", 1);
      // sit around until killed
      க்கு (;;)
        pause(1000);
    }
    எனில் (pids[i] != -1) {
      பட(fds[0], &scratch, 1);
      எனில் (scratch == '0')
        failed = 1;
    }
  }
  எனில் (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  க்கு (i = 0; i < அளவு(pids) / அளவு(pids[0]); i++) {
    எனில் (pids[i] == -1)
      தொடர்;
    kill(pids[i]);
    wait(0);
  }
  எனில் (c == (எழுத்து *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  எனில் (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    எனில் (a == (எழுத்து *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  எனில் (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
வெறுமை
sbrkarg(எழுத்து *s)
{
  எழுத்து *a;
  முழு fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  எனில் (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  எனில் ((n = லிக(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  எனில் (pipe((முழு *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

வெறுமை
validatetest(எழுத்து *s)
{
  முழு hi;
  uint64 p;

  hi = 1100 * 1024;
  க்கு (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    எனில் (link("nosuchfile", (எழுத்து *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
எழுத்து uninit[10000];
வெறுமை
bsstest(எழுத்து *s)
{
  முழு i;

  க்கு (i = 0; i < அளவு(uninit); i++) {
    எனில் (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
வெறுமை
bigargtest(எழுத்து *s)
{
  முழு pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  எனில் (pid == 0) {
    நிலையான எழுத்து *args[MAXARG];
    முழு i;
    எழுத்து big[400];
    memset(big, ' ', அளவு(big));
    big[அளவு(big) - 1] = '\0';
    க்கு (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } இல்லையெனில் எனில் (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  எனில் (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  எனில் (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
வெறுமை
fsfull()
{
  முழு nfiles;
  முழு fsblocks = 0;

  printf("fsfull test\n");

  க்கு (nfiles = 0;; nfiles++) {
    எழுத்து name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    முழு fd = open(name, O_CREATE | O_RDWR);
    எனில் (fd < 0) {
      printf("open %s failed\n", name);
      நிறுத்து;
    }
    முழு total = 0;
    போது (1) {
      முழு cc = லிக(fd, buf, BSIZE);
      எனில் (cc < BSIZE)
        நிறுத்து;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    எனில் (total == 0)
      நிறுத்து;
  }

  போது (nfiles >= 0) {
    எழுத்து name[64];
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

வெறுமை
argptest(எழுத்து *s)
{
  முழு fd;
  fd = open("init", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  பட(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
வெறுமை
stacktest(எழுத்து *s)
{
  முழு pid;
  முழு xstatus;

  pid = fork();
  எனில் (pid == 0) {
    எழுத்து *sp = (எழுத்து *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } இல்லையெனில் எனில் (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  எனில் (xstatus == -1) // kernel killed child?
    exit(0);
  இல்லையெனில்
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
வெறுமை
nowrite(எழுத்து *s)
{
  முழு pid;
  முழு xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  க்கு (முழு ai = 0; ai < அளவு(addrs) / அளவு(addrs[0]); ai++) {
    pid = fork();
    எனில் (pid == 0) {
      நிலையற்ற முழு *addr = (முழு *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } இல்லையெனில் எனில் (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    எனில் (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
வெறுமை *big = (வெறுமை *)0xeaeb0b5b00002f5e;
வெறுமை
pgbug(எழுத்து *s)
{
  எழுத்து *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
வெறுமை
sbrkbugs(எழுத்து *s)
{
  முழு pid = fork();
  எனில் (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  எனில் (pid == 0) {
    முழு sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  எனில் (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  எனில் (pid == 0) {
    முழு sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  எனில் (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  எனில் (pid == 0) {
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
வெறுமை
sbrklast(எழுத்து *s)
{
  uint64 உச்சி = (uint64)sbrk(0);
  எனில் ((உச்சி % PGSIZE) != 0)
    sbrk(PGSIZE - (உச்சி % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  உச்சி = (uint64)sbrk(0);
  எழுத்து *p = (எழுத்து *)(உச்சி - 64);
  p[0] = 'x';
  p[1] = '\0';
  முழு fd = open(p, O_RDWR | O_CREATE);
  லிக(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  பட(fd, p, 1);
  எனில் (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
வெறுமை
sbrk8000(எழுத்து *s)
{
  sbrk(0x80000004);
  நிலையற்ற எழுத்து *உச்சி = sbrk(0);
  *(உச்சி - 1) = *(உச்சி - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
வெறுமை
badarg(எழுத்து *s)
{
  க்கு (முழு i = 0; i < 50000; i++) {
    எழுத்து *argv[2];
    argv[0] = (எழுத்து *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
வெறுமை
lazy_alloc(எழுத்து *s)
{
  எழுத்து *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  எனில் (prev_end == (எழுத்து *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  க்கு (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(எழுத்து **)i = i;

  க்கு (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    எனில் (*(எழுத்து **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
வெறுமை
lazy_unmap(எழுத்து *s)
{
  முழு pid;
  எழுத்து *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  எனில் (prev_end == (எழுத்து *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  க்கு (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(எழுத்து **)i = i;

  க்கு (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    எனில் (pid < 0) {
      printf("error forking\n");
      exit(1);
    } இல்லையெனில் எனில் (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(எழுத்து **)i = i;
      exit(0);
    } இல்லையெனில் {
      முழு status;
      wait(&status);
      எனில் (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

வெறுமை
lazy_copy(எழுத்து *s)
{
  // copyinstr on lazy page
  {
    எழுத்து *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    வெறுமை *xx = sbrk(0);
    வெறுமை *ret = sbrk(-(((uint64)xx) + 1));
    எனில் (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  க்கு (முழு i = 0; i < அளவு(bad) / அளவு(bad[0]); i++) {
    முழு fd = open("README", 0);
    எனில் (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    எனில் (பட(fd, (எழுத்து *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    எனில் (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    எனில் (லிக(fd, (எழுத்து *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

வெறுமை
lazy_copyinstr(எழுத்து *s)
{
  எழுத்து *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  எனில் ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  முழு fd = open(&p[4095], O_RDONLY);
  எனில் (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  கட்டமைப்பு stat st;
  முழு r = fstat(fd, &st);
  எனில் (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  எனில் (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

வெறுமை
lazy_sbrk(எழுத்து *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  எழுத்து *p = sbrk(0);
  போது ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    எனில் (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  முழு n = TRAPFRAME - PGSIZE - (uint64)p;

  எழுத்து *p1 = sbrklazy(n);
  எனில் (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  எனில் (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  எனில் (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  எனில் ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  எனில் ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

வெறுமை
partial_write(எழுத்து *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  முழு fd = open("testfile", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  முழு cc = லிக(fd, "A", 1);
  எனில் (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  எனில் (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  எழுத்து *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  எனில் ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = லிக(fd, p - 1, 2);
  எனில் (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  எழுத்து b;
  cc = பட(fd, &b, 1);
  எனில் (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  எனில் (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  க்கு (முழு i = 0; i < 64; i++) {
    எழுத்து buf[1024];
    memset(buf, 0, அளவு(buf));
    cc = லிக(fd, buf, அளவு(buf));
    எனில் (cc != அளவு(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  எனில் (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = பட(fd, &b, 1);
  எனில் (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  எனில் (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

வெறுமை
unlinkcwd(எழுத்து *s)
{
  எனில் (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  எனில் (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  எனில் (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  எனில் (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  எனில் (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  எனில் (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  எனில் (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

கட்டமைப்பு test {
  வெறுமை (*f)(எழுத்து *);
  எழுத்து *s;
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
வெறுமை
bigdir(எழுத்து *s)
{
  எண்ணிக்கை { N = 500 };
  முழு i, fd;
  எழுத்து name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  எனில் (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  க்கு (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    எனில் (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  க்கு (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    எனில் (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
வெறுமை
manywrites(எழுத்து *s)
{
  முழு nchildren = 4;
  முழு howmany = 30; // increase to look for deadlock

  க்கு (முழு ci = 0; ci < nchildren; ci++) {
    முழு pid = fork();
    எனில் (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    எனில் (pid == 0) {
      எழுத்து name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      க்கு (முழு iters = 0; iters < howmany; iters++) {
        க்கு (முழு i = 0; i < ci + 1; i++) {
          முழு fd = open(name, O_CREATE | O_RDWR);
          எனில் (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          முழு sz = அளவு(buf);
          முழு cc = லிக(fd, buf, sz);
          எனில் (cc != sz) {
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

  க்கு (முழு ci = 0; ci < nchildren; ci++) {
    முழு st = 0;
    wait(&st);
    எனில் (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
வெறுமை
badwrite(எழுத்து *s)
{
  முழு assumed_free = 600;

  unlink("junk");
  க்கு (முழு i = 0; i < assumed_free; i++) {
    முழு fd = open("junk", O_CREATE | O_WRONLY);
    எனில் (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    லிக(fd, (எழுத்து *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  முழு fd = open("junk", O_CREATE | O_WRONLY);
  எனில் (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  எனில் (லிக(fd, "x", 1) != 1) {
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
வெறுமை
execout(எழுத்து *s)
{
  க்கு (முழு avail = 0; avail < 15; avail++) {
    முழு pid = fork();
    எனில் (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } இல்லையெனில் எனில் (pid == 0) {
      // allocate all of memory.
      போது (1) {
        எழுத்து *a = sbrk(PGSIZE);
        எனில் (a == SBRK_ERROR)
          நிறுத்து;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      க்கு (முழு i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      எழுத்து *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } இல்லையெனில் {
      wait((முழு *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
வெறுமை
diskfull(எழுத்து *s)
{
  முழு fi;
  முழு done = 0;

  unlink("diskfulldir");

  க்கு (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    எழுத்து name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    முழு fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    எனில் (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      நிறுத்து;
    }
    க்கு (முழு i = 0; i < MAXFILE; i++) {
      எழுத்து buf[BSIZE];
      எனில் (லிக(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        நிறுத்து;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  முழு nzz = 128;
  க்கு (முழு i = 0; i < nzz; i++) {
    எழுத்து name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    முழு fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    எனில் (fd < 0)
      நிறுத்து;
    close(fd);
  }

  // this mkdir() is expected to fail.
  எனில் (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  க்கு (முழு i = 0; i < nzz; i++) {
    எழுத்து name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  க்கு (முழு i = 0; '0' + i < 0177; i++) {
    எழுத்து name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

வெறுமை
outofinodes(எழுத்து *s)
{
  முழு nzz = 32 * 32;
  க்கு (முழு i = 0; i < nzz; i++) {
    எழுத்து name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    முழு fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    எனில் (fd < 0) {
      // failure is eventually expected.
      நிறுத்து;
    }
    close(fd);
  }

  க்கு (முழு i = 0; i < nzz; i++) {
    எழுத்து name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

வெறுமை
linkoverflow(எழுத்து *s)
{
  எண்ணிக்கை { TARGET = 32768 };
  எண்ணிக்கை { DIRS = 64 };
  கட்டமைப்பு stat st;
  முழு i;

  unlink("/lof");
  முழு fd = open("/lof", O_CREATE | O_RDWR);
  எனில் (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  க்கு (i = 0; i < TARGET; i++) {
    முழு d = i % DIRS;
    முழு f = i / DIRS;

    எழுத்து pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    எனில் (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    எனில் (link("/lof", pn) < 0) {
      எனில் (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      எனில் (st.nlink >= 32767) {
        // overflow check succeeded.
        நிறுத்து;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    எனில் (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  எனில் (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  எனில் (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

கட்டமைப்பு test slowtests[] = {
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
முழு
run(வெறுமை f(எழுத்து *), எழுத்து *s)
{
  முழு pid;
  முழு xstatus;

  printf("test %s: ", s);
  எனில் ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  எனில் (pid == 0) {
    f(s);
    exit(0);
  } இல்லையெனில் {
    wait(&xstatus);
    எனில் (xstatus != 0)
      printf("FAILED\n");
    இல்லையெனில்
      printf("OK\n");
    திரும்பு xstatus == 0;
  }
}

முழு
runtests(கட்டமைப்பு test *tests, எழுத்து *justone, முழு continuous)
{
  முழு ntests = 0;
  க்கு (கட்டமைப்பு test *t = tests; t->s != 0; t++) {
    எனில் ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      எனில் (!run(t->f, t->s)) {
        எனில் (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          திரும்பு -1;
        }
      }
    }
  }
  திரும்பு ntests;
}

// use sbrk() to count how many free physical memory pages there are.
முழு
countfree()
{
  முழு n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  போது (1) {
    எழுத்து *a = sbrk(PGSIZE);
    எனில் (a == SBRK_ERROR) {
      நிறுத்து;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  திரும்பு n;
}

முழு
drivetests(முழு quick, முழு continuous, எழுத்து *justone)
{
  செய் {
    printf("usertests starting\n");
    முழு free0 = countfree();
    முழு free1 = 0;
    முழு ntests = 0;
    முழு n;
    n = runtests(quicktests, justone, continuous);
    எனில் (n < 0) {
      எனில் (continuous != 2) {
        திரும்பு 1;
      }
    } இல்லையெனில் {
      ntests += n;
    }
    எனில் (!quick) {
      எனில் (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      எனில் (n < 0) {
        எனில் (continuous != 2) {
          திரும்பு 1;
        }
      } இல்லையெனில் {
        ntests += n;
      }
    }
    எனில் ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      எனில் (continuous != 2) {
        திரும்பு 1;
      }
    }
    எனில் (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      திரும்பு 1;
    }
  } போது (continuous);
  திரும்பு 0;
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு continuous = 0;
  முழு quick = 0;
  எழுத்து *justone = 0;

  எனில் (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } இல்லையெனில் எனில் (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } இல்லையெனில் எனில் (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } இல்லையெனில் எனில் (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } இல்லையெனில் எனில் (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  எனில் (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
