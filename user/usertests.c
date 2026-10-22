#અન્તર્ભાવ "kernel/param.h"
#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "kernel/syscall.h"
#અન્તર્ભાવ "kernel/memlayout.h"
#અન્તર્ભાવ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

અક્ષર buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
શૂન્ય
copyin(અક્ષર *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  માટે (પૂર્ણાંક ai = 0; ai < કદ(addrs) / કદ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    પૂર્ણાંક fd = open("copyin1", O_CREATE | O_WRONLY);
    જો (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    પૂર્ણાંક n = લિખ(fd, (શૂન્ય *)addr, 8192);
    જો (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (શૂન્ય *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = લિખ(1, (અક્ષર *)addr, 8192);
    જો (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (શૂન્ય *)addr, n);
      exit(1);
    }

    પૂર્ણાંક fds[2];
    જો (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = લિખ(fds[1], (અક્ષર *)addr, 8192);
    જો (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (શૂન્ય *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
શૂન્ય
copyout(અક્ષર *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  માટે (પૂર્ણાંક ai = 0; ai < કદ(addrs) / કદ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    પૂર્ણાંક fd = open("README", 0);
    જો (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    પૂર્ણાંક n = પઠ(fd, (શૂન્ય *)addr, 8192);
    જો (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (શૂન્ય *)addr, n);
      exit(1);
    }
    close(fd);

    પૂર્ણાંક fds[2];
    જો (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = લિખ(fds[1], "x", 1);
    જો (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = પઠ(fds[0], (શૂન્ય *)addr, 8192);
    જો (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (શૂન્ય *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
શૂન્ય
copyinstr1(અક્ષર *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  માટે (પૂર્ણાંક ai = 0; ai < કદ(addrs) / કદ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    પૂર્ણાંક fd = open((અક્ષર *)addr, O_CREATE | O_WRONLY);
    જો (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (શૂન્ય *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
શૂન્ય
copyinstr2(અક્ષર *s)
{
  અક્ષર b[MAXPATH + 1];

  માટે (પૂર્ણાંક i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  પૂર્ણાંક ret = unlink(b);
  જો (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  પૂર્ણાંક fd = open(b, O_CREATE | O_WRONLY);
  જો (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  જો (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  અક્ષર *args[] = {"xx", 0};
  ret = exec(b, args);
  જો (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  પૂર્ણાંક pid = fork();
  જો (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  જો (pid == 0) {
    સ્થિર અક્ષર big[PGSIZE + 1];
    માટે (પૂર્ણાંક i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    અક્ષર *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    જો (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  પૂર્ણાંક st = 0;
  wait(&st);
  જો (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
શૂન્ય
copyinstr3(અક્ષર *s)
{
  sbrk(8192);
  uint64 ટોચ = (uint64)sbrk(0);
  જો ((ટોચ % PGSIZE) != 0) {
    sbrk(PGSIZE - (ટોચ % PGSIZE));
  }
  ટોચ = (uint64)sbrk(0);
  જો (ટોચ % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  અક્ષર *b = (અક્ષર *)(ટોચ - 1);
  *b = 'x';

  પૂર્ણાંક ret = unlink(b);
  જો (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  પૂર્ણાંક fd = open(b, O_CREATE | O_WRONLY);
  જો (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  જો (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  અક્ષર *args[] = {"xx", 0};
  ret = exec(b, args);
  જો (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
શૂન્ય
rwsbrk(અક્ષર *s)
{
  પૂર્ણાંક fd, n;

  uint64 a = (uint64)sbrk(8192);

  જો (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  જો (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  જો (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = લિખ(fd, (શૂન્ય *)(a + PGSIZE), 1024);
  જો (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (શૂન્ય *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  જો (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = પઠ(fd, (શૂન્ય *)(a + PGSIZE), 10);
  જો (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (શૂન્ય *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
શૂન્ય
truncate1(અક્ષર *s)
{
  અક્ષર buf[32];

  unlink("truncfile");
  પૂર્ણાંક fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  લિખ(fd1, "abcd", 4);
  close(fd1);

  પૂર્ણાંક fd2 = open("truncfile", O_RDONLY);
  પૂર્ણાંક n = પઠ(fd2, buf, કદ(buf));
  જો (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  પૂર્ણાંક fd3 = open("truncfile", O_RDONLY);
  n = પઠ(fd3, buf, કદ(buf));
  જો (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = પઠ(fd2, buf, કદ(buf));
  જો (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  લિખ(fd1, "abcdef", 6);

  n = પઠ(fd3, buf, કદ(buf));
  જો (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = પઠ(fd2, buf, કદ(buf));
  જો (n != 2) {
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
શૂન્ય
truncate2(અક્ષર *s)
{
  unlink("truncfile");

  પૂર્ણાંક fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  લિખ(fd1, "abcd", 4);

  પૂર્ણાંક fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  પૂર્ણાંક n = લિખ(fd1, "x", 1);
  જો (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

શૂન્ય
truncate3(અક્ષર *s)
{
  પૂર્ણાંક pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  જો (pid == 0) {
    માટે (પૂર્ણાંક i = 0; i < 100; i++) {
      અક્ષર buf[32];
      પૂર્ણાંક fd = open("truncfile", O_WRONLY);
      જો (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      પૂર્ણાંક n = લિખ(fd, "1234567890", 10);
      જો (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      પઠ(fd, buf, કદ(buf));
      close(fd);
    }
    exit(0);
  }

  માટે (પૂર્ણાંક i = 0; i < 150; i++) {
    પૂર્ણાંક fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    જો (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    પૂર્ણાંક n = લિખ(fd, "xxx", 3);
    જો (n != 3) {
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
શૂન્ય
iputtest(અક્ષર *s)
{
  જો (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  જો (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  જો (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  જો (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
શૂન્ય
exitiputtest(અક્ષર *s)
{
  પૂર્ણાંક pid, xstatus;

  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid == 0) {
    જો (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    જો (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    જો (unlink("../iputdir") < 0) {
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
શૂન્ય
openiputtest(અક્ષર *s)
{
  પૂર્ણાંક pid, xstatus;

  જો (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid == 0) {
    પૂર્ણાંક fd = open("oidir", O_RDWR);
    જો (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  જો (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

શૂન્ય
opentest(અક્ષર *s)
{
  પૂર્ણાંક fd;

  fd = open("echo", 0);
  જો (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  જો (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

શૂન્ય
writetest(અક્ષર *s)
{
  પૂર્ણાંક fd;
  પૂર્ણાંક i;
  ગણના { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  માટે (i = 0; i < N; i++) {
    જો (લિખ(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    જો (લિખ(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  જો (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = પઠ(fd, buf, N * SZ * 2);
  જો (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  જો (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

શૂન્ય
writebig(અક્ષર *s)
{
  પૂર્ણાંક i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  માટે (i = 0; i < MAXFILE; i++) {
    ((પૂર્ણાંક *)buf)[0] = i;
    જો (લિખ(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  જો (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  માટે (;;) {
    i = પઠ(fd, buf, BSIZE);
    જો (i == 0) {
      જો (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      તોડો;
    } નહીંતર જો (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    જો (((પૂર્ણાંક *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((પૂર્ણાંક *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  જો (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
શૂન્ય
createtest(અક્ષર *s)
{
  પૂર્ણાંક i, fd;
  ગણના { N = 52 };

  અક્ષર name[3];
  name[0] = 'a';
  name[2] = '\0';
  માટે (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  માટે (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

શૂન્ય
dirtest(અક્ષર *s)
{
  જો (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  જો (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  જો (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  જો (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

શૂન્ય
exectest(અક્ષર *s)
{
  પૂર્ણાંક fd, xstatus, pid;
  અક્ષર *echoargv[] = {"echo", "OK", 0};
  અક્ષર buf[3];

  unlink("echo-ok");
  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid == 0) {
    પૂર્ણાંક errfd = dup(1);
    જો (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    જો (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    જો (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    જો (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  જો (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  જો (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  જો (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  જો (પઠ(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  જો (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  નહીંતર {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

શૂન્ય
pipe1(અક્ષર *s)
{
  પૂર્ણાંક fds[2], pid, xstatus;
  પૂર્ણાંક seq, i, n, cc, total;
  ગણના { N = 5, SZ = 1033 };

  જો (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  જો (pid == 0) {
    close(fds[0]);
    માટે (n = 0; n < N; n++) {
      માટે (i = 0; i < SZ; i++)
        buf[i] = seq++;
      જો (લિખ(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } નહીંતર જો (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    જ્યારે ((n = પઠ(fds[0], buf, cc)) > 0) {
      માટે (i = 0; i < n; i++) {
        જો ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          પાછા;
        }
      }
      total += n;
      cc = cc * 2;
      જો (cc > કદ(buf))
        cc = કદ(buf);
    }
    જો (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } નહીંતર {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
શૂન્ય
killstatus(અક્ષર *s)
{
  પૂર્ણાંક xst;

  માટે (પૂર્ણાંક i = 0; i < 100; i++) {
    પૂર્ણાંક pid1 = fork();
    જો (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid1 == 0) {
      જ્યારે (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    જો (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
શૂન્ય
killzero(અક્ષર *s)
{
  પૂર્ણાંક pid, xst;

  kill(0);

  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid == 0) {
    exit(7);
  }
  જો (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  જો (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
શૂન્ય
preempt(અક્ષર *s)
{
  પૂર્ણાંક pid1, pid2, pid3;
  પૂર્ણાંક pfds[2];

  pid1 = fork();
  જો (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  જો (pid1 == 0)
    માટે (;;)
      ;

  pid2 = fork();
  જો (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid2 == 0)
    માટે (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  જો (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid3 == 0) {
    close(pfds[0]);
    જો (લિખ(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    માટે (;;)
      ;
  }

  close(pfds[1]);
  જો (પઠ(pfds[0], buf, કદ(buf)) != 1) {
    printf("%s: preempt read error", s);
    પાછા;
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
શૂન્ય
exitwait(અક્ષર *s)
{
  પૂર્ણાંક i, pid;

  માટે (i = 0; i < 100; i++) {
    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid) {
      પૂર્ણાંક xstate;
      જો (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      જો (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } નહીંતર {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
શૂન્ય
reparent(અક્ષર *s)
{
  પૂર્ણાંક master_pid = getpid();
  માટે (પૂર્ણાંક i = 0; i < 200; i++) {
    પૂર્ણાંક pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid) {
      જો (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } નહીંતર {
      પૂર્ણાંક pid2 = fork();
      જો (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
શૂન્ય
twochildren(અક્ષર *s)
{
  માટે (પૂર્ણાંક i = 0; i < 1000; i++) {
    પૂર્ણાંક pid1 = fork();
    જો (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid1 == 0) {
      exit(0);
    } નહીંતર {
      પૂર્ણાંક pid2 = fork();
      જો (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      જો (pid2 == 0) {
        exit(0);
      } નહીંતર {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
શૂન્ય
forkfork(અક્ષર *s)
{
  ગણના { N = 2 };

  માટે (પૂર્ણાંક i = 0; i < N; i++) {
    પૂર્ણાંક pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    જો (pid == 0) {
      માટે (પૂર્ણાંક j = 0; j < 200; j++) {
        પૂર્ણાંક pid1 = fork();
        જો (pid1 < 0) {
          exit(1);
        }
        જો (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  પૂર્ણાંક xstatus;
  માટે (પૂર્ણાંક i = 0; i < N; i++) {
    wait(&xstatus);
    જો (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

શૂન્ય
forkforkfork(અક્ષર *s)
{
  unlink("stopforking");

  પૂર્ણાંક pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  જો (pid == 0) {
    જ્યારે (1) {
      પૂર્ણાંક fd = open("stopforking", 0);
      જો (fd >= 0) {
        exit(0);
      }
      જો (fork() < 0) {
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
શૂન્ય
reparent2(અક્ષર *s)
{
  માટે (પૂર્ણાંક i = 0; i < 800; i++) {
    પૂર્ણાંક pid1 = fork();
    જો (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    જો (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
શૂન્ય
mem(અક્ષર *s)
{
  શૂન્ય *m1, *m2;
  પૂર્ણાંક pid;

  જો ((pid = fork()) == 0) {
    m1 = 0;
    જ્યારે ((m2 = malloc(10001)) != 0) {
      *(અક્ષર **)m2 = m1;
      m1 = m2;
    }
    જ્યારે (m1) {
      m2 = *(અક્ષર **)m1;
      મુક્ત(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    જો (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    મુક્ત(m1);
    exit(0);
  } નહીંતર {
    પૂર્ણાંક xstatus;
    wait(&xstatus);
    જો (xstatus == -1) {
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
શૂન્ય
sharedfd(અક્ષર *s)
{
  પૂર્ણાંક fd, pid, i, n, nc, np;
  ગણના { N = 1000, SZ = 10 };
  અક્ષર buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', કદ(buf));
  માટે (i = 0; i < N; i++) {
    જો (લિખ(fd, buf, કદ(buf)) != કદ(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  જો (pid == 0) {
    exit(0);
  } નહીંતર {
    પૂર્ણાંક xstatus;
    wait(&xstatus);
    જો (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  જો (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  જ્યારે ((n = પઠ(fd, buf, કદ(buf))) > 0) {
    માટે (i = 0; i < કદ(buf); i++) {
      જો (buf[i] == 'c')
        nc++;
      જો (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  જો (nc == N * SZ && np == N * SZ) {
    exit(0);
  } નહીંતર {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
શૂન્ય
fourfiles(અક્ષર *s)
{
  પૂર્ણાંક fd, pid, i, j, n, total, pi;
  અક્ષર *names[] = {"f0", "f1", "f2", "f3"};
  અક્ષર *fname;
  ગણના { N = 12, NCHILD = 4, SZ = 500 };

  માટે (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    જો (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      જો (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      માટે (i = 0; i < N; i++) {
        જો ((n = લિખ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  પૂર્ણાંક xstatus;
  માટે (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    જો (xstatus != 0)
      exit(xstatus);
  }

  માટે (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    જ્યારે ((n = પઠ(fd, buf, કદ(buf))) > 0) {
      માટે (j = 0; j < n; j++) {
        જો (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    જો (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
શૂન્ય
createdelete(અક્ષર *s)
{
  ગણના { N = 20, NCHILD = 4 };
  પૂર્ણાંક pid, i, fd, pi;
  અક્ષર name[32];

  માટે (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    જો (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      માટે (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        જો (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        જો (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          જો (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  પૂર્ણાંક xstatus;
  માટે (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    જો (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  માટે (i = 0; i < N; i++) {
    માટે (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      જો ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } નહીંતર જો ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      જો (fd >= 0)
        close(fd);
    }
  }

  માટે (i = 0; i < N; i++) {
    માટે (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
શૂન્ય
unlinkread(અક્ષર *s)
{
  ગણના { SZ = 5 };
  પૂર્ણાંક fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  લિખ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  જો (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  જો (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  લિખ(fd1, "yyy", 3);
  close(fd1);

  જો (પઠ(fd, buf, કદ(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  જો (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  જો (લિખ(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

શૂન્ય
linktest(અક્ષર *s)
{
  ગણના { SZ = 5 };
  પૂર્ણાંક fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  જો (લિખ(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  જો (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  જો (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  જો (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  જો (પઠ(fd, buf, કદ(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  જો (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  જો (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  જો (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
શૂન્ય
concreate(અક્ષર *s)
{
  ગણના { N = 40 };
  અક્ષર file[3];
  પૂર્ણાંક i, pid, n, fd;
  અક્ષર fa[N];
  સંરચના {
    ushort inum;
    અક્ષર name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  માટે (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    જો (pid && (i % 3) == 1) {
      link("C0", file);
    } નહીંતર જો (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } નહીંતર {
      fd = open(file, O_CREATE | O_RDWR);
      જો (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    જો (pid == 0) {
      exit(0);
    } નહીંતર {
      પૂર્ણાંક xstatus;
      wait(&xstatus);
      જો (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, કદ(fa));
  fd = open(".", 0);
  n = 0;
  જ્યારે (પઠ(fd, &de, કદ(de)) > 0) {
    જો (de.inum == 0)
      ચાલુ;
    જો (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      જો (i < 0 || i >= કદ(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      જો (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  જો (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  માટે (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } નહીંતર {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    જો (pid == 0)
      exit(0);
    નહીંતર
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
શૂન્ય
linkunlink(અક્ષર *s)
{
  પૂર્ણાંક pid, i;

  unlink("x");
  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned પૂર્ણાંક x = (pid ? 1 : 97);
  માટે (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    જો ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } નહીંતર જો ((x % 3) == 1) {
      link("cat", "x");
    } નહીંતર {
      unlink("x");
    }
  }

  જો (pid)
    wait(0);
  નહીંતર
    exit(0);
}

શૂન્ય
subdir(અક્ષર *s)
{
  પૂર્ણાંક fd, cc;

  unlink("ff");
  જો (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  લિખ(fd, "ff", 2);
  close(fd);

  જો (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  જો (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  લિખ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  જો (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = પઠ(fd, buf, કદ(buf));
  જો (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  જો (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  જો (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  જો (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  જો (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  જો (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  જો (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  જો (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  જો (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  જો (પઠ(fd, buf, કદ(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  જો (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  જો (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  જો (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  જો (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  જો (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  જો (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  જો (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  જો (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  જો (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  જો (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  જો (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  જો (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  જો (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  જો (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  જો (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  જો (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  જો (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  જો (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  જો (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  જો (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  જો (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
શૂન્ય
bigwrite(અક્ષર *s)
{
  પૂર્ણાંક fd, sz;

  unlink("bigwrite");
  માટે (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    જો (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    પૂર્ણાંક i;
    માટે (i = 0; i < 2; i++) {
      પૂર્ણાંક cc = લિખ(fd, buf, sz);
      જો (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

શૂન્ય
bigfile(અક્ષર *s)
{
  ગણના { N = 20, SZ = 600 };
  પૂર્ણાંક fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  માટે (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    જો (લિખ(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  જો (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  માટે (i = 0;; i++) {
    cc = પઠ(fd, buf, SZ / 2);
    જો (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    જો (cc == 0)
      તોડો;
    જો (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    જો (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  જો (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

શૂન્ય
fourteen(અક્ષર *s)
{
  પૂર્ણાંક fd;

  // DIRSIZ is 14.

  જો (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  જો (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  જો (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  જો (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  જો (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  જો (mkdir("123456789012345/12345678901234") == 0) {
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

શૂન્ય
rmdot(અક્ષર *s)
{
  જો (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  જો (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  જો (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  જો (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  જો (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  જો (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  જો (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  જો (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

શૂન્ય
dirfile(અક્ષર *s)
{
  પૂર્ણાંક fd;

  fd = open("dirfile", O_CREATE);
  જો (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  જો (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  જો (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  જો (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  જો (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  જો (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  જો (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  જો (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  જો (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  જો (લિખ(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
શૂન્ય
iref(અક્ષર *s)
{
  પૂર્ણાંક i, fd;

  માટે (i = 0; i < NINODE + 1; i++) {
    જો (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    જો (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    જો (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    જો (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  માટે (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
શૂન્ય
forktest(અક્ષર *s)
{
  ગણના { N = 1000 };
  પૂર્ણાંક n, pid;

  માટે (n = 0; n < N; n++) {
    pid = fork();
    જો (pid < 0)
      તોડો;
    જો (pid == 0)
      exit(0);
  }

  જો (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  જો (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  માટે (; n > 0; n--) {
    જો (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  જો (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

શૂન્ય
sbrkbasic(અક્ષર *s)
{
  ગણના { TOOMUCH = 1024 * 1024 * 1024 };
  પૂર્ણાંક i, pid, xstatus;
  અક્ષર *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  જો (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  જો (pid == 0) {
    a = sbrk(TOOMUCH);
    જો (a == (અક્ષર *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    માટે (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  જો (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  માટે (i = 0; i < 5000; i++) {
    b = sbrk(1);
    જો (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  જો (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  જો (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  જો (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

શૂન્ય
sbrkmuch(અક્ષર *s)
{
  ગણના { BIG = 100 * 1024 * 1024 };
  અક્ષર *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  જો (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (અક્ષર *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  જો (c == (અક્ષર *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  જો (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  જો (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  જો (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  જો (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
શૂન્ય
kernmem(અક્ષર *s)
{
  અક્ષર *a;
  પૂર્ણાંક pid;

  માટે (a = (અક્ષર *)(KERNBASE); a < (અક્ષર *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    પૂર્ણાંક xstatus;
    wait(&xstatus);
    જો (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
શૂન્ય
MAXVAplus(અક્ષર *s)
{
  અસ્થિર uint64 a = MAXVA;
  માટે (; a != 0; a <<= 1) {
    પૂર્ણાંક pid;
    pid = fork();
    જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    જો (pid == 0) {
      *(અક્ષર *)a = 99;
      printf("%s: oops wrote %p\n", s, (શૂન્ય *)a);
      exit(1);
    }
    પૂર્ણાંક xstatus;
    wait(&xstatus);
    જો (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
શૂન્ય
sbrkfail(અક્ષર *s)
{
  ગણના { BIG = 100 * 1024 * 1024 };
  પૂર્ણાંક i, xstatus;
  પૂર્ણાંક fds[2];
  અક્ષર scratch;
  અક્ષર *c, *a;
  પૂર્ણાંક pids[10];
  પૂર્ણાંક pid;
  પૂર્ણાંક failed;

  failed = 0;
  જો (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  માટે (i = 0; i < કદ(pids) / કદ(pids[0]); i++) {
    જો ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      જો (sbrk(BIG - (uint64)sbrk(0)) == (અક્ષર *)SBRK_ERROR)
        લિખ(fds[1], "0", 1);
      નહીંતર
        લિખ(fds[1], "1", 1);
      // sit around until killed
      માટે (;;)
        pause(1000);
    }
    જો (pids[i] != -1) {
      પઠ(fds[0], &scratch, 1);
      જો (scratch == '0')
        failed = 1;
    }
  }
  જો (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  માટે (i = 0; i < કદ(pids) / કદ(pids[0]); i++) {
    જો (pids[i] == -1)
      ચાલુ;
    kill(pids[i]);
    wait(0);
  }
  જો (c == (અક્ષર *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  જો (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    જો (a == (અક્ષર *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  જો (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
શૂન્ય
sbrkarg(અક્ષર *s)
{
  અક્ષર *a;
  પૂર્ણાંક fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  જો (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  જો ((n = લિખ(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  જો (pipe((પૂર્ણાંક *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

શૂન્ય
validatetest(અક્ષર *s)
{
  પૂર્ણાંક hi;
  uint64 p;

  hi = 1100 * 1024;
  માટે (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    જો (link("nosuchfile", (અક્ષર *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
અક્ષર uninit[10000];
શૂન્ય
bsstest(અક્ષર *s)
{
  પૂર્ણાંક i;

  માટે (i = 0; i < કદ(uninit); i++) {
    જો (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
શૂન્ય
bigargtest(અક્ષર *s)
{
  પૂર્ણાંક pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  જો (pid == 0) {
    સ્થિર અક્ષર *args[MAXARG];
    પૂર્ણાંક i;
    અક્ષર big[400];
    memset(big, ' ', કદ(big));
    big[કદ(big) - 1] = '\0';
    માટે (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } નહીંતર જો (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  જો (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  જો (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
શૂન્ય
fsfull()
{
  પૂર્ણાંક nfiles;
  પૂર્ણાંક fsblocks = 0;

  printf("fsfull test\n");

  માટે (nfiles = 0;; nfiles++) {
    અક્ષર name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    પૂર્ણાંક fd = open(name, O_CREATE | O_RDWR);
    જો (fd < 0) {
      printf("open %s failed\n", name);
      તોડો;
    }
    પૂર્ણાંક total = 0;
    જ્યારે (1) {
      પૂર્ણાંક cc = લિખ(fd, buf, BSIZE);
      જો (cc < BSIZE)
        તોડો;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    જો (total == 0)
      તોડો;
  }

  જ્યારે (nfiles >= 0) {
    અક્ષર name[64];
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

શૂન્ય
argptest(અક્ષર *s)
{
  પૂર્ણાંક fd;
  fd = open("init", O_RDONLY);
  જો (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  પઠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
શૂન્ય
stacktest(અક્ષર *s)
{
  પૂર્ણાંક pid;
  પૂર્ણાંક xstatus;

  pid = fork();
  જો (pid == 0) {
    અક્ષર *sp = (અક્ષર *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } નહીંતર જો (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  જો (xstatus == -1) // kernel killed child?
    exit(0);
  નહીંતર
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
શૂન્ય
nowrite(અક્ષર *s)
{
  પૂર્ણાંક pid;
  પૂર્ણાંક xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  માટે (પૂર્ણાંક ai = 0; ai < કદ(addrs) / કદ(addrs[0]); ai++) {
    pid = fork();
    જો (pid == 0) {
      અસ્થિર પૂર્ણાંક *addr = (પૂર્ણાંક *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } નહીંતર જો (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    જો (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
શૂન્ય *big = (શૂન્ય *)0xeaeb0b5b00002f5e;
શૂન્ય
pgbug(અક્ષર *s)
{
  અક્ષર *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
શૂન્ય
sbrkbugs(અક્ષર *s)
{
  પૂર્ણાંક pid = fork();
  જો (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  જો (pid == 0) {
    પૂર્ણાંક sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  જો (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  જો (pid == 0) {
    પૂર્ણાંક sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  જો (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  જો (pid == 0) {
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
શૂન્ય
sbrklast(અક્ષર *s)
{
  uint64 ટોચ = (uint64)sbrk(0);
  જો ((ટોચ % PGSIZE) != 0)
    sbrk(PGSIZE - (ટોચ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  ટોચ = (uint64)sbrk(0);
  અક્ષર *p = (અક્ષર *)(ટોચ - 64);
  p[0] = 'x';
  p[1] = '\0';
  પૂર્ણાંક fd = open(p, O_RDWR | O_CREATE);
  લિખ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  પઠ(fd, p, 1);
  જો (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
શૂન્ય
sbrk8000(અક્ષર *s)
{
  sbrk(0x80000004);
  અસ્થિર અક્ષર *ટોચ = sbrk(0);
  *(ટોચ - 1) = *(ટોચ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
શૂન્ય
badarg(અક્ષર *s)
{
  માટે (પૂર્ણાંક i = 0; i < 50000; i++) {
    અક્ષર *argv[2];
    argv[0] = (અક્ષર *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
શૂન્ય
lazy_alloc(અક્ષર *s)
{
  અક્ષર *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  જો (prev_end == (અક્ષર *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  માટે (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(અક્ષર **)i = i;

  માટે (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    જો (*(અક્ષર **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
શૂન્ય
lazy_unmap(અક્ષર *s)
{
  પૂર્ણાંક pid;
  અક્ષર *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  જો (prev_end == (અક્ષર *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  માટે (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(અક્ષર **)i = i;

  માટે (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    જો (pid < 0) {
      printf("error forking\n");
      exit(1);
    } નહીંતર જો (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(અક્ષર **)i = i;
      exit(0);
    } નહીંતર {
      પૂર્ણાંક status;
      wait(&status);
      જો (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

શૂન્ય
lazy_copy(અક્ષર *s)
{
  // copyinstr on lazy page
  {
    અક્ષર *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    શૂન્ય *xx = sbrk(0);
    શૂન્ય *ret = sbrk(-(((uint64)xx) + 1));
    જો (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  માટે (પૂર્ણાંક i = 0; i < કદ(bad) / કદ(bad[0]); i++) {
    પૂર્ણાંક fd = open("README", 0);
    જો (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    જો (પઠ(fd, (અક્ષર *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    જો (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    જો (લિખ(fd, (અક્ષર *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

શૂન્ય
lazy_copyinstr(અક્ષર *s)
{
  અક્ષર *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  જો ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  પૂર્ણાંક fd = open(&p[4095], O_RDONLY);
  જો (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  સંરચના stat st;
  પૂર્ણાંક r = fstat(fd, &st);
  જો (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  જો (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

શૂન્ય
lazy_sbrk(અક્ષર *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  અક્ષર *p = sbrk(0);
  જ્યારે ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    જો (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  પૂર્ણાંક n = TRAPFRAME - PGSIZE - (uint64)p;

  અક્ષર *p1 = sbrklazy(n);
  જો (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  જો (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  જો (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  જો ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  જો ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

શૂન્ય
partial_write(અક્ષર *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  પૂર્ણાંક fd = open("testfile", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  પૂર્ણાંક cc = લિખ(fd, "A", 1);
  જો (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  જો (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  અક્ષર *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  જો ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = લિખ(fd, p - 1, 2);
  જો (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  જો (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  અક્ષર b;
  cc = પઠ(fd, &b, 1);
  જો (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  જો (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  માટે (પૂર્ણાંક i = 0; i < 64; i++) {
    અક્ષર buf[1024];
    memset(buf, 0, કદ(buf));
    cc = લિખ(fd, buf, કદ(buf));
    જો (cc != કદ(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  જો (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = પઠ(fd, &b, 1);
  જો (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  જો (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

શૂન્ય
unlinkcwd(અક્ષર *s)
{
  જો (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  જો (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  જો (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  જો (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  જો (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  જો (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  જો (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

સંરચના test {
  શૂન્ય (*f)(અક્ષર *);
  અક્ષર *s;
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
શૂન્ય
bigdir(અક્ષર *s)
{
  ગણના { N = 500 };
  પૂર્ણાંક i, fd;
  અક્ષર name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  જો (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  માટે (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    જો (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  માટે (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    જો (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
શૂન્ય
manywrites(અક્ષર *s)
{
  પૂર્ણાંક nchildren = 4;
  પૂર્ણાંક howmany = 30; // increase to look for deadlock

  માટે (પૂર્ણાંક ci = 0; ci < nchildren; ci++) {
    પૂર્ણાંક pid = fork();
    જો (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    જો (pid == 0) {
      અક્ષર name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      માટે (પૂર્ણાંક iters = 0; iters < howmany; iters++) {
        માટે (પૂર્ણાંક i = 0; i < ci + 1; i++) {
          પૂર્ણાંક fd = open(name, O_CREATE | O_RDWR);
          જો (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          પૂર્ણાંક sz = કદ(buf);
          પૂર્ણાંક cc = લિખ(fd, buf, sz);
          જો (cc != sz) {
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

  માટે (પૂર્ણાંક ci = 0; ci < nchildren; ci++) {
    પૂર્ણાંક st = 0;
    wait(&st);
    જો (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
શૂન્ય
badwrite(અક્ષર *s)
{
  પૂર્ણાંક assumed_free = 600;

  unlink("junk");
  માટે (પૂર્ણાંક i = 0; i < assumed_free; i++) {
    પૂર્ણાંક fd = open("junk", O_CREATE | O_WRONLY);
    જો (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    લિખ(fd, (અક્ષર *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  પૂર્ણાંક fd = open("junk", O_CREATE | O_WRONLY);
  જો (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  જો (લિખ(fd, "x", 1) != 1) {
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
શૂન્ય
execout(અક્ષર *s)
{
  માટે (પૂર્ણાંક avail = 0; avail < 15; avail++) {
    પૂર્ણાંક pid = fork();
    જો (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } નહીંતર જો (pid == 0) {
      // allocate all of memory.
      જ્યારે (1) {
        અક્ષર *a = sbrk(PGSIZE);
        જો (a == SBRK_ERROR)
          તોડો;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      માટે (પૂર્ણાંક i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      અક્ષર *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } નહીંતર {
      wait((પૂર્ણાંક *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
શૂન્ય
diskfull(અક્ષર *s)
{
  પૂર્ણાંક fi;
  પૂર્ણાંક done = 0;

  unlink("diskfulldir");

  માટે (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    અક્ષર name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    પૂર્ણાંક fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    જો (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      તોડો;
    }
    માટે (પૂર્ણાંક i = 0; i < MAXFILE; i++) {
      અક્ષર buf[BSIZE];
      જો (લિખ(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        તોડો;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  પૂર્ણાંક nzz = 128;
  માટે (પૂર્ણાંક i = 0; i < nzz; i++) {
    અક્ષર name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    પૂર્ણાંક fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    જો (fd < 0)
      તોડો;
    close(fd);
  }

  // this mkdir() is expected to fail.
  જો (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  માટે (પૂર્ણાંક i = 0; i < nzz; i++) {
    અક્ષર name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  માટે (પૂર્ણાંક i = 0; '0' + i < 0177; i++) {
    અક્ષર name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

શૂન્ય
outofinodes(અક્ષર *s)
{
  પૂર્ણાંક nzz = 32 * 32;
  માટે (પૂર્ણાંક i = 0; i < nzz; i++) {
    અક્ષર name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    પૂર્ણાંક fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    જો (fd < 0) {
      // failure is eventually expected.
      તોડો;
    }
    close(fd);
  }

  માટે (પૂર્ણાંક i = 0; i < nzz; i++) {
    અક્ષર name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

શૂન્ય
linkoverflow(અક્ષર *s)
{
  ગણના { TARGET = 32768 };
  ગણના { DIRS = 64 };
  સંરચના stat st;
  પૂર્ણાંક i;

  unlink("/lof");
  પૂર્ણાંક fd = open("/lof", O_CREATE | O_RDWR);
  જો (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  માટે (i = 0; i < TARGET; i++) {
    પૂર્ણાંક d = i % DIRS;
    પૂર્ણાંક f = i / DIRS;

    અક્ષર pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    જો (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    જો (link("/lof", pn) < 0) {
      જો (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      જો (st.nlink >= 32767) {
        // overflow check succeeded.
        તોડો;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    જો (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  જો (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  જો (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

સંરચના test slowtests[] = {
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
પૂર્ણાંક
run(શૂન્ય f(અક્ષર *), અક્ષર *s)
{
  પૂર્ણાંક pid;
  પૂર્ણાંક xstatus;

  printf("test %s: ", s);
  જો ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  જો (pid == 0) {
    f(s);
    exit(0);
  } નહીંતર {
    wait(&xstatus);
    જો (xstatus != 0)
      printf("FAILED\n");
    નહીંતર
      printf("OK\n");
    પાછા xstatus == 0;
  }
}

પૂર્ણાંક
runtests(સંરચના test *tests, અક્ષર *justone, પૂર્ણાંક continuous)
{
  પૂર્ણાંક ntests = 0;
  માટે (સંરચના test *t = tests; t->s != 0; t++) {
    જો ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      જો (!run(t->f, t->s)) {
        જો (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          પાછા -1;
        }
      }
    }
  }
  પાછા ntests;
}

// use sbrk() to count how many free physical memory pages there are.
પૂર્ણાંક
countfree()
{
  પૂર્ણાંક n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  જ્યારે (1) {
    અક્ષર *a = sbrk(PGSIZE);
    જો (a == SBRK_ERROR) {
      તોડો;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  પાછા n;
}

પૂર્ણાંક
drivetests(પૂર્ણાંક quick, પૂર્ણાંક continuous, અક્ષર *justone)
{
  કરો {
    printf("usertests starting\n");
    પૂર્ણાંક free0 = countfree();
    પૂર્ણાંક free1 = 0;
    પૂર્ણાંક ntests = 0;
    પૂર્ણાંક n;
    n = runtests(quicktests, justone, continuous);
    જો (n < 0) {
      જો (continuous != 2) {
        પાછા 1;
      }
    } નહીંતર {
      ntests += n;
    }
    જો (!quick) {
      જો (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      જો (n < 0) {
        જો (continuous != 2) {
          પાછા 1;
        }
      } નહીંતર {
        ntests += n;
      }
    }
    જો ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      જો (continuous != 2) {
        પાછા 1;
      }
    }
    જો (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      પાછા 1;
    }
  } જ્યારે (continuous);
  પાછા 0;
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક continuous = 0;
  પૂર્ણાંક quick = 0;
  અક્ષર *justone = 0;

  જો (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } નહીંતર જો (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } નહીંતર જો (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } નહીંતર જો (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } નહીંતર જો (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  જો (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
