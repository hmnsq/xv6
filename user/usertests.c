#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#define BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

var_na buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
void
copyin(var_na *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ཆེད་དུ (ཧྲིལ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ཧྲིལ fd = open("copyin1", O_CREATE | O_WRONLY);
    གལ་ཏེ (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    ཧྲིལ n = write(fd, (void *)addr, 8192);
    གལ་ཏེ (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (void *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = write(1, (var_na *)addr, 8192);
    གལ་ཏེ (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }

    ཧྲིལ fds[2];
    གལ་ཏེ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], (var_na *)addr, 8192);
    གལ་ཏེ (n > 0) {
      printf("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (void *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
void
copyout(var_na *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  ཆེད་དུ (ཧྲིལ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ཧྲིལ fd = open("README", 0);
    གལ་ཏེ (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    ཧྲིལ n = read(fd, (void *)addr, 8192);
    གལ་ཏེ (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }
    close(fd);

    ཧྲིལ fds[2];
    གལ་ཏེ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], "x", 1);
    གལ་ཏེ (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = read(fds[0], (void *)addr, 8192);
    གལ་ཏེ (n > 0) {
      printf("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (void *)addr,
             n);
      exit(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
void
copyinstr1(var_na *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ཆེད་དུ (ཧྲིལ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ཧྲིལ fd = open((var_na *)addr, O_CREATE | O_WRONLY);
    གལ་ཏེ (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (void *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
void
copyinstr2(var_na *s)
{
  var_na b[MAXPATH + 1];

  ཆེད་དུ (ཧྲིལ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ཧྲིལ ret = unlink(b);
  གལ་ཏེ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ཧྲིལ fd = open(b, O_CREATE | O_WRONLY);
  གལ་ཏེ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  གལ་ཏེ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  var_na *args[] = {"xx", 0};
  ret = exec(b, args);
  གལ་ཏེ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ཧྲིལ pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    static var_na big[PGSIZE + 1];
    ཆེད་དུ (ཧྲིལ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    var_na *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    གལ་ཏེ (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  ཧྲིལ st = 0;
  wait(&st);
  གལ་ཏེ (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
void
copyinstr3(var_na *s)
{
  sbrk(8192);
  uint64 top = (uint64)sbrk(0);
  གལ་ཏེ ((top % PGSIZE) != 0) {
    sbrk(PGSIZE - (top % PGSIZE));
  }
  top = (uint64)sbrk(0);
  གལ་ཏེ (top % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  var_na *b = (var_na *)(top - 1);
  *b = 'x';

  ཧྲིལ ret = unlink(b);
  གལ་ཏེ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ཧྲིལ fd = open(b, O_CREATE | O_WRONLY);
  གལ་ཏེ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  གལ་ཏེ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  var_na *args[] = {"xx", 0};
  ret = exec(b, args);
  གལ་ཏེ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
void
rwsbrk(var_na *s)
{
  ཧྲིལ fd, n;

  uint64 a = (uint64)sbrk(8192);

  གལ་ཏེ (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  གལ་ཏེ (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  གལ་ཏེ (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = write(fd, (void *)(a + PGSIZE), 1024);
  གལ་ཏེ (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (void *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = read(fd, (void *)(a + PGSIZE), 10);
  གལ་ཏེ (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (void *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
void
truncate1(var_na *s)
{
  var_na buf[32];

  unlink("truncfile");
  ཧྲིལ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  write(fd1, "abcd", 4);
  close(fd1);

  ཧྲིལ fd2 = open("truncfile", O_RDONLY);
  ཧྲིལ n = read(fd2, buf, parimaa_na(buf));
  གལ་ཏེ (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ཧྲིལ fd3 = open("truncfile", O_RDONLY);
  n = read(fd3, buf, parimaa_na(buf));
  གལ་ཏེ (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, parimaa_na(buf));
  གལ་ཏེ (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  write(fd1, "abcdef", 6);

  n = read(fd3, buf, parimaa_na(buf));
  གལ་ཏེ (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, parimaa_na(buf));
  གལ་ཏེ (n != 2) {
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
void
truncate2(var_na *s)
{
  unlink("truncfile");

  ཧྲིལ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  write(fd1, "abcd", 4);

  ཧྲིལ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ཧྲིལ n = write(fd1, "x", 1);
  གལ་ཏེ (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

void
truncate3(var_na *s)
{
  ཧྲིལ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  གལ་ཏེ (pid == 0) {
    ཆེད་དུ (ཧྲིལ i = 0; i < 100; i++) {
      var_na buf[32];
      ཧྲིལ fd = open("truncfile", O_WRONLY);
      གལ་ཏེ (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      ཧྲིལ n = write(fd, "1234567890", 10);
      གལ་ཏེ (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      read(fd, buf, parimaa_na(buf));
      close(fd);
    }
    exit(0);
  }

  ཆེད་དུ (ཧྲིལ i = 0; i < 150; i++) {
    ཧྲིལ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    གལ་ཏེ (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    ཧྲིལ n = write(fd, "xxx", 3);
    གལ་ཏེ (n != 3) {
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
void
iputtest(var_na *s)
{
  གལ་ཏེ (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
void
exitiputtest(var_na *s)
{
  ཧྲིལ pid, xstatus;

  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    གལ་ཏེ (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    གལ་ཏེ (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    གལ་ཏེ (unlink("../iputdir") < 0) {
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
void
openiputtest(var_na *s)
{
  ཧྲིལ pid, xstatus;

  གལ་ཏེ (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    ཧྲིལ fd = open("oidir", O_RDWR);
    གལ་ཏེ (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  གལ་ཏེ (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

void
opentest(var_na *s)
{
  ཧྲིལ fd;

  fd = open("echo", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  གལ་ཏེ (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

void
writetest(var_na *s)
{
  ཧྲིལ fd;
  ཧྲིལ i;
  ga_nanaa { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  ཆེད་དུ (i = 0; i < N; i++) {
    གལ་ཏེ (write(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    གལ་ཏེ (write(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = read(fd, buf, N * SZ * 2);
  གལ་ཏེ (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

void
writebig(var_na *s)
{
  ཧྲིལ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  ཆེད་དུ (i = 0; i < MAXFILE; i++) {
    ((ཧྲིལ *)buf)[0] = i;
    གལ་ཏེ (write(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  ཆེད་དུ (;;) {
    i = read(fd, buf, BSIZE);
    གལ་ཏེ (i == 0) {
      གལ་ཏེ (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      bha_nga;
    } དེ་མིན གལ་ཏེ (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    གལ་ཏེ (((ཧྲིལ *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((ཧྲིལ *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  གལ་ཏེ (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
void
createtest(var_na *s)
{
  ཧྲིལ i, fd;
  ga_nanaa { N = 52 };

  var_na name[3];
  name[0] = 'a';
  name[2] = '\0';
  ཆེད་དུ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ཆེད་དུ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

void
dirtest(var_na *s)
{
  གལ་ཏེ (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  གལ་ཏེ (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  གལ་ཏེ (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  གལ་ཏེ (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

void
exectest(var_na *s)
{
  ཧྲིལ fd, xstatus, pid;
  var_na *echoargv[] = {"echo", "OK", 0};
  var_na buf[3];

  unlink("echo-ok");
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    ཧྲིལ errfd = dup(1);
    གལ་ཏེ (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    གལ་ཏེ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    གལ་ཏེ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    གལ་ཏེ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  གལ་ཏེ (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  གལ་ཏེ (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  གལ་ཏེ (read(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  གལ་ཏེ (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  དེ་མིན {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

void
pipe1(var_na *s)
{
  ཧྲིལ fds[2], pid, xstatus;
  ཧྲིལ seq, i, n, cc, total;
  ga_nanaa { N = 5, SZ = 1033 };

  གལ་ཏེ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  གལ་ཏེ (pid == 0) {
    close(fds[0]);
    ཆེད་དུ (n = 0; n < N; n++) {
      ཆེད་དུ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      གལ་ཏེ (write(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } དེ་མིན གལ་ཏེ (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    སྐབས ((n = read(fds[0], buf, cc)) > 0) {
      ཆེད་དུ (i = 0; i < n; i++) {
        གལ་ཏེ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ཕྱིར;
        }
      }
      total += n;
      cc = cc * 2;
      གལ་ཏེ (cc > parimaa_na(buf))
        cc = parimaa_na(buf);
    }
    གལ་ཏེ (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } དེ་མིན {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
void
killstatus(var_na *s)
{
  ཧྲིལ xst;

  ཆེད་དུ (ཧྲིལ i = 0; i < 100; i++) {
    ཧྲིལ pid1 = fork();
    གལ་ཏེ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid1 == 0) {
      སྐབས (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    གལ་ཏེ (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
void
killzero(var_na *s)
{
  ཧྲིལ pid, xst;

  kill(0);

  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    exit(7);
  }
  གལ་ཏེ (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  གལ་ཏེ (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
void
preempt(var_na *s)
{
  ཧྲིལ pid1, pid2, pid3;
  ཧྲིལ pfds[2];

  pid1 = fork();
  གལ་ཏེ (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  གལ་ཏེ (pid1 == 0)
    ཆེད་དུ (;;)
      ;

  pid2 = fork();
  གལ་ཏེ (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid2 == 0)
    ཆེད་དུ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  གལ་ཏེ (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid3 == 0) {
    close(pfds[0]);
    གལ་ཏེ (write(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    ཆེད་དུ (;;)
      ;
  }

  close(pfds[1]);
  གལ་ཏེ (read(pfds[0], buf, parimaa_na(buf)) != 1) {
    printf("%s: preempt read error", s);
    ཕྱིར;
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
void
exitwait(var_na *s)
{
  ཧྲིལ i, pid;

  ཆེད་དུ (i = 0; i < 100; i++) {
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid) {
      ཧྲིལ xstate;
      གལ་ཏེ (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      གལ་ཏེ (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } དེ་མིན {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
void
reparent(var_na *s)
{
  ཧྲིལ master_pid = getpid();
  ཆེད་དུ (ཧྲིལ i = 0; i < 200; i++) {
    ཧྲིལ pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid) {
      གལ་ཏེ (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } དེ་མིན {
      ཧྲིལ pid2 = fork();
      གལ་ཏེ (pid2 < 0) {
        kill(master_pid);
        exit(1);
      }
      exit(0);
    }
  }
  exit(0);
}

// what if two children exit() at the same time?
void
twochildren(var_na *s)
{
  ཆེད་དུ (ཧྲིལ i = 0; i < 1000; i++) {
    ཧྲིལ pid1 = fork();
    གལ་ཏེ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid1 == 0) {
      exit(0);
    } དེ་མིན {
      ཧྲིལ pid2 = fork();
      གལ་ཏེ (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      གལ་ཏེ (pid2 == 0) {
        exit(0);
      } དེ་མིན {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
void
forkfork(var_na *s)
{
  ga_nanaa { N = 2 };

  ཆེད་དུ (ཧྲིལ i = 0; i < N; i++) {
    ཧྲིལ pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    གལ་ཏེ (pid == 0) {
      ཆེད་དུ (ཧྲིལ j = 0; j < 200; j++) {
        ཧྲིལ pid1 = fork();
        གལ་ཏེ (pid1 < 0) {
          exit(1);
        }
        གལ་ཏེ (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  ཧྲིལ xstatus;
  ཆེད་དུ (ཧྲིལ i = 0; i < N; i++) {
    wait(&xstatus);
    གལ་ཏེ (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

void
forkforkfork(var_na *s)
{
  unlink("stopforking");

  ཧྲིལ pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    སྐབས (1) {
      ཧྲིལ fd = open("stopforking", 0);
      གལ་ཏེ (fd >= 0) {
        exit(0);
      }
      གལ་ཏེ (fork() < 0) {
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
void
reparent2(var_na *s)
{
  ཆེད་དུ (ཧྲིལ i = 0; i < 800; i++) {
    ཧྲིལ pid1 = fork();
    གལ་ཏེ (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    གལ་ཏེ (pid1 == 0) {
      fork();
      fork();
      exit(0);
    }
    wait(0);
  }

  exit(0);
}

// allocate all mem, free it, and allocate again
void
mem(var_na *s)
{
  void *m1, *m2;
  ཧྲིལ pid;

  གལ་ཏེ ((pid = fork()) == 0) {
    m1 = 0;
    སྐབས ((m2 = malloc(10001)) != 0) {
      *(var_na **)m2 = m1;
      m1 = m2;
    }
    སྐབས (m1) {
      m2 = *(var_na **)m1;
      free(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    གལ་ཏེ (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    free(m1);
    exit(0);
  } དེ་མིན {
    ཧྲིལ xstatus;
    wait(&xstatus);
    གལ་ཏེ (xstatus == -1) {
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
void
sharedfd(var_na *s)
{
  ཧྲིལ fd, pid, i, n, nc, np;
  ga_nanaa { N = 1000, SZ = 10 };
  var_na buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', parimaa_na(buf));
  ཆེད་དུ (i = 0; i < N; i++) {
    གལ་ཏེ (write(fd, buf, parimaa_na(buf)) != parimaa_na(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  གལ་ཏེ (pid == 0) {
    exit(0);
  } དེ་མིན {
    ཧྲིལ xstatus;
    wait(&xstatus);
    གལ་ཏེ (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  སྐབས ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ཆེད་དུ (i = 0; i < parimaa_na(buf); i++) {
      གལ་ཏེ (buf[i] == 'c')
        nc++;
      གལ་ཏེ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  གལ་ཏེ (nc == N * SZ && np == N * SZ) {
    exit(0);
  } དེ་མིན {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
void
fourfiles(var_na *s)
{
  ཧྲིལ fd, pid, i, j, n, total, pi;
  var_na *names[] = {"f0", "f1", "f2", "f3"};
  var_na *fname;
  ga_nanaa { N = 12, NCHILD = 4, SZ = 500 };

  ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    གལ་ཏེ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      གལ་ཏེ (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      ཆེད་དུ (i = 0; i < N; i++) {
        གལ་ཏེ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  ཧྲིལ xstatus;
  ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    གལ་ཏེ (xstatus != 0)
      exit(xstatus);
  }

  ཆེད་དུ (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    སྐབས ((n = read(fd, buf, parimaa_na(buf))) > 0) {
      ཆེད་དུ (j = 0; j < n; j++) {
        གལ་ཏེ (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    གལ་ཏེ (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
void
createdelete(var_na *s)
{
  ga_nanaa { N = 20, NCHILD = 4 };
  ཧྲིལ pid, i, fd, pi;
  var_na name[32];

  ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    གལ་ཏེ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ཆེད་དུ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        གལ་ཏེ (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        གལ་ཏེ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          གལ་ཏེ (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  ཧྲིལ xstatus;
  ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    གལ་ཏེ (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  ཆེད་དུ (i = 0; i < N; i++) {
    ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      གལ་ཏེ ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } དེ་མིན གལ་ཏེ ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      གལ་ཏེ (fd >= 0)
        close(fd);
    }
  }

  ཆེད་དུ (i = 0; i < N; i++) {
    ཆེད་དུ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
void
unlinkread(var_na *s)
{
  ga_nanaa { SZ = 5 };
  ཧྲིལ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  write(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  གལ་ཏེ (read(fd, buf, parimaa_na(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  གལ་ཏེ (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  གལ་ཏེ (write(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

void
linktest(var_na *s)
{
  ga_nanaa { SZ = 5 };
  ཧྲིལ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  གལ་ཏེ (write(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  གལ་ཏེ (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  གལ་ཏེ (read(fd, buf, parimaa_na(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  གལ་ཏེ (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  གལ་ཏེ (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
void
concreate(var_na *s)
{
  ga_nanaa { N = 40 };
  var_na file[3];
  ཧྲིལ i, pid, n, fd;
  var_na fa[N];
  sa_nracanaa {
    ushort inum;
    var_na name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ཆེད་དུ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    གལ་ཏེ (pid && (i % 3) == 1) {
      link("C0", file);
    } དེ་མིན གལ་ཏེ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } དེ་མིན {
      fd = open(file, O_CREATE | O_RDWR);
      གལ་ཏེ (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    གལ་ཏེ (pid == 0) {
      exit(0);
    } དེ་མིན {
      ཧྲིལ xstatus;
      wait(&xstatus);
      གལ་ཏེ (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, parimaa_na(fa));
  fd = open(".", 0);
  n = 0;
  སྐབས (read(fd, &de, parimaa_na(de)) > 0) {
    གལ་ཏེ (de.inum == 0)
      continue;
    གལ་ཏེ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      གལ་ཏེ (i < 0 || i >= parimaa_na(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      གལ་ཏེ (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  གལ་ཏེ (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  ཆེད་དུ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } དེ་མིན {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    གལ་ཏེ (pid == 0)
      exit(0);
    དེ་མིན
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink(var_na *s)
{
  ཧྲིལ pid, i;

  unlink("x");
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned ཧྲིལ x = (pid ? 1 : 97);
  ཆེད་དུ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    གལ་ཏེ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } དེ་མིན གལ་ཏེ ((x % 3) == 1) {
      link("cat", "x");
    } དེ་མིན {
      unlink("x");
    }
  }

  གལ་ཏེ (pid)
    wait(0);
  དེ་མིན
    exit(0);
}

void
subdir(var_na *s)
{
  ཧྲིལ fd, cc;

  unlink("ff");
  གལ་ཏེ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "ff", 2);
  close(fd);

  གལ་ཏེ (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  གལ་ཏེ (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = read(fd, buf, parimaa_na(buf));
  གལ་ཏེ (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  གལ་ཏེ (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  གལ་ཏེ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  གལ་ཏེ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  གལ་ཏེ (read(fd, buf, parimaa_na(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  གལ་ཏེ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  གལ་ཏེ (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
void
bigwrite(var_na *s)
{
  ཧྲིལ fd, sz;

  unlink("bigwrite");
  ཆེད་དུ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    གལ་ཏེ (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    ཧྲིལ i;
    ཆེད་དུ (i = 0; i < 2; i++) {
      ཧྲིལ cc = write(fd, buf, sz);
      གལ་ཏེ (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

void
bigfile(var_na *s)
{
  ga_nanaa { N = 20, SZ = 600 };
  ཧྲིལ fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  ཆེད་དུ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    གལ་ཏེ (write(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  ཆེད་དུ (i = 0;; i++) {
    cc = read(fd, buf, SZ / 2);
    གལ་ཏེ (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    གལ་ཏེ (cc == 0)
      bha_nga;
    གལ་ཏེ (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    གལ་ཏེ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  གལ་ཏེ (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

void
fourteen(var_na *s)
{
  ཧྲིལ fd;

  // DIRSIZ is 14.

  གལ་ཏེ (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  གལ་ཏེ (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  གལ་ཏེ (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("123456789012345/12345678901234") == 0) {
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

void
rmdot(var_na *s)
{
  གལ་ཏེ (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

void
dirfile(var_na *s)
{
  ཧྲིལ fd;

  fd = open("dirfile", O_CREATE);
  གལ་ཏེ (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  གལ་ཏེ (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  གལ་ཏེ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  གལ་ཏེ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  གལ་ཏེ (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  གལ་ཏེ (write(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
void
iref(var_na *s)
{
  ཧྲིལ i, fd;

  ཆེད་དུ (i = 0; i < NINODE + 1; i++) {
    གལ་ཏེ (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    གལ་ཏེ (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    གལ་ཏེ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    གལ་ཏེ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ཆེད་དུ (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
void
forktest(var_na *s)
{
  ga_nanaa { N = 1000 };
  ཧྲིལ n, pid;

  ཆེད་དུ (n = 0; n < N; n++) {
    pid = fork();
    གལ་ཏེ (pid < 0)
      bha_nga;
    གལ་ཏེ (pid == 0)
      exit(0);
  }

  གལ་ཏེ (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  གལ་ཏེ (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  ཆེད་དུ (; n > 0; n--) {
    གལ་ཏེ (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  གལ་ཏེ (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

void
sbrkbasic(var_na *s)
{
  ga_nanaa { TOOMUCH = 1024 * 1024 * 1024 };
  ཧྲིལ i, pid, xstatus;
  var_na *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    a = sbrk(TOOMUCH);
    གལ་ཏེ (a == (var_na *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    ཆེད་དུ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  གལ་ཏེ (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ཆེད་དུ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    གལ་ཏེ (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  གལ་ཏེ (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

void
sbrkmuch(var_na *s)
{
  ga_nanaa { BIG = 100 * 1024 * 1024 };
  var_na *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  གལ་ཏེ (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (var_na *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  གལ་ཏེ (c == (var_na *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  གལ་ཏེ (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  གལ་ཏེ (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  གལ་ཏེ (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  གལ་ཏེ (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
void
kernmem(var_na *s)
{
  var_na *a;
  ཧྲིལ pid;

  ཆེད་དུ (a = (var_na *)(KERNBASE); a < (var_na *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    ཧྲིལ xstatus;
    wait(&xstatus);
    གལ་ཏེ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
void
MAXVAplus(var_na *s)
{
  ca_ncala uint64 a = MAXVA;
  ཆེད་དུ (; a != 0; a <<= 1) {
    ཧྲིལ pid;
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    གལ་ཏེ (pid == 0) {
      *(var_na *)a = 99;
      printf("%s: oops wrote %p\n", s, (void *)a);
      exit(1);
    }
    ཧྲིལ xstatus;
    wait(&xstatus);
    གལ་ཏེ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
void
sbrkfail(var_na *s)
{
  ga_nanaa { BIG = 100 * 1024 * 1024 };
  ཧྲིལ i, xstatus;
  ཧྲིལ fds[2];
  var_na scratch;
  var_na *c, *a;
  ཧྲིལ pids[10];
  ཧྲིལ pid;
  ཧྲིལ failed;

  failed = 0;
  གལ་ཏེ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  ཆེད་དུ (i = 0; i < parimaa_na(pids) / parimaa_na(pids[0]); i++) {
    གལ་ཏེ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      གལ་ཏེ (sbrk(BIG - (uint64)sbrk(0)) == (var_na *)SBRK_ERROR)
        write(fds[1], "0", 1);
      དེ་མིན
        write(fds[1], "1", 1);
      // sit around until killed
      ཆེད་དུ (;;)
        pause(1000);
    }
    གལ་ཏེ (pids[i] != -1) {
      read(fds[0], &scratch, 1);
      གལ་ཏེ (scratch == '0')
        failed = 1;
    }
  }
  གལ་ཏེ (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ཆེད་དུ (i = 0; i < parimaa_na(pids) / parimaa_na(pids[0]); i++) {
    གལ་ཏེ (pids[i] == -1)
      continue;
    kill(pids[i]);
    wait(0);
  }
  གལ་ཏེ (c == (var_na *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    གལ་ཏེ (a == (var_na *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  གལ་ཏེ (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
void
sbrkarg(var_na *s)
{
  var_na *a;
  ཧྲིལ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  གལ་ཏེ (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  གལ་ཏེ ((n = write(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  གལ་ཏེ (pipe((ཧྲིལ *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

void
validatetest(var_na *s)
{
  ཧྲིལ hi;
  uint64 p;

  hi = 1100 * 1024;
  ཆེད་དུ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    གལ་ཏེ (link("nosuchfile", (var_na *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
var_na uninit[10000];
void
bsstest(var_na *s)
{
  ཧྲིལ i;

  ཆེད་དུ (i = 0; i < parimaa_na(uninit); i++) {
    གལ་ཏེ (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
void
bigargtest(var_na *s)
{
  ཧྲིལ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  གལ་ཏེ (pid == 0) {
    static var_na *args[MAXARG];
    ཧྲིལ i;
    var_na big[400];
    memset(big, ' ', parimaa_na(big));
    big[parimaa_na(big) - 1] = '\0';
    ཆེད་དུ (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } དེ་མིན གལ་ཏེ (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  གལ་ཏེ (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  གལ་ཏེ (fd < 0) {
    printf("%s: bigarg test failed!\n", s);
    exit(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
void
fsfull()
{
  ཧྲིལ nfiles;
  ཧྲིལ fsblocks = 0;

  printf("fsfull test\n");

  ཆེད་དུ (nfiles = 0;; nfiles++) {
    var_na name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    ཧྲིལ fd = open(name, O_CREATE | O_RDWR);
    གལ་ཏེ (fd < 0) {
      printf("open %s failed\n", name);
      bha_nga;
    }
    ཧྲིལ total = 0;
    སྐབས (1) {
      ཧྲིལ cc = write(fd, buf, BSIZE);
      གལ་ཏེ (cc < BSIZE)
        bha_nga;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    གལ་ཏེ (total == 0)
      bha_nga;
  }

  སྐབས (nfiles >= 0) {
    var_na name[64];
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

void
argptest(var_na *s)
{
  ཧྲིལ fd;
  fd = open("init", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  read(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
void
stacktest(var_na *s)
{
  ཧྲིལ pid;
  ཧྲིལ xstatus;

  pid = fork();
  གལ་ཏེ (pid == 0) {
    var_na *sp = (var_na *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } དེ་མིན གལ་ཏེ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  གལ་ཏེ (xstatus == -1) // kernel killed child?
    exit(0);
  དེ་མིན
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
void
nowrite(var_na *s)
{
  ཧྲིལ pid;
  ཧྲིལ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ཆེད་དུ (ཧྲིལ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    pid = fork();
    གལ་ཏེ (pid == 0) {
      ca_ncala ཧྲིལ *addr = (ཧྲིལ *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } དེ་མིན གལ་ཏེ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    གལ་ཏེ (xstatus == 0) {
      // kernel did not kill child!
      exit(1);
    }
  }
  exit(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
void *big = (void *)0xeaeb0b5b00002f5e;
void
pgbug(var_na *s)
{
  var_na *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
void
sbrkbugs(var_na *s)
{
  ཧྲིལ pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    ཧྲིལ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    ཧྲིལ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  གལ་ཏེ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
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
void
sbrklast(var_na *s)
{
  uint64 top = (uint64)sbrk(0);
  གལ་ཏེ ((top % PGSIZE) != 0)
    sbrk(PGSIZE - (top % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  top = (uint64)sbrk(0);
  var_na *p = (var_na *)(top - 64);
  p[0] = 'x';
  p[1] = '\0';
  ཧྲིལ fd = open(p, O_RDWR | O_CREATE);
  write(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  read(fd, p, 1);
  གལ་ཏེ (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
void
sbrk8000(var_na *s)
{
  sbrk(0x80000004);
  ca_ncala var_na *top = sbrk(0);
  *(top - 1) = *(top - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
void
badarg(var_na *s)
{
  ཆེད་དུ (ཧྲིལ i = 0; i < 50000; i++) {
    var_na *argv[2];
    argv[0] = (var_na *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
void
lazy_alloc(var_na *s)
{
  var_na *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  གལ་ཏེ (prev_end == (var_na *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ཆེད་དུ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(var_na **)i = i;

  ཆེད་དུ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    གལ་ཏེ (*(var_na **)i != i) {
      printf("failed to read value from memory\n");
      exit(1);
    }
  }

  exit(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
void
lazy_unmap(var_na *s)
{
  ཧྲིལ pid;
  var_na *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  གལ་ཏེ (prev_end == (var_na *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ཆེད་དུ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(var_na **)i = i;

  ཆེད་དུ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("error forking\n");
      exit(1);
    } དེ་མིན གལ་ཏེ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(var_na **)i = i;
      exit(0);
    } དེ་མིན {
      ཧྲིལ status;
      wait(&status);
      གལ་ཏེ (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

void
lazy_copy(var_na *s)
{
  // copyinstr on lazy page
  {
    var_na *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    void *xx = sbrk(0);
    void *ret = sbrk(-(((uint64)xx) + 1));
    གལ་ཏེ (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ཆེད་དུ (ཧྲིལ i = 0; i < parimaa_na(bad) / parimaa_na(bad[0]); i++) {
    ཧྲིལ fd = open("README", 0);
    གལ་ཏེ (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    གལ་ཏེ (read(fd, (var_na *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    གལ་ཏེ (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    གལ་ཏེ (write(fd, (var_na *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

void
lazy_copyinstr(var_na *s)
{
  var_na *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  གལ་ཏེ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ཧྲིལ fd = open(&p[4095], O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  sa_nracanaa stat st;
  ཧྲིལ r = fstat(fd, &st);
  གལ་ཏེ (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  གལ་ཏེ (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

void
lazy_sbrk(var_na *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  var_na *p = sbrk(0);
  སྐབས ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    གལ་ཏེ (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  ཧྲིལ n = TRAPFRAME - PGSIZE - (uint64)p;

  var_na *p1 = sbrklazy(n);
  གལ་ཏེ (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  གལ་ཏེ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  གལ་ཏེ (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  གལ་ཏེ ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  གལ་ཏེ ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

void
partial_write(var_na *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ཧྲིལ fd = open("testfile", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  ཧྲིལ cc = write(fd, "A", 1);
  གལ་ཏེ (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  var_na *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  གལ་ཏེ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = write(fd, p - 1, 2);
  གལ་ཏེ (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  var_na b;
  cc = read(fd, &b, 1);
  གལ་ཏེ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  གལ་ཏེ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ཆེད་དུ (ཧྲིལ i = 0; i < 64; i++) {
    var_na buf[1024];
    memset(buf, 0, parimaa_na(buf));
    cc = write(fd, buf, parimaa_na(buf));
    གལ་ཏེ (cc != parimaa_na(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = read(fd, &b, 1);
  གལ་ཏེ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  གལ་ཏེ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

void
unlinkcwd(var_na *s)
{
  གལ་ཏེ (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  གལ་ཏེ (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  གལ་ཏེ (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  གལ་ཏེ (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  གལ་ཏེ (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  གལ་ཏེ (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

sa_nracanaa test {
  void (*f)(var_na *);
  var_na *s;
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
void
bigdir(var_na *s)
{
  ga_nanaa { N = 500 };
  ཧྲིལ i, fd;
  var_na name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  གལ་ཏེ (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  ཆེད་དུ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    གལ་ཏེ (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  ཆེད་དུ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    གལ་ཏེ (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
void
manywrites(var_na *s)
{
  ཧྲིལ nchildren = 4;
  ཧྲིལ howmany = 30; // increase to look for deadlock

  ཆེད་དུ (ཧྲིལ ci = 0; ci < nchildren; ci++) {
    ཧྲིལ pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    གལ་ཏེ (pid == 0) {
      var_na name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ཆེད་དུ (ཧྲིལ iters = 0; iters < howmany; iters++) {
        ཆེད་དུ (ཧྲིལ i = 0; i < ci + 1; i++) {
          ཧྲིལ fd = open(name, O_CREATE | O_RDWR);
          གལ་ཏེ (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          ཧྲིལ sz = parimaa_na(buf);
          ཧྲིལ cc = write(fd, buf, sz);
          གལ་ཏེ (cc != sz) {
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

  ཆེད་དུ (ཧྲིལ ci = 0; ci < nchildren; ci++) {
    ཧྲིལ st = 0;
    wait(&st);
    གལ་ཏེ (st != 0)
      exit(st);
  }
  exit(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
void
badwrite(var_na *s)
{
  ཧྲིལ assumed_free = 600;

  unlink("junk");
  ཆེད་དུ (ཧྲིལ i = 0; i < assumed_free; i++) {
    ཧྲིལ fd = open("junk", O_CREATE | O_WRONLY);
    གལ་ཏེ (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    write(fd, (var_na *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ཧྲིལ fd = open("junk", O_CREATE | O_WRONLY);
  གལ་ཏེ (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  གལ་ཏེ (write(fd, "x", 1) != 1) {
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
void
execout(var_na *s)
{
  ཆེད་དུ (ཧྲིལ avail = 0; avail < 15; avail++) {
    ཧྲིལ pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } དེ་མིན གལ་ཏེ (pid == 0) {
      // allocate all of memory.
      སྐབས (1) {
        var_na *a = sbrk(PGSIZE);
        གལ་ཏེ (a == SBRK_ERROR)
          bha_nga;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ཆེད་དུ (ཧྲིལ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      var_na *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } དེ་མིན {
      wait((ཧྲིལ *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
void
diskfull(var_na *s)
{
  ཧྲིལ fi;
  ཧྲིལ done = 0;

  unlink("diskfulldir");

  ཆེད་དུ (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    var_na name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ཧྲིལ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    གལ་ཏེ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      bha_nga;
    }
    ཆེད་དུ (ཧྲིལ i = 0; i < MAXFILE; i++) {
      var_na buf[BSIZE];
      གལ་ཏེ (write(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        bha_nga;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ཧྲིལ nzz = 128;
  ཆེད་དུ (ཧྲིལ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ཧྲིལ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    གལ་ཏེ (fd < 0)
      bha_nga;
    close(fd);
  }

  // this mkdir() is expected to fail.
  གལ་ཏེ (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ཆེད་དུ (ཧྲིལ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ཆེད་དུ (ཧྲིལ i = 0; '0' + i < 0177; i++) {
    var_na name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

void
outofinodes(var_na *s)
{
  ཧྲིལ nzz = 32 * 32;
  ཆེད་དུ (ཧྲིལ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ཧྲིལ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    གལ་ཏེ (fd < 0) {
      // failure is eventually expected.
      bha_nga;
    }
    close(fd);
  }

  ཆེད་དུ (ཧྲིལ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

void
linkoverflow(var_na *s)
{
  ga_nanaa { TARGET = 32768 };
  ga_nanaa { DIRS = 64 };
  sa_nracanaa stat st;
  ཧྲིལ i;

  unlink("/lof");
  ཧྲིལ fd = open("/lof", O_CREATE | O_RDWR);
  གལ་ཏེ (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  ཆེད་དུ (i = 0; i < TARGET; i++) {
    ཧྲིལ d = i % DIRS;
    ཧྲིལ f = i / DIRS;

    var_na pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    གལ་ཏེ (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    གལ་ཏེ (link("/lof", pn) < 0) {
      གལ་ཏེ (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      གལ་ཏེ (st.nlink >= 32767) {
        // overflow check succeeded.
        bha_nga;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    གལ་ཏེ (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  གལ་ཏེ (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  གལ་ཏེ (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

sa_nracanaa test slowtests[] = {
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
ཧྲིལ
run(void f(var_na *), var_na *s)
{
  ཧྲིལ pid;
  ཧྲིལ xstatus;

  printf("test %s: ", s);
  གལ་ཏེ ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  གལ་ཏེ (pid == 0) {
    f(s);
    exit(0);
  } དེ་མིན {
    wait(&xstatus);
    གལ་ཏེ (xstatus != 0)
      printf("FAILED\n");
    དེ་མིན
      printf("OK\n");
    ཕྱིར xstatus == 0;
  }
}

ཧྲིལ
runtests(sa_nracanaa test *tests, var_na *justone, ཧྲིལ continuous)
{
  ཧྲིལ ntests = 0;
  ཆེད་དུ (sa_nracanaa test *t = tests; t->s != 0; t++) {
    གལ་ཏེ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      གལ་ཏེ (!run(t->f, t->s)) {
        གལ་ཏེ (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ཕྱིར -1;
        }
      }
    }
  }
  ཕྱིར ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ཧྲིལ
countfree()
{
  ཧྲིལ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  སྐབས (1) {
    var_na *a = sbrk(PGSIZE);
    གལ་ཏེ (a == SBRK_ERROR) {
      bha_nga;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ཕྱིར n;
}

ཧྲིལ
drivetests(ཧྲིལ quick, ཧྲིལ continuous, var_na *justone)
{
  do {
    printf("usertests starting\n");
    ཧྲིལ free0 = countfree();
    ཧྲིལ free1 = 0;
    ཧྲིལ ntests = 0;
    ཧྲིལ n;
    n = runtests(quicktests, justone, continuous);
    གལ་ཏེ (n < 0) {
      གལ་ཏེ (continuous != 2) {
        ཕྱིར 1;
      }
    } དེ་མིན {
      ntests += n;
    }
    གལ་ཏེ (!quick) {
      གལ་ཏེ (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      གལ་ཏེ (n < 0) {
        གལ་ཏེ (continuous != 2) {
          ཕྱིར 1;
        }
      } དེ་མིན {
        ntests += n;
      }
    }
    གལ་ཏེ ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      གལ་ཏེ (continuous != 2) {
        ཕྱིར 1;
      }
    }
    གལ་ཏེ (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ཕྱིར 1;
    }
  } སྐབས (continuous);
  ཕྱིར 0;
}

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ continuous = 0;
  ཧྲིལ quick = 0;
  var_na *justone = 0;

  གལ་ཏེ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } དེ་མིན གལ་ཏེ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } དེ་མིན གལ་ཏེ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } དེ་མིན གལ་ཏེ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } དེ་མིན གལ་ཏེ (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  གལ་ཏེ (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
