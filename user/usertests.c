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

char buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
void
copyin(char *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  для (ціле ai = 0; ai < sizeof(addrs) / sizeof(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ціле fd = open("copyin1", O_CREATE | O_WRONLY);
    якщо (fd < 0) {
      printf("open(copyin1) failed\n");
      exit(1);
    }
    ціле n = write(fd, (void *)addr, 8192);
    якщо (n >= 0) {
      printf("write(fd, %p, 8192) returned %d, not -1\n", (void *)addr, n);
      exit(1);
    }
    close(fd);
    unlink("copyin1");

    n = write(1, (char *)addr, 8192);
    якщо (n > 0) {
      printf("write(1, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }

    ціле fds[2];
    якщо (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], (char *)addr, 8192);
    якщо (n > 0) {
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
copyout(char *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  для (ціле ai = 0; ai < sizeof(addrs) / sizeof(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ціле fd = open("README", 0);
    якщо (fd < 0) {
      printf("open(README) failed\n");
      exit(1);
    }
    ціле n = read(fd, (void *)addr, 8192);
    якщо (n > 0) {
      printf("read(fd, %p, 8192) returned %d, not -1 or 0\n", (void *)addr, n);
      exit(1);
    }
    close(fd);

    ціле fds[2];
    якщо (pipe(fds) < 0) {
      printf("pipe() failed\n");
      exit(1);
    }
    n = write(fds[1], "x", 1);
    якщо (n != 1) {
      printf("pipe write failed\n");
      exit(1);
    }
    n = read(fds[0], (void *)addr, 8192);
    якщо (n > 0) {
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
copyinstr1(char *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  для (ціле ai = 0; ai < sizeof(addrs) / sizeof(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ціле fd = open((char *)addr, O_CREATE | O_WRONLY);
    якщо (fd >= 0) {
      printf("open(%p) returned %d, not -1\n", (void *)addr, fd);
      exit(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
void
copyinstr2(char *s)
{
  char b[MAXPATH + 1];

  для (ціле i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ціле ret = unlink(b);
  якщо (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ціле fd = open(b, O_CREATE | O_WRONLY);
  якщо (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  якщо (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  char *args[] = {"xx", 0};
  ret = exec(b, args);
  якщо (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ціле pid = fork();
  якщо (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  якщо (pid == 0) {
    static char big[PGSIZE + 1];
    для (ціле i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    char *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    якщо (ret != -1) {
      printf("exec(echo, BIG) returned %d, not -1\n", fd);
      exit(1);
    }
    exit(747); // OK
  }

  ціле st = 0;
  wait(&st);
  якщо (st != 747) {
    printf("exec(echo, BIG) succeeded, should have failed\n");
    exit(1);
  }
}

// what if a string argument crosses over the end of last user page?
void
copyinstr3(char *s)
{
  sbrk(8192);
  uint64 top = (uint64)sbrk(0);
  якщо ((top % PGSIZE) != 0) {
    sbrk(PGSIZE - (top % PGSIZE));
  }
  top = (uint64)sbrk(0);
  якщо (top % PGSIZE) {
    printf("oops\n");
    exit(1);
  }

  char *b = (char *)(top - 1);
  *b = 'x';

  ціле ret = unlink(b);
  якщо (ret != -1) {
    printf("unlink(%s) returned %d, not -1\n", b, ret);
    exit(1);
  }

  ціле fd = open(b, O_CREATE | O_WRONLY);
  якщо (fd != -1) {
    printf("open(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }

  ret = link(b, b);
  якщо (ret != -1) {
    printf("link(%s, %s) returned %d, not -1\n", b, b, ret);
    exit(1);
  }

  char *args[] = {"xx", 0};
  ret = exec(b, args);
  якщо (ret != -1) {
    printf("exec(%s) returned %d, not -1\n", b, fd);
    exit(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
void
rwsbrk(char *s)
{
  ціле fd, n;

  uint64 a = (uint64)sbrk(8192);

  якщо (a == (uint64)SBRK_ERROR) {
    printf("sbrk(rwsbrk) failed\n");
    exit(1);
  }

  якщо (sbrk(-8192) == SBRK_ERROR) {
    printf("sbrk(rwsbrk) shrink failed\n");
    exit(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  якщо (fd < 0) {
    printf("open(rwsbrk) failed\n");
    exit(1);
  }
  n = write(fd, (void *)(a + PGSIZE), 1024);
  якщо (n >= 0) {
    printf("write(fd, %p, 1024) returned %d, not -1\n", (void *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  якщо (fd < 0) {
    printf("open(README) failed\n");
    exit(1);
  }
  n = read(fd, (void *)(a + PGSIZE), 10);
  якщо (n >= 0) {
    printf("read(fd, %p, 10) returned %d, not -1\n", (void *)a + PGSIZE, n);
    exit(1);
  }
  close(fd);

  exit(0);
}

// test O_TRUNC.
void
truncate1(char *s)
{
  char buf[32];

  unlink("truncfile");
  ціле fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  write(fd1, "abcd", 4);
  close(fd1);

  ціле fd2 = open("truncfile", O_RDONLY);
  ціле n = read(fd2, buf, sizeof(buf));
  якщо (n != 4) {
    printf("%s: read %d bytes, wanted 4\n", s, n);
    exit(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ціле fd3 = open("truncfile", O_RDONLY);
  n = read(fd3, buf, sizeof(buf));
  якщо (n != 0) {
    printf("aaa fd3=%d\n", fd3);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, sizeof(buf));
  якщо (n != 0) {
    printf("bbb fd2=%d\n", fd2);
    printf("%s: read %d bytes, wanted 0\n", s, n);
    exit(1);
  }

  write(fd1, "abcdef", 6);

  n = read(fd3, buf, sizeof(buf));
  якщо (n != 6) {
    printf("%s: read %d bytes, wanted 6\n", s, n);
    exit(1);
  }

  n = read(fd2, buf, sizeof(buf));
  якщо (n != 2) {
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
truncate2(char *s)
{
  unlink("truncfile");

  ціле fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  write(fd1, "abcd", 4);

  ціле fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ціле n = write(fd1, "x", 1);
  якщо (n != -1) {
    printf("%s: write returned %d, expected -1\n", s, n);
    exit(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

void
truncate3(char *s)
{
  ціле pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  якщо (pid == 0) {
    для (ціле i = 0; i < 100; i++) {
      char buf[32];
      ціле fd = open("truncfile", O_WRONLY);
      якщо (fd < 0) {
        printf("%s: open failed\n", s);
        exit(1);
      }
      ціле n = write(fd, "1234567890", 10);
      якщо (n != 10) {
        printf("%s: write got %d, expected 10\n", s, n);
        exit(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      read(fd, buf, sizeof(buf));
      close(fd);
    }
    exit(0);
  }

  для (ціле i = 0; i < 150; i++) {
    ціле fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    якщо (fd < 0) {
      printf("%s: open failed\n", s);
      exit(1);
    }
    ціле n = write(fd, "xxx", 3);
    якщо (n != 3) {
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
iputtest(char *s)
{
  якщо (mkdir("iputdir") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }
  якщо (chdir("iputdir") < 0) {
    printf("%s: chdir iputdir failed\n", s);
    exit(1);
  }
  якщо (unlink("../iputdir") < 0) {
    printf("%s: unlink ../iputdir failed\n", s);
    exit(1);
  }
  якщо (chdir("/") < 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
void
exitiputtest(char *s)
{
  ціле pid, xstatus;

  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid == 0) {
    якщо (mkdir("iputdir") < 0) {
      printf("%s: mkdir failed\n", s);
      exit(1);
    }
    якщо (chdir("iputdir") < 0) {
      printf("%s: child chdir failed\n", s);
      exit(1);
    }
    якщо (unlink("../iputdir") < 0) {
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
openiputtest(char *s)
{
  ціле pid, xstatus;

  якщо (mkdir("oidir") < 0) {
    printf("%s: mkdir oidir failed\n", s);
    exit(1);
  }
  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid == 0) {
    ціле fd = open("oidir", O_RDWR);
    якщо (fd >= 0) {
      printf("%s: open directory for write succeeded\n", s);
      exit(1);
    }
    exit(0);
  }
  pause(1);
  якщо (unlink("oidir") != 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  exit(xstatus);
}

// simple file system tests

void
opentest(char *s)
{
  ціле fd;

  fd = open("echo", 0);
  якщо (fd < 0) {
    printf("%s: open echo failed!\n", s);
    exit(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  якщо (fd >= 0) {
    printf("%s: open doesnotexist succeeded!\n", s);
    exit(1);
  }
}

void
writetest(char *s)
{
  ціле fd;
  ціле i;
  enum { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: error: creat small failed!\n", s);
    exit(1);
  }
  для (i = 0; i < N; i++) {
    якщо (write(fd, "aaaaaaaaaa", SZ) != SZ) {
      printf("%s: error: write aa %d new file failed\n", s, i);
      exit(1);
    }
    якщо (write(fd, "bbbbbbbbbb", SZ) != SZ) {
      printf("%s: error: write bb %d new file failed\n", s, i);
      exit(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: error: open small failed!\n", s);
    exit(1);
  }
  i = read(fd, buf, N * SZ * 2);
  якщо (i != N * SZ * 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  close(fd);

  якщо (unlink("small") < 0) {
    printf("%s: unlink small failed\n", s);
    exit(1);
  }
}

void
writebig(char *s)
{
  ціле i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: error: creat big failed!\n", s);
    exit(1);
  }

  для (i = 0; i < MAXFILE; i++) {
    ((ціле *)buf)[0] = i;
    якщо (write(fd, buf, BSIZE) != BSIZE) {
      printf("%s: error: write big file failed i=%d\n", s, i);
      exit(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: error: open big failed!\n", s);
    exit(1);
  }

  n = 0;
  для (;;) {
    i = read(fd, buf, BSIZE);
    якщо (i == 0) {
      якщо (n != MAXFILE) {
        printf("%s: read only %d blocks from big", s, n);
        exit(1);
      }
      break;
    } інакше якщо (i != BSIZE) {
      printf("%s: read failed %d\n", s, i);
      exit(1);
    }
    якщо (((ціле *)buf)[0] != n) {
      printf("%s: read content of block %d is %d\n", s, n, ((ціле *)buf)[0]);
      exit(1);
    }
    n++;
  }
  close(fd);
  якщо (unlink("big") < 0) {
    printf("%s: unlink big failed\n", s);
    exit(1);
  }
}

// many creates, followed by unlink test
void
createtest(char *s)
{
  ціле i, fd;
  enum { N = 52 };

  char name[3];
  name[0] = 'a';
  name[2] = '\0';
  для (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  для (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

void
dirtest(char *s)
{
  якщо (mkdir("dir0") < 0) {
    printf("%s: mkdir failed\n", s);
    exit(1);
  }

  якщо (chdir("dir0") < 0) {
    printf("%s: chdir dir0 failed\n", s);
    exit(1);
  }

  якщо (chdir("..") < 0) {
    printf("%s: chdir .. failed\n", s);
    exit(1);
  }

  якщо (unlink("dir0") < 0) {
    printf("%s: unlink dir0 failed\n", s);
    exit(1);
  }
}

void
exectest(char *s)
{
  ціле fd, xstatus, pid;
  char *echoargv[] = {"echo", "OK", 0};
  char buf[3];

  unlink("echo-ok");
  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid == 0) {
    ціле errfd = dup(1);
    якщо (errfd < 0) {
      printf("%s: dup failed\n", s);
      exit(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    якщо (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      exit(1);
    }
    якщо (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      exit(1);
    }
    якщо (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      exit(1);
    }
    // won't get to here
  }
  якщо (wait(&xstatus) != pid) {
    printf("%s: wait failed!\n", s);
  }
  якщо (xstatus != 0) {
    printf("%s: nonzero wait status %d\n", s, xstatus);
    exit(1);
  }

  fd = open("echo-ok", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  якщо (read(fd, buf, 2) != 2) {
    printf("%s: read failed\n", s);
    exit(1);
  }
  unlink("echo-ok");
  якщо (buf[0] == 'O' && buf[1] == 'K')
    exit(0);
  інакше {
    printf("%s: wrong output\n", s);
    exit(1);
  }
}

// simple fork and pipe read/write

void
pipe1(char *s)
{
  ціле fds[2], pid, xstatus;
  ціле seq, i, n, cc, total;
  enum { N = 5, SZ = 1033 };

  якщо (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  pid = fork();
  seq = 0;
  якщо (pid == 0) {
    close(fds[0]);
    для (n = 0; n < N; n++) {
      для (i = 0; i < SZ; i++)
        buf[i] = seq++;
      якщо (write(fds[1], buf, SZ) != SZ) {
        printf("%s: pipe1 oops 1\n", s);
        exit(1);
      }
    }
    exit(0);
  } інакше якщо (pid > 0) {
    close(fds[1]);
    total = 0;
    cc = 1;
    поки ((n = read(fds[0], buf, cc)) > 0) {
      для (i = 0; i < n; i++) {
        якщо ((buf[i] & 0xff) != (seq++ & 0xff)) {
          printf("%s: pipe1 oops 2\n", s);
          повернути;
        }
      }
      total += n;
      cc = cc * 2;
      якщо (cc > sizeof(buf))
        cc = sizeof(buf);
    }
    якщо (total != N * SZ) {
      printf("%s: pipe1 oops 3 total %d\n", s, total);
      exit(1);
    }
    close(fds[0]);
    wait(&xstatus);
    exit(xstatus);
  } інакше {
    printf("%s: fork() failed\n", s);
    exit(1);
  }
}

// test if child is killed (status = -1)
void
killstatus(char *s)
{
  ціле xst;

  для (ціле i = 0; i < 100; i++) {
    ціле pid1 = fork();
    якщо (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid1 == 0) {
      поки (1) {
        getpid();
      }
      exit(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    якщо (xst != -1) {
      printf("%s: status should be -1\n", s);
      exit(1);
    }
  }
  exit(0);
}

// test that kill on unused process 0 doesn't set the killed flag
void
killzero(char *s)
{
  ціле pid, xst;

  kill(0);

  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid == 0) {
    exit(7);
  }
  якщо (wait(&xst) != pid) {
    printf("%s: wait wrong pid\n", s);
    exit(1);
  }
  якщо (xst != 7) {
    printf("%s: child exited with status %d, expected 7\n", s, xst);
    exit(1);
  }
  exit(0);
}

// meant to be run w/ at most two CPUs
void
preempt(char *s)
{
  ціле pid1, pid2, pid3;
  ціле pfds[2];

  pid1 = fork();
  якщо (pid1 < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  якщо (pid1 == 0)
    для (;;)
      ;

  pid2 = fork();
  якщо (pid2 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid2 == 0)
    для (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  якщо (pid3 < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid3 == 0) {
    close(pfds[0]);
    якщо (write(pfds[1], "x", 1) != 1)
      printf("%s: preempt write error", s);
    close(pfds[1]);
    для (;;)
      ;
  }

  close(pfds[1]);
  якщо (read(pfds[0], buf, sizeof(buf)) != 1) {
    printf("%s: preempt read error", s);
    повернути;
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
exitwait(char *s)
{
  ціле i, pid;

  для (i = 0; i < 100; i++) {
    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid) {
      ціле xstate;
      якщо (wait(&xstate) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
      якщо (i != xstate) {
        printf("%s: wait wrong exit status\n", s);
        exit(1);
      }
    } інакше {
      exit(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
void
reparent(char *s)
{
  ціле master_pid = getpid();
  для (ціле i = 0; i < 200; i++) {
    ціле pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid) {
      якщо (wait(0) != pid) {
        printf("%s: wait wrong pid\n", s);
        exit(1);
      }
    } інакше {
      ціле pid2 = fork();
      якщо (pid2 < 0) {
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
twochildren(char *s)
{
  для (ціле i = 0; i < 1000; i++) {
    ціле pid1 = fork();
    якщо (pid1 < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid1 == 0) {
      exit(0);
    } інакше {
      ціле pid2 = fork();
      якщо (pid2 < 0) {
        printf("%s: fork failed\n", s);
        exit(1);
      }
      якщо (pid2 == 0) {
        exit(0);
      } інакше {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
void
forkfork(char *s)
{
  enum { N = 2 };

  для (ціле i = 0; i < N; i++) {
    ціле pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed", s);
      exit(1);
    }
    якщо (pid == 0) {
      для (ціле j = 0; j < 200; j++) {
        ціле pid1 = fork();
        якщо (pid1 < 0) {
          exit(1);
        }
        якщо (pid1 == 0) {
          exit(0);
        }
        wait(0);
      }
      exit(0);
    }
  }

  ціле xstatus;
  для (ціле i = 0; i < N; i++) {
    wait(&xstatus);
    якщо (xstatus != 0) {
      printf("%s: fork in child failed", s);
      exit(1);
    }
  }
}

void
forkforkfork(char *s)
{
  unlink("stopforking");

  ціле pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed", s);
    exit(1);
  }
  якщо (pid == 0) {
    поки (1) {
      ціле fd = open("stopforking", 0);
      якщо (fd >= 0) {
        exit(0);
      }
      якщо (fork() < 0) {
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
reparent2(char *s)
{
  для (ціле i = 0; i < 800; i++) {
    ціле pid1 = fork();
    якщо (pid1 < 0) {
      printf("fork failed\n");
      exit(1);
    }
    якщо (pid1 == 0) {
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
mem(char *s)
{
  void *m1, *m2;
  ціле pid;

  якщо ((pid = fork()) == 0) {
    m1 = 0;
    поки ((m2 = malloc(10001)) != 0) {
      *(char **)m2 = m1;
      m1 = m2;
    }
    поки (m1) {
      m2 = *(char **)m1;
      free(m1);
      m1 = m2;
    }
    m1 = malloc(1024 * 20);
    якщо (m1 == 0) {
      printf("%s: couldn't allocate mem?!!\n", s);
      exit(1);
    }
    free(m1);
    exit(0);
  } інакше {
    ціле xstatus;
    wait(&xstatus);
    якщо (xstatus == -1) {
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
sharedfd(char *s)
{
  ціле fd, pid, i, n, nc, np;
  enum { N = 1000, SZ = 10 };
  char buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: cannot open sharedfd for writing", s);
    exit(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', sizeof(buf));
  для (i = 0; i < N; i++) {
    якщо (write(fd, buf, sizeof(buf)) != sizeof(buf)) {
      printf("%s: write sharedfd failed\n", s);
      exit(1);
    }
  }
  якщо (pid == 0) {
    exit(0);
  } інакше {
    ціле xstatus;
    wait(&xstatus);
    якщо (xstatus != 0)
      exit(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  якщо (fd < 0) {
    printf("%s: cannot open sharedfd for reading\n", s);
    exit(1);
  }
  nc = np = 0;
  поки ((n = read(fd, buf, sizeof(buf))) > 0) {
    для (i = 0; i < sizeof(buf); i++) {
      якщо (buf[i] == 'c')
        nc++;
      якщо (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  якщо (nc == N * SZ && np == N * SZ) {
    exit(0);
  } інакше {
    printf("%s: nc/np test fails\n", s);
    exit(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
void
fourfiles(char *s)
{
  ціле fd, pid, i, j, n, total, pi;
  char *names[] = {"f0", "f1", "f2", "f3"};
  char *fname;
  enum { N = 12, NCHILD = 4, SZ = 500 };

  для (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    якщо (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      якщо (fd < 0) {
        printf("%s: create failed\n", s);
        exit(1);
      }

      memset(buf, '0' + pi, SZ);
      для (i = 0; i < N; i++) {
        якщо ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }

  ціле xstatus;
  для (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    якщо (xstatus != 0)
      exit(xstatus);
  }

  для (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    поки ((n = read(fd, buf, sizeof(buf))) > 0) {
      для (j = 0; j < n; j++) {
        якщо (buf[j] != '0' + i) {
          printf("%s: wrong char\n", s);
          exit(1);
        }
      }
      total += n;
    }
    close(fd);
    якщо (total != N * SZ) {
      printf("wrong length %d\n", total);
      exit(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
void
createdelete(char *s)
{
  enum { N = 20, NCHILD = 4 };
  ціле pid, i, fd, pi;
  char name[32];

  для (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }

    якщо (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      для (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        якщо (fd < 0) {
          printf("%s: create failed\n", s);
          exit(1);
        }
        close(fd);
        якщо (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          якщо (unlink(name) < 0) {
            printf("%s: unlink failed\n", s);
            exit(1);
          }
        }
      }
      exit(0);
    }
  }

  ціле xstatus;
  для (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    якщо (xstatus != 0)
      exit(1);
  }

  name[0] = name[1] = name[2] = 0;
  для (i = 0; i < N; i++) {
    для (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      якщо ((i == 0 || i >= N / 2) && fd < 0) {
        printf("%s: oops createdelete %s didn't exist\n", s, name);
        exit(1);
      } інакше якщо ((i >= 1 && i < N / 2) && fd >= 0) {
        printf("%s: oops createdelete %s did exist\n", s, name);
        exit(1);
      }
      якщо (fd >= 0)
        close(fd);
    }
  }

  для (i = 0; i < N; i++) {
    для (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
void
unlinkread(char *s)
{
  enum { SZ = 5 };
  ціле fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: create unlinkread failed\n", s);
    exit(1);
  }
  write(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  якщо (fd < 0) {
    printf("%s: open unlinkread failed\n", s);
    exit(1);
  }
  якщо (unlink("unlinkread") != 0) {
    printf("%s: unlink unlinkread failed\n", s);
    exit(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  якщо (read(fd, buf, sizeof(buf)) != SZ) {
    printf("%s: unlinkread read failed", s);
    exit(1);
  }
  якщо (buf[0] != 'h') {
    printf("%s: unlinkread wrong data\n", s);
    exit(1);
  }
  якщо (write(fd, buf, 10) != 10) {
    printf("%s: unlinkread write failed\n", s);
    exit(1);
  }
  close(fd);
  unlink("unlinkread");
}

void
linktest(char *s)
{
  enum { SZ = 5 };
  ціле fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: create lf1 failed\n", s);
    exit(1);
  }
  якщо (write(fd, "hello", SZ) != SZ) {
    printf("%s: write lf1 failed\n", s);
    exit(1);
  }
  close(fd);

  якщо (link("lf1", "lf2") < 0) {
    printf("%s: link lf1 lf2 failed\n", s);
    exit(1);
  }
  unlink("lf1");

  якщо (open("lf1", 0) >= 0) {
    printf("%s: unlinked lf1 but it is still there!\n", s);
    exit(1);
  }

  fd = open("lf2", 0);
  якщо (fd < 0) {
    printf("%s: open lf2 failed\n", s);
    exit(1);
  }
  якщо (read(fd, buf, sizeof(buf)) != SZ) {
    printf("%s: read lf2 failed\n", s);
    exit(1);
  }
  close(fd);

  якщо (link("lf2", "lf2") >= 0) {
    printf("%s: link lf2 lf2 succeeded! oops\n", s);
    exit(1);
  }

  unlink("lf2");
  якщо (link("lf2", "lf1") >= 0) {
    printf("%s: link non-existent succeeded! oops\n", s);
    exit(1);
  }

  якщо (link(".", "lf1") >= 0) {
    printf("%s: link . lf1 succeeded! oops\n", s);
    exit(1);
  }
}

// test concurrent create/link/unlink of the same file
void
concreate(char *s)
{
  enum { N = 40 };
  char file[3];
  ціле i, pid, n, fd;
  char fa[N];
  struct {
    ushort inum;
    char name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  для (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    якщо (pid && (i % 3) == 1) {
      link("C0", file);
    } інакше якщо (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } інакше {
      fd = open(file, O_CREATE | O_RDWR);
      якщо (fd < 0) {
        printf("concreate create %s failed\n", file);
        exit(1);
      }
      close(fd);
    }
    якщо (pid == 0) {
      exit(0);
    } інакше {
      ціле xstatus;
      wait(&xstatus);
      якщо (xstatus != 0)
        exit(1);
    }
  }

  memset(fa, 0, sizeof(fa));
  fd = open(".", 0);
  n = 0;
  поки (read(fd, &de, sizeof(de)) > 0) {
    якщо (de.inum == 0)
      continue;
    якщо (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      якщо (i < 0 || i >= sizeof(fa)) {
        printf("%s: concreate weird file %s\n", s, de.name);
        exit(1);
      }
      якщо (fa[i]) {
        printf("%s: concreate duplicate file %s\n", s, de.name);
        exit(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  якщо (n != N) {
    printf("%s: concreate not enough files in directory listing\n", s);
    exit(1);
  }

  для (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } інакше {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    якщо (pid == 0)
      exit(0);
    інакше
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink(char *s)
{
  ціле pid, i;

  unlink("x");
  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }

  unsigned ціле x = (pid ? 1 : 97);
  для (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    якщо ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } інакше якщо ((x % 3) == 1) {
      link("cat", "x");
    } інакше {
      unlink("x");
    }
  }

  якщо (pid)
    wait(0);
  інакше
    exit(0);
}

void
subdir(char *s)
{
  ціле fd, cc;

  unlink("ff");
  якщо (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: create dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "ff", 2);
  close(fd);

  якщо (unlink("dd") >= 0) {
    printf("%s: unlink dd (non-empty dir) succeeded!\n", s);
    exit(1);
  }

  якщо (mkdir("/dd/dd") != 0) {
    printf("%s: subdir mkdir dd/dd failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: create dd/dd/ff failed\n", s);
    exit(1);
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  якщо (fd < 0) {
    printf("%s: open dd/dd/../ff failed\n", s);
    exit(1);
  }
  cc = read(fd, buf, sizeof(buf));
  якщо (cc != 2 || buf[0] != 'f') {
    printf("%s: dd/dd/../ff wrong content\n", s);
    exit(1);
  }
  close(fd);

  якщо (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    printf("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    exit(1);
  }

  якщо (unlink("dd/dd/ff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  якщо (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    exit(1);
  }

  якщо (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }
  якщо (chdir("dd/../../dd") != 0) {
    printf("%s: chdir dd/../../dd failed\n", s);
    exit(1);
  }
  якщо (chdir("dd/../../../dd") != 0) {
    printf("%s: chdir dd/../../../dd failed\n", s);
    exit(1);
  }
  якщо (chdir("./..") != 0) {
    printf("%s: chdir ./.. failed\n", s);
    exit(1);
  }

  fd = open("dd/dd/ffff", 0);
  якщо (fd < 0) {
    printf("%s: open dd/dd/ffff failed\n", s);
    exit(1);
  }
  якщо (read(fd, buf, sizeof(buf)) != 2) {
    printf("%s: read dd/dd/ffff wrong len\n", s);
    exit(1);
  }
  close(fd);

  якщо (open("dd/dd/ff", O_RDONLY) >= 0) {
    printf("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    exit(1);
  }

  якщо (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  якщо (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    printf("%s: create dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  якщо (open("dd", O_CREATE) >= 0) {
    printf("%s: create dd succeeded!\n", s);
    exit(1);
  }
  якщо (open("dd", O_RDWR) >= 0) {
    printf("%s: open dd rdwr succeeded!\n", s);
    exit(1);
  }
  якщо (open("dd", O_WRONLY) >= 0) {
    printf("%s: open dd wronly succeeded!\n", s);
    exit(1);
  }
  якщо (link("dd/ff/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  якщо (link("dd/xx/ff", "dd/dd/xx") == 0) {
    printf("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    exit(1);
  }
  якщо (link("dd/ff", "dd/dd/ffff") == 0) {
    printf("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  якщо (mkdir("dd/ff/ff") == 0) {
    printf("%s: mkdir dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  якщо (mkdir("dd/xx/ff") == 0) {
    printf("%s: mkdir dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  якщо (mkdir("dd/dd/ffff") == 0) {
    printf("%s: mkdir dd/dd/ffff succeeded!\n", s);
    exit(1);
  }
  якщо (unlink("dd/xx/ff") == 0) {
    printf("%s: unlink dd/xx/ff succeeded!\n", s);
    exit(1);
  }
  якщо (unlink("dd/ff/ff") == 0) {
    printf("%s: unlink dd/ff/ff succeeded!\n", s);
    exit(1);
  }
  якщо (chdir("dd/ff") == 0) {
    printf("%s: chdir dd/ff succeeded!\n", s);
    exit(1);
  }
  якщо (chdir("dd/xx") == 0) {
    printf("%s: chdir dd/xx succeeded!\n", s);
    exit(1);
  }

  якщо (unlink("dd/dd/ffff") != 0) {
    printf("%s: unlink dd/dd/ff failed\n", s);
    exit(1);
  }
  якщо (unlink("dd/ff") != 0) {
    printf("%s: unlink dd/ff failed\n", s);
    exit(1);
  }
  якщо (unlink("dd") == 0) {
    printf("%s: unlink non-empty dd succeeded!\n", s);
    exit(1);
  }
  якщо (unlink("dd/dd") < 0) {
    printf("%s: unlink dd/dd failed\n", s);
    exit(1);
  }
  якщо (unlink("dd") < 0) {
    printf("%s: unlink dd failed\n", s);
    exit(1);
  }
}

// test writes that are larger than the log.
void
bigwrite(char *s)
{
  ціле fd, sz;

  unlink("bigwrite");
  для (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    якщо (fd < 0) {
      printf("%s: cannot create bigwrite\n", s);
      exit(1);
    }
    ціле i;
    для (i = 0; i < 2; i++) {
      ціле cc = write(fd, buf, sz);
      якщо (cc != sz) {
        printf("%s: write(%d) ret %d\n", s, sz, cc);
        exit(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

void
bigfile(char *s)
{
  enum { N = 20, SZ = 600 };
  ціле fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  для (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    якщо (write(fd, buf, SZ) != SZ) {
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  якщо (fd < 0) {
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  для (i = 0;; i++) {
    cc = read(fd, buf, SZ / 2);
    якщо (cc < 0) {
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    якщо (cc == 0)
      break;
    якщо (cc != SZ / 2) {
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    якщо (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  якщо (total != N * SZ) {
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}

void
fourteen(char *s)
{
  ціле fd;

  // DIRSIZ is 14.

  якщо (mkdir("12345678901234") != 0) {
    printf("%s: mkdir 12345678901234 failed\n", s);
    exit(1);
  }
  якщо (mkdir("12345678901234/123456789012345") != 0) {
    printf("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    exit(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  якщо (fd < 0) {
    printf(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    exit(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  якщо (fd < 0) {
    printf("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    exit(1);
  }
  close(fd);

  якщо (mkdir("12345678901234/12345678901234") == 0) {
    printf("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    exit(1);
  }
  якщо (mkdir("123456789012345/12345678901234") == 0) {
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
rmdot(char *s)
{
  якщо (mkdir("dots") != 0) {
    printf("%s: mkdir dots failed\n", s);
    exit(1);
  }
  якщо (chdir("dots") != 0) {
    printf("%s: chdir dots failed\n", s);
    exit(1);
  }
  якщо (unlink(".") == 0) {
    printf("%s: rm . worked!\n", s);
    exit(1);
  }
  якщо (unlink("..") == 0) {
    printf("%s: rm .. worked!\n", s);
    exit(1);
  }
  якщо (chdir("/") != 0) {
    printf("%s: chdir / failed\n", s);
    exit(1);
  }
  якщо (unlink("dots/.") == 0) {
    printf("%s: unlink dots/. worked!\n", s);
    exit(1);
  }
  якщо (unlink("dots/..") == 0) {
    printf("%s: unlink dots/.. worked!\n", s);
    exit(1);
  }
  якщо (unlink("dots") != 0) {
    printf("%s: unlink dots failed!\n", s);
    exit(1);
  }
}

void
dirfile(char *s)
{
  ціле fd;

  fd = open("dirfile", O_CREATE);
  якщо (fd < 0) {
    printf("%s: create dirfile failed\n", s);
    exit(1);
  }
  close(fd);
  якщо (chdir("dirfile") == 0) {
    printf("%s: chdir dirfile succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", 0);
  якщо (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  якщо (fd >= 0) {
    printf("%s: create dirfile/xx succeeded!\n", s);
    exit(1);
  }
  якщо (mkdir("dirfile/xx") == 0) {
    printf("%s: mkdir dirfile/xx succeeded!\n", s);
    exit(1);
  }
  якщо (unlink("dirfile/xx") == 0) {
    printf("%s: unlink dirfile/xx succeeded!\n", s);
    exit(1);
  }
  якщо (link("README", "dirfile/xx") == 0) {
    printf("%s: link to dirfile/xx succeeded!\n", s);
    exit(1);
  }
  якщо (unlink("dirfile") != 0) {
    printf("%s: unlink dirfile failed!\n", s);
    exit(1);
  }

  fd = open(".", O_RDWR);
  якщо (fd >= 0) {
    printf("%s: open . for writing succeeded!\n", s);
    exit(1);
  }
  fd = open(".", 0);
  якщо (write(fd, "x", 1) > 0) {
    printf("%s: write . succeeded!\n", s);
    exit(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
void
iref(char *s)
{
  ціле i, fd;

  для (i = 0; i < NINODE + 1; i++) {
    якщо (mkdir("irefd") != 0) {
      printf("%s: mkdir irefd failed\n", s);
      exit(1);
    }
    якщо (chdir("irefd") != 0) {
      printf("%s: chdir irefd failed\n", s);
      exit(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    якщо (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    якщо (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  для (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
void
forktest(char *s)
{
  enum { N = 1000 };
  ціле n, pid;

  для (n = 0; n < N; n++) {
    pid = fork();
    якщо (pid < 0)
      break;
    якщо (pid == 0)
      exit(0);
  }

  якщо (n == 0) {
    printf("%s: no fork at all!\n", s);
    exit(1);
  }

  якщо (n == N) {
    printf("%s: fork claimed to work 1000 times!\n", s);
    exit(1);
  }

  для (; n > 0; n--) {
    якщо (wait(0) < 0) {
      printf("%s: wait stopped early\n", s);
      exit(1);
    }
  }

  якщо (wait(0) != -1) {
    printf("%s: wait got too many\n", s);
    exit(1);
  }
}

void
sbrkbasic(char *s)
{
  enum { TOOMUCH = 1024 * 1024 * 1024 };
  ціле i, pid, xstatus;
  char *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  якщо (pid < 0) {
    printf("fork failed in sbrkbasic\n");
    exit(1);
  }
  якщо (pid == 0) {
    a = sbrk(TOOMUCH);
    якщо (a == (char *)SBRK_ERROR) {
      // it's OK if this fails.
      exit(0);
    }

    для (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    exit(1);
  }

  wait(&xstatus);
  якщо (xstatus == 1) {
    printf("%s: too much memory allocated!\n", s);
    exit(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  для (i = 0; i < 5000; i++) {
    b = sbrk(1);
    якщо (b != a) {
      printf("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      exit(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  якщо (pid < 0) {
    printf("%s: sbrk test fork failed\n", s);
    exit(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  якщо (c != a + 1) {
    printf("%s: sbrk test failed post-fork\n", s);
    exit(1);
  }
  якщо (pid == 0)
    exit(0);
  wait(&xstatus);
  exit(xstatus);
}

void
sbrkmuch(char *s)
{
  enum { BIG = 100 * 1024 * 1024 };
  char *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  якщо (p != a) {
    printf("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    exit(1);
  }

  lastaddr = (char *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  якщо (c == (char *)SBRK_ERROR) {
    printf("%s: sbrk could not deallocate\n", s);
    exit(1);
  }
  c = sbrk(0);
  якщо (c != a - PGSIZE) {
    printf("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    exit(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  якщо (c != a || sbrk(0) != a + PGSIZE) {
    printf("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    exit(1);
  }
  якщо (*lastaddr == 99) {
    // should be zero
    printf("%s: sbrk de-allocation didn't really deallocate\n", s);
    exit(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  якщо (c != a) {
    printf("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    exit(1);
  }
}

// can we read the kernel's memory?
void
kernmem(char *s)
{
  char *a;
  ціле pid;

  для (a = (char *)(KERNBASE); a < (char *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid == 0) {
      printf("%s: oops could read %p = %x\n", s, a, *a);
      exit(1);
    }
    ціле xstatus;
    wait(&xstatus);
    якщо (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
void
MAXVAplus(char *s)
{
  volatile uint64 a = MAXVA;
  для (; a != 0; a <<= 1) {
    ціле pid;
    pid = fork();
    якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    якщо (pid == 0) {
      *(char *)a = 99;
      printf("%s: oops wrote %p\n", s, (void *)a);
      exit(1);
    }
    ціле xstatus;
    wait(&xstatus);
    якщо (xstatus != -1) // did kernel kill child?
      exit(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
void
sbrkfail(char *s)
{
  enum { BIG = 100 * 1024 * 1024 };
  ціле i, xstatus;
  ціле fds[2];
  char scratch;
  char *c, *a;
  ціле pids[10];
  ціле pid;
  ціле failed;

  failed = 0;
  якщо (pipe(fds) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
  для (i = 0; i < sizeof(pids) / sizeof(pids[0]); i++) {
    якщо ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      якщо (sbrk(BIG - (uint64)sbrk(0)) == (char *)SBRK_ERROR)
        write(fds[1], "0", 1);
      інакше
        write(fds[1], "1", 1);
      // sit around until killed
      для (;;)
        pause(1000);
    }
    якщо (pids[i] != -1) {
      read(fds[0], &scratch, 1);
      якщо (scratch == '0')
        failed = 1;
    }
  }
  якщо (!failed) {
    printf("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  для (i = 0; i < sizeof(pids) / sizeof(pids[0]); i++) {
    якщо (pids[i] == -1)
      continue;
    kill(pids[i]);
    wait(0);
  }
  якщо (c == (char *)SBRK_ERROR) {
    printf("%s: failed sbrk leaked memory\n", s);
    exit(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  якщо (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    якщо (a == (char *)SBRK_ERROR) {
      exit(0);
    }
    printf("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    exit(1);
  }
  wait(&xstatus);
  якщо (xstatus != 0)
    exit(1);
}

// test reads/writes from/to allocated memory
void
sbrkarg(char *s)
{
  char *a;
  ціле fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  якщо (fd < 0) {
    printf("%s: open sbrk failed\n", s);
    exit(1);
  }
  якщо ((n = write(fd, a, PGSIZE)) < 0) {
    printf("%s: write sbrk failed\n", s);
    exit(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  якщо (pipe((ціле *)a) != 0) {
    printf("%s: pipe() failed\n", s);
    exit(1);
  }
}

void
validatetest(char *s)
{
  ціле hi;
  uint64 p;

  hi = 1100 * 1024;
  для (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    якщо (link("nosuchfile", (char *)p) != -1) {
      printf("%s: link should not succeed\n", s);
      exit(1);
    }
  }
}

// does uninitialized data start out zero?
char uninit[10000];
void
bsstest(char *s)
{
  ціле i;

  для (i = 0; i < sizeof(uninit); i++) {
    якщо (uninit[i] != '\0') {
      printf("%s: bss test failed\n", s);
      exit(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
void
bigargtest(char *s)
{
  ціле pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  якщо (pid == 0) {
    static char *args[MAXARG];
    ціле i;
    char big[400];
    memset(big, ' ', sizeof(big));
    big[sizeof(big) - 1] = '\0';
    для (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit(0);
  } інакше якщо (pid < 0) {
    printf("%s: bigargtest: fork failed\n", s);
    exit(1);
  }

  wait(&xstatus);
  якщо (xstatus != 0)
    exit(xstatus);
  fd = open("bigarg-ok", 0);
  якщо (fd < 0) {
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
  ціле nfiles;
  ціле fsblocks = 0;

  printf("fsfull test\n");

  для (nfiles = 0;; nfiles++) {
    char name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    ціле fd = open(name, O_CREATE | O_RDWR);
    якщо (fd < 0) {
      printf("open %s failed\n", name);
      break;
    }
    ціле total = 0;
    поки (1) {
      ціле cc = write(fd, buf, BSIZE);
      якщо (cc < BSIZE)
        break;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    якщо (total == 0)
      break;
  }

  поки (nfiles >= 0) {
    char name[64];
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
argptest(char *s)
{
  ціле fd;
  fd = open("init", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  read(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
void
stacktest(char *s)
{
  ціле pid;
  ціле xstatus;

  pid = fork();
  якщо (pid == 0) {
    char *sp = (char *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    printf("%s: stacktest: read below stack %d\n", s, *sp);
    exit(1);
  } інакше якщо (pid < 0) {
    printf("%s: fork failed\n", s);
    exit(1);
  }
  wait(&xstatus);
  якщо (xstatus == -1) // kernel killed child?
    exit(0);
  інакше
    exit(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
void
nowrite(char *s)
{
  ціле pid;
  ціле xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  для (ціле ai = 0; ai < sizeof(addrs) / sizeof(addrs[0]); ai++) {
    pid = fork();
    якщо (pid == 0) {
      volatile ціле *addr = (ціле *)addrs[ai];
      *addr = 10;
      printf("%s: write to %p did not fail!\n", s, addr);
      exit(0);
    } інакше якщо (pid < 0) {
      printf("%s: fork failed\n", s);
      exit(1);
    }
    wait(&xstatus);
    якщо (xstatus == 0) {
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
pgbug(char *s)
{
  char *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  exit(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
void
sbrkbugs(char *s)
{
  ціле pid = fork();
  якщо (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  якщо (pid == 0) {
    ціле sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    exit(0);
  }
  wait(0);

  pid = fork();
  якщо (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  якщо (pid == 0) {
    ціле sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    exit(0);
  }
  wait(0);

  pid = fork();
  якщо (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  якщо (pid == 0) {
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
sbrklast(char *s)
{
  uint64 top = (uint64)sbrk(0);
  якщо ((top % PGSIZE) != 0)
    sbrk(PGSIZE - (top % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  top = (uint64)sbrk(0);
  char *p = (char *)(top - 64);
  p[0] = 'x';
  p[1] = '\0';
  ціле fd = open(p, O_RDWR | O_CREATE);
  write(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  read(fd, p, 1);
  якщо (p[0] != 'x')
    exit(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
void
sbrk8000(char *s)
{
  sbrk(0x80000004);
  volatile char *top = sbrk(0);
  *(top - 1) = *(top - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
void
badarg(char *s)
{
  для (ціле i = 0; i < 50000; i++) {
    char *argv[2];
    argv[0] = (char *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  exit(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
void
lazy_alloc(char *s)
{
  char *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  якщо (prev_end == (char *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  для (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(char **)i = i;

  для (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    якщо (*(char **)i != i) {
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
lazy_unmap(char *s)
{
  ціле pid;
  char *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  якщо (prev_end == (char *)SBRK_ERROR) {
    printf("sbrklazy() failed\n");
    exit(1);
  }
  new_end = prev_end + REGION_SZ;

  для (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(char **)i = i;

  для (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = fork();
    якщо (pid < 0) {
      printf("error forking\n");
      exit(1);
    } інакше якщо (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(char **)i = i;
      exit(0);
    } інакше {
      ціле status;
      wait(&status);
      якщо (status == 0) {
        printf("memory not unmapped\n");
        exit(1);
      }
    }
  }

  exit(0);
}

void
lazy_copy(char *s)
{
  // copyinstr on lazy page
  {
    char *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    void *xx = sbrk(0);
    void *ret = sbrk(-(((uint64)xx) + 1));
    якщо (ret != xx) {
      printf("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      exit(1);
    }
  }

  // read() and write() to these addresses should fail.
  unsigned long bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  для (ціле i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
    ціле fd = open("README", 0);
    якщо (fd < 0) {
      printf("cannot open README\n");
      exit(1);
    }
    якщо (read(fd, (char *)bad[i], 512) >= 0) {
      printf("read succeeded\n");
      exit(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    якщо (fd < 0) {
      printf("cannot open junk\n");
      exit(1);
    }
    якщо (write(fd, (char *)bad[i], 512) >= 0) {
      printf("write succeeded\n");
      exit(1);
    }
    close(fd);
  }

  exit(0);
}

void
lazy_copyinstr(char *s)
{
  char *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  якщо ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ціле fd = open(&p[4095], O_RDONLY);
  якщо (fd < 0) {
    printf("could not open /");
    exit(1);
  }

  struct stat st;
  ціле r = fstat(fd, &st);
  якщо (r < 0) {
    printf("could not stat /");
    exit(1);
  }

  якщо (st.type != T_DIR) {
    printf("/ is not T_DIR");
    exit(1);
  }

  close(fd);
}

void
lazy_sbrk(char *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  char *p = sbrk(0);
  поки ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    якщо (p < 0) {
      printf("sbrklazy(%d) returned %p\n", 1 << 30, p);
      exit(1);
    }

    p = sbrklazy(0);
  }

  ціле n = TRAPFRAME - PGSIZE - (uint64)p;

  char *p1 = sbrklazy(n);
  якщо (p1 < 0 || p1 != p) {
    printf("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    exit(1);
  }

  p = sbrk(PGSIZE);
  якщо (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    printf("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    exit(1);
  }

  p[0] = 1;
  якщо (p[1] != 0) {
    printf("sbrk() returned non-zero-filled memory\n");
    exit(1);
  }

  p = sbrk(1);
  якщо ((uint64)p != -1) {
    printf("sbrk(1) returned %p, expected error\n", p);
    exit(1);
  }

  p = sbrklazy(1);
  якщо ((uint64)p != -1) {
    printf("sbrklazy(1) returned %p, expected error\n", p);
    exit(1);
  }

  exit(0);
}

void
partial_write(char *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ціле fd = open("testfile", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: cannot create testfile\n", s);
    exit(1);
  }

  ціле cc = write(fd, "A", 1);
  якщо (cc != 1) {
    printf("%s: could not write A\n", s);
    exit(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  якщо (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  char *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  якщо ((uint64)p % PGSIZE != 0) {
    printf("%s: sbrk did not align\n", s);
    exit(1);
  }

  p[-1] = 'X';

  cc = write(fd, p - 1, 2);
  якщо (cc != -1) {
    printf("%s: write succeeded, should have failed\n", s);
    exit(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  char b;
  cc = read(fd, &b, 1);
  якщо (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  якщо (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  для (ціле i = 0; i < 64; i++) {
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    cc = write(fd, buf, sizeof(buf));
    якщо (cc != sizeof(buf)) {
      printf("%s: could not write to bigfile\n", s);
      exit(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  якщо (fd < 0) {
    printf("%s: cannot re-open testfile\n", s);
    exit(1);
  }

  cc = read(fd, &b, 1);
  якщо (cc != 1) {
    printf("%s: cannot read testfile\n", s);
    exit(1);
  }

  close(fd);

  якщо (b != 'X') {
    printf("%s: read returned %c, expected X\n", s, b);
    exit(1);
  }

  unlink("testfile");
}

void
unlinkcwd(char *s)
{
  якщо (mkdir("/a") < 0) {
    printf("%s: mkdir /a failed\n", s);
    exit(1);
  }
  якщо (mkdir("/a/b") < 0) {
    printf("%s: mkdir /a/b failed\n", s);
    exit(1);
  }
  якщо (chdir("/a/b") < 0) {
    printf("%s: chdir failed\n", s);
    exit(1);
  }
  якщо (unlink("/a/b") < 0) {
    printf("%s: unlink /a/b failed\n", s);
    exit(1);
  }
  якщо (unlink("/a") < 0) {
    printf("%s: unlink /a failed\n", s);
    exit(1);
  }
  якщо (open("../", O_RDONLY) > 0) {
    printf("%s: open ../ non-existing directory\n", s);
  }
  якщо (open("../c", O_CREATE) > 0) {
    printf("%s: create ../c non-existing file\n", s);
  }
}

struct test {
  void (*f)(char *);
  char *s;
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
bigdir(char *s)
{
  enum { N = 500 };
  ціле i, fd;
  char name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  якщо (fd < 0) {
    printf("%s: bigdir create failed\n", s);
    exit(1);
  }
  close(fd);

  для (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    якщо (link("bd", name) != 0) {
      printf("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      exit(1);
    }
  }

  unlink("bd");
  для (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    якщо (unlink(name) != 0) {
      printf("%s: bigdir unlink failed", s);
      exit(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
void
manywrites(char *s)
{
  ціле nchildren = 4;
  ціле howmany = 30; // increase to look for deadlock

  для (ціле ci = 0; ci < nchildren; ci++) {
    ціле pid = fork();
    якщо (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    якщо (pid == 0) {
      char name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      для (ціле iters = 0; iters < howmany; iters++) {
        для (ціле i = 0; i < ci + 1; i++) {
          ціле fd = open(name, O_CREATE | O_RDWR);
          якщо (fd < 0) {
            printf("%s: cannot create %s\n", s, name);
            exit(1);
          }
          ціле sz = sizeof(buf);
          ціле cc = write(fd, buf, sz);
          якщо (cc != sz) {
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

  для (ціле ci = 0; ci < nchildren; ci++) {
    ціле st = 0;
    wait(&st);
    якщо (st != 0)
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
badwrite(char *s)
{
  ціле assumed_free = 600;

  unlink("junk");
  для (ціле i = 0; i < assumed_free; i++) {
    ціле fd = open("junk", O_CREATE | O_WRONLY);
    якщо (fd < 0) {
      printf("open junk failed\n");
      exit(1);
    }
    write(fd, (char *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ціле fd = open("junk", O_CREATE | O_WRONLY);
  якщо (fd < 0) {
    printf("open junk failed\n");
    exit(1);
  }
  якщо (write(fd, "x", 1) != 1) {
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
execout(char *s)
{
  для (ціле avail = 0; avail < 15; avail++) {
    ціле pid = fork();
    якщо (pid < 0) {
      printf("fork failed\n");
      exit(1);
    } інакше якщо (pid == 0) {
      // allocate all of memory.
      поки (1) {
        char *a = sbrk(PGSIZE);
        якщо (a == SBRK_ERROR)
          break;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      для (ціле i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      char *args[] = {"echo", "x", 0};
      exec("echo", args);
      exit(0);
    } інакше {
      wait((ціле *)0);
    }
  }

  exit(0);
}

// can the kernel tolerate running out of disk space?
void
diskfull(char *s)
{
  ціле fi;
  ціле done = 0;

  unlink("diskfulldir");

  для (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    char name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ціле fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    якщо (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      printf("%s: could not create file %s\n", s, name);
      done = 1;
      break;
    }
    для (ціле i = 0; i < MAXFILE; i++) {
      char buf[BSIZE];
      якщо (write(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        close(fd);
        break;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ціле nzz = 128;
  для (ціле i = 0; i < nzz; i++) {
    char name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ціле fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    якщо (fd < 0)
      break;
    close(fd);
  }

  // this mkdir() is expected to fail.
  якщо (mkdir("diskfulldir") == 0)
    printf("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  для (ціле i = 0; i < nzz; i++) {
    char name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  для (ціле i = 0; '0' + i < 0177; i++) {
    char name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

void
outofinodes(char *s)
{
  ціле nzz = 32 * 32;
  для (ціле i = 0; i < nzz; i++) {
    char name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ціле fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    якщо (fd < 0) {
      // failure is eventually expected.
      break;
    }
    close(fd);
  }

  для (ціле i = 0; i < nzz; i++) {
    char name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

void
linkoverflow(char *s)
{
  enum { TARGET = 32768 };
  enum { DIRS = 64 };
  struct stat st;
  ціле i;

  unlink("/lof");
  ціле fd = open("/lof", O_CREATE | O_RDWR);
  якщо (fd < 0) {
    printf("%s: cannot create /lof\n", s);
    exit(1);
  }
  close(fd);

  для (i = 0; i < TARGET; i++) {
    ціле d = i % DIRS;
    ціле f = i / DIRS;

    char pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    якщо (f == 0 && mkdir(pn) < 0) {
      printf("%s: mkdir(%s) failed\n", s, pn);
      exit(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    якщо (link("/lof", pn) < 0) {
      якщо (stat("/lof", &st) < 0) {
        printf("%s: stat(/lof) failed\n", s);
        exit(1);
      }
      якщо (st.nlink >= 32767) {
        // overflow check succeeded.
        break;
      }
      printf("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      exit(1);
    }

    якщо (i % 100 == 0) {
      printf("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  якщо (stat("/lof", &st) < 0) {
    printf("%s: stat(/lof) failed\n", s);
    exit(1);
  }

  unlink("/lof");

  якщо (st.nlink < 0) {
    printf("%s: negative link count: %d\n", s, st.nlink);
    exit(1);
  }
}

struct test slowtests[] = {
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
ціле
run(void f(char *), char *s)
{
  ціле pid;
  ціле xstatus;

  printf("test %s: ", s);
  якщо ((pid = fork()) < 0) {
    printf("runtest: fork error\n");
    exit(1);
  }
  якщо (pid == 0) {
    f(s);
    exit(0);
  } інакше {
    wait(&xstatus);
    якщо (xstatus != 0)
      printf("FAILED\n");
    інакше
      printf("OK\n");
    повернути xstatus == 0;
  }
}

ціле
runtests(struct test *tests, char *justone, ціле continuous)
{
  ціле ntests = 0;
  для (struct test *t = tests; t->s != 0; t++) {
    якщо ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      якщо (!run(t->f, t->s)) {
        якщо (continuous != 2) {
          printf("SOME TESTS FAILED\n");
          повернути -1;
        }
      }
    }
  }
  повернути ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ціле
countfree()
{
  ціле n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  поки (1) {
    char *a = sbrk(PGSIZE);
    якщо (a == SBRK_ERROR) {
      break;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  повернути n;
}

ціле
drivetests(ціле quick, ціле continuous, char *justone)
{
  do {
    printf("usertests starting\n");
    ціле free0 = countfree();
    ціле free1 = 0;
    ціле ntests = 0;
    ціле n;
    n = runtests(quicktests, justone, continuous);
    якщо (n < 0) {
      якщо (continuous != 2) {
        повернути 1;
      }
    } інакше {
      ntests += n;
    }
    якщо (!quick) {
      якщо (justone == 0)
        printf("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      якщо (n < 0) {
        якщо (continuous != 2) {
          повернути 1;
        }
      } інакше {
        ntests += n;
      }
    }
    якщо ((free1 = countfree()) < free0) {
      printf("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      якщо (continuous != 2) {
        повернути 1;
      }
    }
    якщо (justone != 0 && ntests == 0) {
      printf("NO TESTS EXECUTED\n");
      повернути 1;
    }
  } поки (continuous);
  повернути 0;
}

ціле
main(ціле argc, char *argv[])
{
  ціле continuous = 0;
  ціле quick = 0;
  char *justone = 0;

  якщо (argc == 2 && strcmp(argv[1], "-q") == 0) {
    quick = 1;
  } інакше якщо (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } інакше якщо (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } інакше якщо (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } інакше якщо (argc > 1) {
    printf("Usage: usertests [-c] [-C] [-q] [testname]\n");
    exit(1);
  }
  якщо (drivetests(quick, continuous, justone)) {
    exit(1);
  }
  printf("ALL TESTS PASSED\n");
  exit(0);
}
