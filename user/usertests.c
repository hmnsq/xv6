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

  ສຳລັບ (ຈຳນວນເຕັມ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ຈຳນວນເຕັມ fd = open("copyin1", O_CREATE | O_WRONLY);
    ຖ້າ (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    ຈຳນວນເຕັມ n = write(fd, (void *)addr, 8192);
    ຖ້າ (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (void *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = write(1, (var_na *)addr, 8192);
    ຖ້າ (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }

    ຈຳນວນເຕັມ fds[2];
    ຖ້າ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], (var_na *)addr, 8192);
    ຖ້າ (n > 0) {
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

  ສຳລັບ (ຈຳນວນເຕັມ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ຈຳນວນເຕັມ fd = open("README", 0);
    ຖ້າ (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    ຈຳນວນເຕັມ n = read(fd, (void *)addr, 8192);
    ຖ້າ (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }
    close(fd);

    ຈຳນວນເຕັມ fds[2];
    ຖ້າ (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], "x", 1);
    ຖ້າ (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = read(fds[0], (void *)addr, 8192);
    ຖ້າ (n > 0) {
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

  ສຳລັບ (ຈຳນວນເຕັມ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ຈຳນວນເຕັມ fd = open((var_na *)addr, O_CREATE | O_WRONLY);
    ຖ້າ (fd >= 0) {
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

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ຈຳນວນເຕັມ ret = unlink(b);
  ຖ້າ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ຈຳນວນເຕັມ fd = open(b, O_CREATE | O_WRONLY);
  ຖ້າ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ຖ້າ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  var_na *args[] = {"xx", 0};
  ret = exec(b, args);
  ຖ້າ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ຈຳນວນເຕັມ pid = fork();
  ຖ້າ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
    static var_na big[PGSIZE + 1];
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    var_na *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ຖ້າ (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  ຈຳນວນເຕັມ st = 0;
  wait(&st);
  ຖ້າ (st != 747) {
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
  ຖ້າ ((top % PGSIZE) != 0) {
    sbrk(PGSIZE - (top % PGSIZE));
  }
  top = (uint64)sbrk(0);
  ຖ້າ (top % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  var_na *b = (var_na *)(top - 1);
  *b = 'x';

  ຈຳນວນເຕັມ ret = unlink(b);
  ຖ້າ (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ຈຳນວນເຕັມ fd = open(b, O_CREATE | O_WRONLY);
  ຖ້າ (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  ຖ້າ (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  var_na *args[] = {"xx", 0};
  ret = exec(b, args);
  ຖ້າ (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
void
rwsbrk(var_na *s)
{
  ຈຳນວນເຕັມ fd, n;

  uint64 a = (uint64)sbrk(8192);

  ຖ້າ (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  ຖ້າ (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ຖ້າ (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = write(fd, (void *)(a + PGSIZE), 1024);
  ຖ້າ (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (void *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = read(fd, (void *)(a + PGSIZE), 10);
  ຖ້າ (n >= 0) {
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
  ຈຳນວນເຕັມ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  write(fd1, "abcd", 4);
  close(fd1);

  ຈຳນວນເຕັມ fd2 = open("truncfile", O_RDONLY);
  ຈຳນວນເຕັມ n = read(fd2, buf, parimaa_na(buf));
  ຖ້າ (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ຈຳນວນເຕັມ fd3 = open("truncfile", O_RDONLY);
  n = read(fd3, buf, parimaa_na(buf));
  ຖ້າ (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, parimaa_na(buf));
  ຖ້າ (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  write(fd1, "abcdef", 6);

  n = read(fd3, buf, parimaa_na(buf));
  ຖ້າ (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, parimaa_na(buf));
  ຖ້າ (n != 2) {
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

  ຈຳນວນເຕັມ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  write(fd1, "abcd", 4);

  ຈຳນວນເຕັມ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ຈຳນວນເຕັມ n = write(fd1, "x", 1);
  ຖ້າ (n != -1) {
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
  ຈຳນວນເຕັມ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  ຖ້າ (pid == 0) {
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 100; i++) {
      var_na buf[32];
      ຈຳນວນເຕັມ fd = open("truncfile", O_WRONLY);
      ຖ້າ (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      ຈຳນວນເຕັມ n = write(fd, "1234567890", 10);
      ຖ້າ (n != 10) {
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

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 150; i++) {
    ຈຳນວນເຕັມ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ຖ້າ (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    ຈຳນວນເຕັມ n = write(fd, "xxx", 3);
    ຖ້າ (n != 3) {
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
  ຖ້າ (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
void
exitiputtest(var_na *s)
{
  ຈຳນວນເຕັມ pid, xstatus;

  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ຖ້າ (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    ຖ້າ (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    ຖ້າ (unlink("../iputdir") < 0) {
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
  ຈຳນວນເຕັມ pid, xstatus;

  ຖ້າ (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ຈຳນວນເຕັມ fd = open("oidir", O_RDWR);
    ຖ້າ (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  ຖ້າ (unlink("oidir") != 0) {
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
  ຈຳນວນເຕັມ fd;

  fd = open("echo", 0);
  ຖ້າ (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ຖ້າ (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

void
writetest(var_na *s)
{
  ຈຳນວນເຕັມ fd;
  ຈຳນວນເຕັມ i;
  ga_nanaa { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  ສຳລັບ (i = 0; i < N; i++) {
    ຖ້າ (write(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    ຖ້າ (write(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = read(fd, buf, N * SZ * 2);
  ຖ້າ (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

void
writebig(var_na *s)
{
  ຈຳນວນເຕັມ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  ສຳລັບ (i = 0; i < MAXFILE; i++) {
    ((ຈຳນວນເຕັມ *)buf)[0] = i;
    ຖ້າ (write(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  ສຳລັບ (;;) {
    i = read(fd, buf, BSIZE);
    ຖ້າ (i == 0) {
      ຖ້າ (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      bha_nga;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    ຖ້າ (((ຈຳນວນເຕັມ *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((ຈຳນວນເຕັມ *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  ຖ້າ (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
void
createtest(var_na *s)
{
  ຈຳນວນເຕັມ i, fd;
  ga_nanaa { N = 52 };

  var_na name[3];
  name[0] = 'a';
  name[2] = '\0';
  ສຳລັບ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ສຳລັບ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

void
dirtest(var_na *s)
{
  ຖ້າ (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  ຖ້າ (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  ຖ້າ (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  ຖ້າ (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

void
exectest(var_na *s)
{
  ຈຳນວນເຕັມ fd, xstatus, pid;
  var_na *echoargv[] = {"echo", "OK", 0};
  var_na buf[3];

  unlink("echo-ok");
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ຈຳນວນເຕັມ errfd = dup(1);
    ຖ້າ (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ຖ້າ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    ຖ້າ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    ຖ້າ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  ຖ້າ (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  ຖ້າ (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ຖ້າ (read(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  ຖ້າ (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  ບໍ່ດັ່ງນັ້ນ {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

void
pipe1(var_na *s)
{
  ຈຳນວນເຕັມ fds[2], pid, xstatus;
  ຈຳນວນເຕັມ seq, i, n, cc, total;
  ga_nanaa { N = 5, SZ = 1033 };

  ຖ້າ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  ຖ້າ (pid == 0) {
    close(fds[0]);
    ສຳລັບ (n = 0; n < N; n++) {
      ສຳລັບ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ຖ້າ (write(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    ໃນຂະນະທີ່ ((n = read(fds[0], buf, cc)) > 0) {
      ສຳລັບ (i = 0; i < n; i++) {
        ຖ້າ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          ສົ່ງຄືນ;
        }
      }
      total += n;
      cc = cc * 2;
      ຖ້າ (cc > parimaa_na(buf))
        cc = parimaa_na(buf);
    }
    ຖ້າ (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } ບໍ່ດັ່ງນັ້ນ {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
void
killstatus(var_na *s)
{
  ຈຳນວນເຕັມ xst;

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 100; i++) {
    ຈຳນວນເຕັມ pid1 = fork();
    ຖ້າ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid1 == 0) {
      ໃນຂະນະທີ່ (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ຖ້າ (xst != -1) {
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
  ຈຳນວນເຕັມ pid, xst;

  kill(0);

  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    exit(7);
  }
  ຖ້າ (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  ຖ້າ (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
void
preempt(var_na *s)
{
  ຈຳນວນເຕັມ pid1, pid2, pid3;
  ຈຳນວນເຕັມ pfds[2];

  pid1 = fork();
  ຖ້າ (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ຖ້າ (pid1 == 0)
    ສຳລັບ (;;)
      ;

  pid2 = fork();
  ຖ້າ (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid2 == 0)
    ສຳລັບ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ຖ້າ (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid3 == 0) {
    close(pfds[0]);
    ຖ້າ (write(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    ສຳລັບ (;;)
      ;
  }

  close(pfds[1]);
  ຖ້າ (read(pfds[0], buf, parimaa_na(buf)) != 1) {
    printf("%s: preempt read error", s);
    ສົ່ງຄືນ;
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
  ຈຳນວນເຕັມ i, pid;

  ສຳລັບ (i = 0; i < 100; i++) {
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid) {
      ຈຳນວນເຕັມ xstate;
      ຖ້າ (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      ຖ້າ (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } ບໍ່ດັ່ງນັ້ນ {
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
  ຈຳນວນເຕັມ master_pid = getpid();
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 200; i++) {
    ຈຳນວນເຕັມ pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid) {
      ຖ້າ (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } ບໍ່ດັ່ງນັ້ນ {
      ຈຳນວນເຕັມ pid2 = fork();
      ຖ້າ (pid2 < 0) {
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
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 1000; i++) {
    ຈຳນວນເຕັມ pid1 = fork();
    ຖ້າ (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid1 == 0) {
      exit(0);
    } ບໍ່ດັ່ງນັ້ນ {
      ຈຳນວນເຕັມ pid2 = fork();
      ຖ້າ (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      ຖ້າ (pid2 == 0) {
        exit(0);
      } ບໍ່ດັ່ງນັ້ນ {
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

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < N; i++) {
    ຈຳນວນເຕັມ pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    ຖ້າ (pid == 0) {
      ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < 200; j++) {
        ຈຳນວນເຕັມ pid1 = fork();
        ຖ້າ (pid1 < 0) {
          exit(1);
        }
        ຖ້າ (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  ຈຳນວນເຕັມ xstatus;
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < N; i++) {
    wait(&xstatus);
    ຖ້າ (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

void
forkforkfork(var_na *s)
{
  unlink("stopforking");

  ຈຳນວນເຕັມ pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ໃນຂະນະທີ່ (1) {
      ຈຳນວນເຕັມ fd = open("stopforking", 0);
      ຖ້າ (fd >= 0) {
        exit(0);
      }
      ຖ້າ (fork() < 0) {
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
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 800; i++) {
    ຈຳນວນເຕັມ pid1 = fork();
    ຖ້າ (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    ຖ້າ (pid1 == 0) {
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
  ຈຳນວນເຕັມ pid;

  ຖ້າ ((pid = fork()) == 0) {
    m1 = 0;
    ໃນຂະນະທີ່ ((m2 = malloc(10001)) != 0) {
      *(var_na **)m2 = m1;
      m1 = m2;
    }
    ໃນຂະນະທີ່ (m1) {
      m2 = *(var_na **)m1;
      free(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    ຖ້າ (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    free(m1);
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ {
    ຈຳນວນເຕັມ xstatus;
    wait(&xstatus);
    ຖ້າ (xstatus == -1) {
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
  ຈຳນວນເຕັມ fd, pid, i, n, nc, np;
  ga_nanaa { N = 1000, SZ = 10 };
  var_na buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', parimaa_na(buf));
  ສຳລັບ (i = 0; i < N; i++) {
    ຖ້າ (write(fd, buf, parimaa_na(buf)) != parimaa_na(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  ຖ້າ (pid == 0) {
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ {
    ຈຳນວນເຕັມ xstatus;
    wait(&xstatus);
    ຖ້າ (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ຖ້າ (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  ໃນຂະນະທີ່ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ສຳລັບ (i = 0; i < parimaa_na(buf); i++) {
      ຖ້າ (buf[i] == 'c')
        nc++;
      ຖ້າ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ຖ້າ (nc == N * SZ && np == N * SZ) {
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
void
fourfiles(var_na *s)
{
  ຈຳນວນເຕັມ fd, pid, i, j, n, total, pi;
  var_na *names[] = {"f0", "f1", "f2", "f3"};
  var_na *fname;
  ga_nanaa { N = 12, NCHILD = 4, SZ = 500 };

  ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ຖ້າ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ຖ້າ (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      ສຳລັບ (i = 0; i < N; i++) {
        ຖ້າ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  ຈຳນວນເຕັມ xstatus;
  ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ຖ້າ (xstatus != 0)
      exit(xstatus);
  }

  ສຳລັບ (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    ໃນຂະນະທີ່ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
      ສຳລັບ (j = 0; j < n; j++) {
        ຖ້າ (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    ຖ້າ (total != N * SZ) {
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
  ຈຳນວນເຕັມ pid, i, fd, pi;
  var_na name[32];

  ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    ຖ້າ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ສຳລັບ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ຖ້າ (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        ຖ້າ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ຖ້າ (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  ຈຳນວນເຕັມ xstatus;
  ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ຖ້າ (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  ສຳລັບ (i = 0; i < N; i++) {
    ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ຖ້າ ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      ຖ້າ (fd >= 0)
        close(fd);
    }
  }

  ສຳລັບ (i = 0; i < N; i++) {
    ສຳລັບ (pi = 0; pi < NCHILD; pi++) {
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
  ຈຳນວນເຕັມ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  write(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  ຖ້າ (read(fd, buf, parimaa_na(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  ຖ້າ (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  ຖ້າ (write(fd, buf, 10) != 10) {
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
  ຈຳນວນເຕັມ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  ຖ້າ (write(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  ຖ້າ (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  ຖ້າ (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  ຖ້າ (read(fd, buf, parimaa_na(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  ຖ້າ (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  ຖ້າ (link(".", "lf1") >= 0) {
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
  ຈຳນວນເຕັມ i, pid, n, fd;
  var_na fa[N];
  sa_nracanaa {
    ushort inum;
    var_na name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ສຳລັບ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ຖ້າ (pid && (i % 3) == 1) {
      link("C0", file);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ບໍ່ດັ່ງນັ້ນ {
      fd = open(file, O_CREATE | O_RDWR);
      ຖ້າ (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    ຖ້າ (pid == 0) {
      exit(0);
    } ບໍ່ດັ່ງນັ້ນ {
      ຈຳນວນເຕັມ xstatus;
      wait(&xstatus);
      ຖ້າ (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, parimaa_na(fa));
  fd = open(".", 0);
  n = 0;
  ໃນຂະນະທີ່ (read(fd, &de, parimaa_na(de)) > 0) {
    ຖ້າ (de.inum == 0)
      continue;
    ຖ້າ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ຖ້າ (i < 0 || i >= parimaa_na(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      ຖ້າ (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ຖ້າ (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  ສຳລັບ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ບໍ່ດັ່ງນັ້ນ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ຖ້າ (pid == 0)
      exit(0);
    ບໍ່ດັ່ງນັ້ນ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink(var_na *s)
{
  ຈຳນວນເຕັມ pid, i;

  unlink("x");
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned ຈຳນວນເຕັມ x = (pid ? 1 : 97);
  ສຳລັບ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ຖ້າ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((x % 3) == 1) {
      link("cat", "x");
    } ບໍ່ດັ່ງນັ້ນ {
      unlink("x");
    }
  }

  ຖ້າ (pid)
    wait(0);
  ບໍ່ດັ່ງນັ້ນ
    exit(0);
}

void
subdir(var_na *s)
{
  ຈຳນວນເຕັມ fd, cc;

  unlink("ff");
  ຖ້າ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "ff", 2);
  close(fd);

  ຖ້າ (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  ຖ້າ (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ຖ້າ (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = read(fd, buf, parimaa_na(buf));
  ຖ້າ (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  ຖ້າ (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ຖ້າ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  ຖ້າ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  ຖ້າ (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  ຖ້າ (read(fd, buf, parimaa_na(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  ຖ້າ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  ຖ້າ (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
void
bigwrite(var_na *s)
{
  ຈຳນວນເຕັມ fd, sz;

  unlink("bigwrite");
  ສຳລັບ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ຖ້າ (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    ຈຳນວນເຕັມ i;
    ສຳລັບ (i = 0; i < 2; i++) {
      ຈຳນວນເຕັມ cc = write(fd, buf, sz);
      ຖ້າ (cc != sz) {
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
  ຈຳນວນເຕັມ fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  ສຳລັບ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ຖ້າ (write(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ຖ້າ (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  ສຳລັບ (i = 0;; i++) {
    cc = read(fd, buf, SZ / 2);
    ຖ້າ (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    ຖ້າ (cc == 0)
      bha_nga;
    ຖ້າ (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    ຖ້າ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  ຖ້າ (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

void
fourteen(var_na *s)
{
  ຈຳນວນເຕັມ fd;

  // DIRSIZ is 14.

  ຖ້າ (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ຖ້າ (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ຖ້າ (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  ຖ້າ (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("123456789012345/12345678901234") == 0) {
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
  ຖ້າ (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  ຖ້າ (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

void
dirfile(var_na *s)
{
  ຈຳນວນເຕັມ fd;

  fd = open("dirfile", O_CREATE);
  ຖ້າ (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  ຖ້າ (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  ຖ້າ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ຖ້າ (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  ຖ້າ (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  ຖ້າ (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  ຖ້າ (write(fd, "x", 1) > 0) {
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
  ຈຳນວນເຕັມ i, fd;

  ສຳລັບ (i = 0; i < NINODE + 1; i++) {
    ຖ້າ (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    ຖ້າ (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ຖ້າ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ຖ້າ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ສຳລັບ (i = 0; i < NINODE + 1; i++) {
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
  ຈຳນວນເຕັມ n, pid;

  ສຳລັບ (n = 0; n < N; n++) {
    pid = fork();
    ຖ້າ (pid < 0)
      bha_nga;
    ຖ້າ (pid == 0)
      exit(0);
  }

  ຖ້າ (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  ຖ້າ (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  ສຳລັບ (; n > 0; n--) {
    ຖ້າ (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  ຖ້າ (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

void
sbrkbasic(var_na *s)
{
  ga_nanaa { TOOMUCH = 1024 * 1024 * 1024 };
  ຈຳນວນເຕັມ i, pid, xstatus;
  var_na *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
    a = sbrk(TOOMUCH);
    ຖ້າ (a == (var_na *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    ສຳລັບ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  ຖ້າ (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ສຳລັບ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ຖ້າ (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ຖ້າ (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0)
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
  ຖ້າ (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (var_na *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ຖ້າ (c == (var_na *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  ຖ້າ (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ຖ້າ (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  ຖ້າ (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ຖ້າ (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
void
kernmem(var_na *s)
{
  var_na *a;
  ຈຳນວນເຕັມ pid;

  ສຳລັບ (a = (var_na *)(KERNBASE); a < (var_na *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    ຈຳນວນເຕັມ xstatus;
    wait(&xstatus);
    ຖ້າ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
void
MAXVAplus(var_na *s)
{
  ca_ncala uint64 a = MAXVA;
  ສຳລັບ (; a != 0; a <<= 1) {
    ຈຳນວນເຕັມ pid;
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    ຖ້າ (pid == 0) {
      *(var_na *)a = 99;
      printf("%s: oops wrote %p\n", s, (void *)a);
      exit(1);
    }
    ຈຳນວນເຕັມ xstatus;
    wait(&xstatus);
    ຖ້າ (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
void
sbrkfail(var_na *s)
{
  ga_nanaa { BIG = 100 * 1024 * 1024 };
  ຈຳນວນເຕັມ i, xstatus;
  ຈຳນວນເຕັມ fds[2];
  var_na scratch;
  var_na *c, *a;
  ຈຳນວນເຕັມ pids[10];
  ຈຳນວນເຕັມ pid;
  ຈຳນວນເຕັມ failed;

  failed = 0;
  ຖ້າ (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  ສຳລັບ (i = 0; i < parimaa_na(pids) / parimaa_na(pids[0]); i++) {
    ຖ້າ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ຖ້າ (sbrk(BIG - (uint64)sbrk(0)) == (var_na *)SBRK_ERROR)
        write(fds[1], "0", 1);
      ບໍ່ດັ່ງນັ້ນ
        write(fds[1], "1", 1);
      // sit around until killed
      ສຳລັບ (;;)
        pause(1000);
    }
    ຖ້າ (pids[i] != -1) {
      read(fds[0], &scratch, 1);
      ຖ້າ (scratch == '0')
        failed = 1;
    }
  }
  ຖ້າ (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ສຳລັບ (i = 0; i < parimaa_na(pids) / parimaa_na(pids[0]); i++) {
    ຖ້າ (pids[i] == -1)
      continue;
    kill(pids[i]);
    wait(0);
  }
  ຖ້າ (c == (var_na *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  ຖ້າ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ຖ້າ (a == (var_na *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  ຖ້າ (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
void
sbrkarg(var_na *s)
{
  var_na *a;
  ຈຳນວນເຕັມ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ຖ້າ (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  ຖ້າ ((n = write(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ຖ້າ (pipe((ຈຳນວນເຕັມ *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

void
validatetest(var_na *s)
{
  ຈຳນວນເຕັມ hi;
  uint64 p;

  hi = 1100 * 1024;
  ສຳລັບ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ຖ້າ (link("nosuchfile", (var_na *)p) != -1) {
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
  ຈຳນວນເຕັມ i;

  ສຳລັບ (i = 0; i < parimaa_na(uninit); i++) {
    ຖ້າ (uninit[i] != '\0') {
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
  ຈຳນວນເຕັມ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ຖ້າ (pid == 0) {
    static var_na *args[MAXARG];
    ຈຳນວນເຕັມ i;
    var_na big[400];
    memset(big, ' ', parimaa_na(big));
    big[parimaa_na(big) - 1] = '\0';
    ສຳລັບ (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  ຖ້າ (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  ຖ້າ (fd < 0) {
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
  ຈຳນວນເຕັມ nfiles;
  ຈຳນວນເຕັມ fsblocks = 0;

  printf("fsfull test\n");

  ສຳລັບ (nfiles = 0;; nfiles++) {
    var_na name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    ຈຳນວນເຕັມ fd = open(name, O_CREATE | O_RDWR);
    ຖ້າ (fd < 0) {
      printf("open %s failed\n", name);
      bha_nga;
    }
    ຈຳນວນເຕັມ total = 0;
    ໃນຂະນະທີ່ (1) {
      ຈຳນວນເຕັມ cc = write(fd, buf, BSIZE);
      ຖ້າ (cc < BSIZE)
        bha_nga;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    ຖ້າ (total == 0)
      bha_nga;
  }

  ໃນຂະນະທີ່ (nfiles >= 0) {
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
  ຈຳນວນເຕັມ fd;
  fd = open("init", O_RDONLY);
  ຖ້າ (fd < 0) {
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
  ຈຳນວນເຕັມ pid;
  ຈຳນວນເຕັມ xstatus;

  pid = fork();
  ຖ້າ (pid == 0) {
    var_na *sp = (var_na *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  ຖ້າ (xstatus == -1) // kernel killed child?
    exit(0);
  ບໍ່ດັ່ງນັ້ນ
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
void
nowrite(var_na *s)
{
  ຈຳນວນເຕັມ pid;
  ຈຳນວນເຕັມ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ສຳລັບ (ຈຳນວນເຕັມ ai = 0; ai < parimaa_na(addrs) / parimaa_na(addrs[0]); ai++) {
    pid = fork();
    ຖ້າ (pid == 0) {
      ca_ncala ຈຳນວນເຕັມ *addr = (ຈຳນວນເຕັມ *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    ຖ້າ (xstatus == 0) {
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
  ຈຳນວນເຕັມ pid = fork();
  ຖ້າ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ຈຳນວນເຕັມ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  ຖ້າ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
    ຈຳນວນເຕັມ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  ຖ້າ (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
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
  ຖ້າ ((top % PGSIZE) != 0)
    sbrk(PGSIZE - (top % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  top = (uint64)sbrk(0);
  var_na *p = (var_na *)(top - 64);
  p[0] = 'x';
  p[1] = '\0';
  ຈຳນວນເຕັມ fd = open(p, O_RDWR | O_CREATE);
  write(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  read(fd, p, 1);
  ຖ້າ (p[0] != 'x')
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
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 50000; i++) {
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
  ຖ້າ (prev_end == (var_na *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ສຳລັບ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(var_na **)i = i;

  ສຳລັບ (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    ຖ້າ (*(var_na **)i != i) {
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
  ຈຳນວນເຕັມ pid;
  var_na *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  ຖ້າ (prev_end == (var_na *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  ສຳລັບ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(var_na **)i = i;

  ສຳລັບ (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("error forking\n");
      exit(1);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(var_na **)i = i;
      exit(0);
    } ບໍ່ດັ່ງນັ້ນ {
      ຈຳນວນເຕັມ status;
      wait(&status);
      ຖ້າ (status == 0) {
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
    ຖ້າ (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < parimaa_na(bad) / parimaa_na(bad[0]); i++) {
    ຈຳນວນເຕັມ fd = open("README", 0);
    ຖ້າ (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    ຖ້າ (read(fd, (var_na *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ຖ້າ (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    ຖ້າ (write(fd, (var_na *)bad[i], 512) >= 0) {
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
  ຖ້າ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ຈຳນວນເຕັມ fd = open(&p[4095], O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  sa_nracanaa stat st;
  ຈຳນວນເຕັມ r = fstat(fd, &st);
  ຖ້າ (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  ຖ້າ (st.type != T_DIR) {
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
  ໃນຂະນະທີ່ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ຖ້າ (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  ຈຳນວນເຕັມ n = TRAPFRAME - PGSIZE - (uint64)p;

  var_na *p1 = sbrklazy(n);
  ຖ້າ (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  ຖ້າ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  ຖ້າ (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  ຖ້າ ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  ຖ້າ ((uint64)p != -1) {
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
  ຈຳນວນເຕັມ fd = open("testfile", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  ຈຳນວນເຕັມ cc = write(fd, "A", 1);
  ຖ້າ (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  var_na *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ຖ້າ ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = write(fd, p - 1, 2);
  ຖ້າ (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  var_na b;
  cc = read(fd, &b, 1);
  ຖ້າ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ຖ້າ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 64; i++) {
    var_na buf[1024];
    memset(buf, 0, parimaa_na(buf));
    cc = write(fd, buf, parimaa_na(buf));
    ຖ້າ (cc != parimaa_na(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ຖ້າ (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = read(fd, &b, 1);
  ຖ້າ (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  ຖ້າ (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

void
unlinkcwd(var_na *s)
{
  ຖ້າ (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  ຖ້າ (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  ຖ້າ (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  ຖ້າ (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  ຖ້າ (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  ຖ້າ (open("../c", O_CREATE) > 0) {
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
  ຈຳນວນເຕັມ i, fd;
  var_na name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ຖ້າ (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  ສຳລັບ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ຖ້າ (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  ສຳລັບ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ຖ້າ (unlink(name) != 0) {
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
  ຈຳນວນເຕັມ nchildren = 4;
  ຈຳນວນເຕັມ howmany = 30; // increase to look for deadlock

  ສຳລັບ (ຈຳນວນເຕັມ ci = 0; ci < nchildren; ci++) {
    ຈຳນວນເຕັມ pid = fork();
    ຖ້າ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    ຖ້າ (pid == 0) {
      var_na name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ສຳລັບ (ຈຳນວນເຕັມ iters = 0; iters < howmany; iters++) {
        ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < ci + 1; i++) {
          ຈຳນວນເຕັມ fd = open(name, O_CREATE | O_RDWR);
          ຖ້າ (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          ຈຳນວນເຕັມ sz = parimaa_na(buf);
          ຈຳນວນເຕັມ cc = write(fd, buf, sz);
          ຖ້າ (cc != sz) {
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

  ສຳລັບ (ຈຳນວນເຕັມ ci = 0; ci < nchildren; ci++) {
    ຈຳນວນເຕັມ st = 0;
    wait(&st);
    ຖ້າ (st != 0)
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
  ຈຳນວນເຕັມ assumed_free = 600;

  unlink("junk");
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < assumed_free; i++) {
    ຈຳນວນເຕັມ fd = open("junk", O_CREATE | O_WRONLY);
    ຖ້າ (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    write(fd, (var_na *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ຈຳນວນເຕັມ fd = open("junk", O_CREATE | O_WRONLY);
  ຖ້າ (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  ຖ້າ (write(fd, "x", 1) != 1) {
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
  ສຳລັບ (ຈຳນວນເຕັມ avail = 0; avail < 15; avail++) {
    ຈຳນວນເຕັມ pid = fork();
    ຖ້າ (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pid == 0) {
      // allocate all of memory.
      ໃນຂະນະທີ່ (1) {
        var_na *a = sbrk(PGSIZE);
        ຖ້າ (a == SBRK_ERROR)
          bha_nga;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      var_na *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } ບໍ່ດັ່ງນັ້ນ {
      wait((ຈຳນວນເຕັມ *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
void
diskfull(var_na *s)
{
  ຈຳນວນເຕັມ fi;
  ຈຳນວນເຕັມ done = 0;

  unlink("diskfulldir");

  ສຳລັບ (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    var_na name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ຈຳນວນເຕັມ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ຖ້າ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      bha_nga;
    }
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < MAXFILE; i++) {
      var_na buf[BSIZE];
      ຖ້າ (write(fd, buf, BSIZE) != BSIZE) {
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
  ຈຳນວນເຕັມ nzz = 128;
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ຈຳນວນເຕັມ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ຖ້າ (fd < 0)
      bha_nga;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ຖ້າ (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; '0' + i < 0177; i++) {
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
  ຈຳນວນເຕັມ nzz = 32 * 32;
  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < nzz; i++) {
    var_na name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ຈຳນວນເຕັມ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ຖ້າ (fd < 0) {
      // failure is eventually expected.
      bha_nga;
    }
    close(fd);
  }

  ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < nzz; i++) {
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
  ຈຳນວນເຕັມ i;

  unlink("/lof");
  ຈຳນວນເຕັມ fd = open("/lof", O_CREATE | O_RDWR);
  ຖ້າ (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  ສຳລັບ (i = 0; i < TARGET; i++) {
    ຈຳນວນເຕັມ d = i % DIRS;
    ຈຳນວນເຕັມ f = i / DIRS;

    var_na pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ຖ້າ (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ຖ້າ (link("/lof", pn) < 0) {
      ຖ້າ (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      ຖ້າ (st.nlink >= 32767) {
        // overflow check succeeded.
        bha_nga;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    ຖ້າ (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ຖ້າ (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  ຖ້າ (st.nlink < 0) {
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
ຈຳນວນເຕັມ
run(void f(var_na *), var_na *s)
{
  ຈຳນວນເຕັມ pid;
  ຈຳນວນເຕັມ xstatus;

  printf("test %s: ", s);
  ຖ້າ ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  ຖ້າ (pid == 0) {
    f(s);
    exit(0);
  } ບໍ່ດັ່ງນັ້ນ {
    wait(&xstatus);
    ຖ້າ (xstatus != 0)
      printf("FAILED\n");
    ບໍ່ດັ່ງນັ້ນ
      printf("OK\n");
    ສົ່ງຄືນ xstatus == 0;
  }
}

ຈຳນວນເຕັມ
runtests(sa_nracanaa test *tests, var_na *justone, ຈຳນວນເຕັມ continuous)
{
  ຈຳນວນເຕັມ ntests = 0;
  ສຳລັບ (sa_nracanaa test *t = tests; t->s != 0; t++) {
    ຖ້າ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ຖ້າ (!run(t->f, t->s)) {
        ຖ້າ (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          ສົ່ງຄືນ -1;
        }
      }
    }
  }
  ສົ່ງຄືນ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ຈຳນວນເຕັມ
countfree()
{
  ຈຳນວນເຕັມ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ໃນຂະນະທີ່ (1) {
    var_na *a = sbrk(PGSIZE);
    ຖ້າ (a == SBRK_ERROR) {
      bha_nga;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ
drivetests(ຈຳນວນເຕັມ quick, ຈຳນວນເຕັມ continuous, var_na *justone)
{
  do {
    printf("usertests starting\n");
    ຈຳນວນເຕັມ free0 = countfree();
    ຈຳນວນເຕັມ free1 = 0;
    ຈຳນວນເຕັມ ntests = 0;
    ຈຳນວນເຕັມ n;
    n = runtests(quicktests, justone, continuous);
    ຖ້າ (n < 0) {
      ຖ້າ (continuous != 2) {
        ສົ່ງຄືນ 1;
      }
    } ບໍ່ດັ່ງນັ້ນ {
      ntests += n;
    }
    ຖ້າ (!quick) {
      ຖ້າ (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ຖ້າ (n < 0) {
        ຖ້າ (continuous != 2) {
          ສົ່ງຄືນ 1;
        }
      } ບໍ່ດັ່ງນັ້ນ {
        ntests += n;
      }
    }
    ຖ້າ ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ຖ້າ (continuous != 2) {
        ສົ່ງຄືນ 1;
      }
    }
    ຖ້າ (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      ສົ່ງຄືນ 1;
    }
  } ໃນຂະນະທີ່ (continuous);
  ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ continuous = 0;
  ຈຳນວນເຕັມ quick = 0;
  var_na *justone = 0;

  ຖ້າ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  ຖ້າ (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
