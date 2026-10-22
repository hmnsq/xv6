#ಅನ್ತರ್ಭಾವ "kernel/param.h"
#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "kernel/syscall.h"
#ಅನ್ತರ್ಭಾವ "kernel/memlayout.h"
#ಅನ್ತರ್ಭಾವ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#ವ್ಯಾಖ್ಯಾನಿಸು BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

ಅಕ್ಷರ buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
ಶೂನ್ಯ
copyin(ಅಕ್ಷರ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ai = 0; ai < ಗಾತ್ರ(addrs) / ಗಾತ್ರ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಂಕ fd = open("copyin1", O_CREATE | O_WRONLY);
    ಆದರೆ (fd < 0) {
      ಬರೆ("open(copyin1) failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ n = ಲಿಖ(fd, (ಶೂನ್ಯ *)addr, 8192);
    ಆದರೆ (n >= 0) {
      ಬರೆ("write(fd, %p, 8192) returned %d, not -1\n", (ಶೂನ್ಯ *)addr, n);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fd);
    unlink("copyin1");

    n = ಲಿಖ(1, (ಅಕ್ಷರ *)addr, 8192);
    ಆದರೆ (n > 0) {
      ಬರೆ("write(1, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr, n);
      ನಿರ್ಗಮಿಸು(1);
    }

    ಪೂರ್ಣಾಂಕ fds[2];
    ಆದರೆ (pipe(fds) < 0) {
      ಬರೆ("pipe() failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    n = ಲಿಖ(fds[1], (ಅಕ್ಷರ *)addr, 8192);
    ಆದರೆ (n > 0) {
      ಬರೆ("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr,
             n);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
ಶೂನ್ಯ
copyout(ಅಕ್ಷರ *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ai = 0; ai < ಗಾತ್ರ(addrs) / ಗಾತ್ರ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಂಕ fd = open("README", 0);
    ಆದರೆ (fd < 0) {
      ಬರೆ("open(README) failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ n = ಪಠ(fd, (ಶೂನ್ಯ *)addr, 8192);
    ಆದರೆ (n > 0) {
      ಬರೆ("read(fd, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr, n);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fd);

    ಪೂರ್ಣಾಂಕ fds[2];
    ಆದರೆ (pipe(fds) < 0) {
      ಬರೆ("pipe() failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    n = ಲಿಖ(fds[1], "x", 1);
    ಆದರೆ (n != 1) {
      ಬರೆ("pipe write failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    n = ಪಠ(fds[0], (ಶೂನ್ಯ *)addr, 8192);
    ಆದರೆ (n > 0) {
      ಬರೆ("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (ಶೂನ್ಯ *)addr,
             n);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
ಶೂನ್ಯ
copyinstr1(ಅಕ್ಷರ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ai = 0; ai < ಗಾತ್ರ(addrs) / ಗಾತ್ರ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ಪೂರ್ಣಾಂಕ fd = open((ಅಕ್ಷರ *)addr, O_CREATE | O_WRONLY);
    ಆದರೆ (fd >= 0) {
      ಬರೆ("open(%p) returned %d, not -1\n", (ಶೂನ್ಯ *)addr, fd);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
ಶೂನ್ಯ
copyinstr2(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ b[MAXPATH + 1];

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ಪೂರ್ಣಾಂಕ ret = unlink(b);
  ಆದರೆ (ret != -1) {
    ಬರೆ("unlink(%s) returned %d, not -1\n", b, ret);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಪೂರ್ಣಾಂಕ fd = open(b, O_CREATE | O_WRONLY);
  ಆದರೆ (fd != -1) {
    ಬರೆ("open(%s) returned %d, not -1\n", b, fd);
    ನಿರ್ಗಮಿಸು(1);
  }

  ret = link(b, b);
  ಆದರೆ (ret != -1) {
    ಬರೆ("link(%s, %s) returned %d, not -1\n", b, b, ret);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಅಕ್ಷರ *ವಾದಗಳು[] = {"xx", 0};
  ret = exec(b, ವಾದಗಳು);
  ಆದರೆ (ret != -1) {
    ಬರೆ("exec(%s) returned %d, not -1\n", b, fd);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಪೂರ್ಣಾಂಕ pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಸ್ಥಿರ ಅಕ್ಷರ big[PGSIZE + 1];
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    ಅಕ್ಷರ *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ಆದರೆ (ret != -1) {
      ಬರೆ("exec(echo, BIG) returned %d, not -1\n", fd);
      ನಿರ್ಗಮಿಸು(1);
    }
    ನಿರ್ಗಮಿಸು(747); // OK
  }

  ಪೂರ್ಣಾಂಕ st = 0;
  wait(&st);
  ಆದರೆ (st != 747) {
    ಬರೆ("exec(echo, BIG) succeeded, should have failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
}

// what if a string argument crosses over the end of last user page?
ಶೂನ್ಯ
copyinstr3(ಅಕ್ಷರ *s)
{
  sbrk(8192);
  uint64 ತುದಿ = (uint64)sbrk(0);
  ಆದರೆ ((ತುದಿ % PGSIZE) != 0) {
    sbrk(PGSIZE - (ತುದಿ % PGSIZE));
  }
  ತುದಿ = (uint64)sbrk(0);
  ಆದರೆ (ತುದಿ % PGSIZE) {
    ಬರೆ("oops\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  ಅಕ್ಷರ *b = (ಅಕ್ಷರ *)(ತುದಿ - 1);
  *b = 'x';

  ಪೂರ್ಣಾಂಕ ret = unlink(b);
  ಆದರೆ (ret != -1) {
    ಬರೆ("unlink(%s) returned %d, not -1\n", b, ret);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಪೂರ್ಣಾಂಕ fd = open(b, O_CREATE | O_WRONLY);
  ಆದರೆ (fd != -1) {
    ಬರೆ("open(%s) returned %d, not -1\n", b, fd);
    ನಿರ್ಗಮಿಸು(1);
  }

  ret = link(b, b);
  ಆದರೆ (ret != -1) {
    ಬರೆ("link(%s, %s) returned %d, not -1\n", b, b, ret);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಅಕ್ಷರ *ವಾದಗಳು[] = {"xx", 0};
  ret = exec(b, ವಾದಗಳು);
  ಆದರೆ (ret != -1) {
    ಬರೆ("exec(%s) returned %d, not -1\n", b, fd);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
ಶೂನ್ಯ
rwsbrk(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, n;

  uint64 a = (uint64)sbrk(8192);

  ಆದರೆ (a == (uint64)SBRK_ERROR) {
    ಬರೆ("sbrk(rwsbrk) failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (sbrk(-8192) == SBRK_ERROR) {
    ಬರೆ("sbrk(rwsbrk) shrink failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("open(rwsbrk) failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  n = ಲಿಖ(fd, (ಶೂನ್ಯ *)(a + PGSIZE), 1024);
  ಆದರೆ (n >= 0) {
    ಬರೆ("write(fd, %p, 1024) returned %d, not -1\n", (ಶೂನ್ಯ *)a + PGSIZE, n);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("open(README) failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  n = ಪಠ(fd, (ಶೂನ್ಯ *)(a + PGSIZE), 10);
  ಆದರೆ (n >= 0) {
    ಬರೆ("read(fd, %p, 10) returned %d, not -1\n", (ಶೂನ್ಯ *)a + PGSIZE, n);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ನಿರ್ಗಮಿಸು(0);
}

// test O_TRUNC.
ಶೂನ್ಯ
truncate1(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ buf[32];

  unlink("truncfile");
  ಪೂರ್ಣಾಂಕ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  ಲಿಖ(fd1, "abcd", 4);
  close(fd1);

  ಪೂರ್ಣಾಂಕ fd2 = open("truncfile", O_RDONLY);
  ಪೂರ್ಣಾಂಕ n = ಪಠ(fd2, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (n != 4) {
    ಬರೆ("%s: read %d bytes, wanted 4\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ಪೂರ್ಣಾಂಕ fd3 = open("truncfile", O_RDONLY);
  n = ಪಠ(fd3, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (n != 0) {
    ಬರೆ("aaa fd3=%d\n", fd3);
    ಬರೆ("%s: read %d bytes, wanted 0\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
  }

  n = ಪಠ(fd2, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (n != 0) {
    ಬರೆ("bbb fd2=%d\n", fd2);
    ಬರೆ("%s: read %d bytes, wanted 0\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಲಿಖ(fd1, "abcdef", 6);

  n = ಪಠ(fd3, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (n != 6) {
    ಬರೆ("%s: read %d bytes, wanted 6\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
  }

  n = ಪಠ(fd2, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (n != 2) {
    ಬರೆ("%s: read %d bytes, wanted 2\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
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
ಶೂನ್ಯ
truncate2(ಅಕ್ಷರ *s)
{
  unlink("truncfile");

  ಪೂರ್ಣಾಂಕ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  ಲಿಖ(fd1, "abcd", 4);

  ಪೂರ್ಣಾಂಕ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ಪೂರ್ಣಾಂಕ n = ಲಿಖ(fd1, "x", 1);
  ಆದರೆ (n != -1) {
    ಬರೆ("%s: write returned %d, expected -1\n", s, n);
    ನಿರ್ಗಮಿಸು(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

ಶೂನ್ಯ
truncate3(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (pid == 0) {
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
      ಅಕ್ಷರ buf[32];
      ಪೂರ್ಣಾಂಕ fd = open("truncfile", O_WRONLY);
      ಆದರೆ (fd < 0) {
        ಬರೆ("%s: open failed\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಪೂರ್ಣಾಂಕ n = ಲಿಖ(fd, "1234567890", 10);
      ಆದರೆ (n != 10) {
        ಬರೆ("%s: write got %d, expected 10\n", s, n);
        ನಿರ್ಗಮಿಸು(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      ಪಠ(fd, buf, ಗಾತ್ರ(buf));
      close(fd);
    }
    ನಿರ್ಗಮಿಸು(0);
  }

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 150; i++) {
    ಪೂರ್ಣಾಂಕ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ಆದರೆ (fd < 0) {
      ಬರೆ("%s: open failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ n = ಲಿಖ(fd, "xxx", 3);
    ಆದರೆ (n != 3) {
      ಬರೆ("%s: write got %d, expected 3\n", s, n);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  ನಿರ್ಗಮಿಸು(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
ಶೂನ್ಯ
iputtest(ಅಕ್ಷರ *s)
{
  ಆದರೆ (mkdir("iputdir") < 0) {
    ಬರೆ("%s: mkdir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("iputdir") < 0) {
    ಬರೆ("%s: chdir iputdir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("../iputdir") < 0) {
    ಬರೆ("%s: unlink ../iputdir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("/") < 0) {
    ಬರೆ("%s: chdir / failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
ಶೂನ್ಯ
exitiputtest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, xstatus;

  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಆದರೆ (mkdir("iputdir") < 0) {
      ಬರೆ("%s: mkdir failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (chdir("iputdir") < 0) {
      ಬರೆ("%s: child chdir failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (unlink("../iputdir") < 0) {
      ಬರೆ("%s: unlink ../iputdir failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ನಿರ್ಗಮಿಸು(0);
  }
  wait(&xstatus);
  ನಿರ್ಗಮಿಸು(xstatus);
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
ಶೂನ್ಯ
openiputtest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, xstatus;

  ಆದರೆ (mkdir("oidir") < 0) {
    ಬರೆ("%s: mkdir oidir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಪೂರ್ಣಾಂಕ fd = open("oidir", O_RDWR);
    ಆದರೆ (fd >= 0) {
      ಬರೆ("%s: open directory for write succeeded\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ನಿರ್ಗಮಿಸು(0);
  }
  pause(1);
  ಆದರೆ (unlink("oidir") != 0) {
    ಬರೆ("%s: unlink failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  wait(&xstatus);
  ನಿರ್ಗಮಿಸು(xstatus);
}

// simple file system tests

ಶೂನ್ಯ
opentest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd;

  fd = open("echo", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open echo failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ಆದರೆ (fd >= 0) {
    ಬರೆ("%s: open doesnotexist succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
writetest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd;
  ಪೂರ್ಣಾಂಕ i;
  ಎಣಿಕೆ { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: error: creat small failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಗಾಗಿ (i = 0; i < N; i++) {
    ಆದರೆ (ಲಿಖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      ಬರೆ("%s: error: write aa %d new file failed\n", s, i);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (ಲಿಖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      ಬರೆ("%s: error: write bb %d new file failed\n", s, i);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: error: open small failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  i = ಪಠ(fd, buf, N * SZ * 2);
  ಆದರೆ (i != N * SZ * 2) {
    ಬರೆ("%s: read failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (unlink("small") < 0) {
    ಬರೆ("%s: unlink small failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
writebig(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: error: creat big failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಗಾಗಿ (i = 0; i < MAXFILE; i++) {
    ((ಪೂರ್ಣಾಂಕ *)buf)[0] = i;
    ಆದರೆ (ಲಿಖ(fd, buf, BSIZE) != BSIZE) {
      ಬರೆ("%s: error: write big file failed i=%d\n", s, i);
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: error: open big failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  n = 0;
  ಗಾಗಿ (;;) {
    i = ಪಠ(fd, buf, BSIZE);
    ಆದರೆ (i == 0) {
      ಆದರೆ (n != MAXFILE) {
        ಬರೆ("%s: read only %d blocks from big", s, n);
        ನಿರ್ಗಮಿಸು(1);
      }
      ನಿಲ್ಲಿಸು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i != BSIZE) {
      ಬರೆ("%s: read failed %d\n", s, i);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (((ಪೂರ್ಣಾಂಕ *)buf)[0] != n) {
      ಬರೆ("%s: read content of block %d is %d\n", s, n, ((ಪೂರ್ಣಾಂಕ *)buf)[0]);
      ನಿರ್ಗಮಿಸು(1);
    }
    n++;
  }
  close(fd);
  ಆದರೆ (unlink("big") < 0) {
    ಬರೆ("%s: unlink big failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// many creates, followed by unlink test
ಶೂನ್ಯ
createtest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ i, fd;
  ಎಣಿಕೆ { N = 52 };

  ಅಕ್ಷರ name[3];
  name[0] = 'a';
  name[2] = '\0';
  ಗಾಗಿ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ಗಾಗಿ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

ಶೂನ್ಯ
dirtest(ಅಕ್ಷರ *s)
{
  ಆದರೆ (mkdir("dir0") < 0) {
    ಬರೆ("%s: mkdir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (chdir("dir0") < 0) {
    ಬರೆ("%s: chdir dir0 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (chdir("..") < 0) {
    ಬರೆ("%s: chdir .. failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (unlink("dir0") < 0) {
    ಬರೆ("%s: unlink dir0 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
exectest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, xstatus, pid;
  ಅಕ್ಷರ *echoargv[] = {"echo", "OK", 0};
  ಅಕ್ಷರ buf[3];

  unlink("echo-ok");
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಪೂರ್ಣಾಂಕ errfd = dup(1);
    ಆದರೆ (errfd < 0) {
      ಬರೆ("%s: dup failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ಆದರೆ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    // won't get to here
  }
  ಆದರೆ (wait(&xstatus) != pid) {
    ಬರೆ("%s: wait failed!\n", s);
  }
  ಆದರೆ (xstatus != 0) {
    ಬರೆ("%s: nonzero wait status %d\n", s, xstatus);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಪಠ(fd, buf, 2) != 2) {
    ಬರೆ("%s: read failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  unlink("echo-ok");
  ಆದರೆ (buf[0] == 'O' && buf[1] == 'K')
    ನಿರ್ಗಮಿಸು(0);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಬರೆ("%s: wrong output\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// simple fork and pipe read/write

ಶೂನ್ಯ
pipe1(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fds[2], pid, xstatus;
  ಪೂರ್ಣಾಂಕ seq, i, n, cc, ಒಟ್ಟು;
  ಎಣಿಕೆ { N = 5, SZ = 1033 };

  ಆದರೆ (pipe(fds) != 0) {
    ಬರೆ("%s: pipe() failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  pid = fork();
  seq = 0;
  ಆದರೆ (pid == 0) {
    close(fds[0]);
    ಗಾಗಿ (n = 0; n < N; n++) {
      ಗಾಗಿ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ಆದರೆ (ಲಿಖ(fds[1], buf, SZ) != SZ) {
        ಬರೆ("%s: pipe1 oops 1\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
    }
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid > 0) {
    close(fds[1]);
    ಒಟ್ಟು = 0;
    cc = 1;
    ಆಗ ((n = ಪಠ(fds[0], buf, cc)) > 0) {
      ಗಾಗಿ (i = 0; i < n; i++) {
        ಆದರೆ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          ಬರೆ("%s: pipe1 oops 2\n", s);
          ಹಿಂತಿರುಗಿ;
        }
      }
      ಒಟ್ಟು += n;
      cc = cc * 2;
      ಆದರೆ (cc > ಗಾತ್ರ(buf))
        cc = ಗಾತ್ರ(buf);
    }
    ಆದರೆ (ಒಟ್ಟು != N * SZ) {
      ಬರೆ("%s: pipe1 oops 3 total %d\n", s, ಒಟ್ಟು);
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fds[0]);
    wait(&xstatus);
    ನಿರ್ಗಮಿಸು(xstatus);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಬರೆ("%s: fork() failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// test if child is killed (status = -1)
ಶೂನ್ಯ
killstatus(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ xst;

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
    ಪೂರ್ಣಾಂಕ pid1 = fork();
    ಆದರೆ (pid1 < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid1 == 0) {
      ಆಗ (1) {
        getpid();
      }
      ನಿರ್ಗಮಿಸು(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ಆದರೆ (xst != -1) {
      ಬರೆ("%s: status should be -1\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  ನಿರ್ಗಮಿಸು(0);
}

// test that kill on unused process 0 doesn't set the killed flag
ಶೂನ್ಯ
killzero(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, xst;

  kill(0);

  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ನಿರ್ಗಮಿಸು(7);
  }
  ಆದರೆ (wait(&xst) != pid) {
    ಬರೆ("%s: wait wrong pid\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (xst != 7) {
    ಬರೆ("%s: child exited with status %d, expected 7\n", s, xst);
    ನಿರ್ಗಮಿಸು(1);
  }
  ನಿರ್ಗಮಿಸು(0);
}

// meant to be run w/ at most two CPUs
ಶೂನ್ಯ
preempt(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid1, pid2, pid3;
  ಪೂರ್ಣಾಂಕ pfds[2];

  pid1 = fork();
  ಆದರೆ (pid1 < 0) {
    ಬರೆ("%s: fork failed", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid1 == 0)
    ಗಾಗಿ (;;)
      ;

  pid2 = fork();
  ಆದರೆ (pid2 < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid2 == 0)
    ಗಾಗಿ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ಆದರೆ (pid3 < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid3 == 0) {
    close(pfds[0]);
    ಆದರೆ (ಲಿಖ(pfds[1], "x", 1) != 1)
      ಬರೆ("%s: preempt write error", s);
    close(pfds[1]);
    ಗಾಗಿ (;;)
      ;
  }

  close(pfds[1]);
  ಆದರೆ (ಪಠ(pfds[0], buf, ಗಾತ್ರ(buf)) != 1) {
    ಬರೆ("%s: preempt read error", s);
    ಹಿಂತಿರುಗಿ;
  }
  close(pfds[0]);
  ಬರೆ("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  ಬರೆ("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
ಶೂನ್ಯ
exitwait(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ i, pid;

  ಗಾಗಿ (i = 0; i < 100; i++) {
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid) {
      ಪೂರ್ಣಾಂಕ xstate;
      ಆದರೆ (wait(&xstate) != pid) {
        ಬರೆ("%s: wait wrong pid\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (i != xstate) {
        ಬರೆ("%s: wait wrong exit status\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      ನಿರ್ಗಮಿಸು(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
ಶೂನ್ಯ
reparent(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ master_pid = getpid();
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 200; i++) {
    ಪೂರ್ಣಾಂಕ pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid) {
      ಆದರೆ (wait(0) != pid) {
        ಬರೆ("%s: wait wrong pid\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಪೂರ್ಣಾಂಕ pid2 = fork();
      ಆದರೆ (pid2 < 0) {
        kill(master_pid);
        ನಿರ್ಗಮಿಸು(1);
      }
      ನಿರ್ಗಮಿಸು(0);
    }
  }
  ನಿರ್ಗಮಿಸು(0);
}

// what if two children exit() at the same time?
ಶೂನ್ಯ
twochildren(ಅಕ್ಷರ *s)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 1000; i++) {
    ಪೂರ್ಣಾಂಕ pid1 = fork();
    ಆದರೆ (pid1 < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid1 == 0) {
      ನಿರ್ಗಮಿಸು(0);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಪೂರ್ಣಾಂಕ pid2 = fork();
      ಆದರೆ (pid2 < 0) {
        ಬರೆ("%s: fork failed\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (pid2 == 0) {
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
ಶೂನ್ಯ
forkfork(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 2 };

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N; i++) {
    ಪೂರ್ಣಾಂಕ pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid == 0) {
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 200; j++) {
        ಪೂರ್ಣಾಂಕ pid1 = fork();
        ಆದರೆ (pid1 < 0) {
          ನಿರ್ಗಮಿಸು(1);
        }
        ಆದರೆ (pid1 == 0) {
          ನಿರ್ಗಮಿಸು(0);
        }
        wait(0);
      }
      ನಿರ್ಗಮಿಸು(0);
    }
  }

  ಪೂರ್ಣಾಂಕ xstatus;
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N; i++) {
    wait(&xstatus);
    ಆದರೆ (xstatus != 0) {
      ಬರೆ("%s: fork in child failed", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
}

ಶೂನ್ಯ
forkforkfork(ಅಕ್ಷರ *s)
{
  unlink("stopforking");

  ಪೂರ್ಣಾಂಕ pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಆಗ (1) {
      ಪೂರ್ಣಾಂಕ fd = open("stopforking", 0);
      ಆದರೆ (fd >= 0) {
        ನಿರ್ಗಮಿಸು(0);
      }
      ಆದರೆ (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    ನಿರ್ಗಮಿಸು(0);
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
ಶೂನ್ಯ
reparent2(ಅಕ್ಷರ *s)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 800; i++) {
    ಪೂರ್ಣಾಂಕ pid1 = fork();
    ಆದರೆ (pid1 < 0) {
      ಬರೆ("fork failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid1 == 0) {
      fork();
      fork();
      ನಿರ್ಗಮಿಸು(0);
    }
    wait(0);
  }

  ನಿರ್ಗಮಿಸು(0);
}

// allocate all mem, free it, and allocate again
ಶೂನ್ಯ
mem(ಅಕ್ಷರ *s)
{
  ಶೂನ್ಯ *m1, *m2;
  ಪೂರ್ಣಾಂಕ pid;

  ಆದರೆ ((pid = fork()) == 0) {
    m1 = 0;
    ಆಗ ((m2 = ಸ್ಮರಣೆ_ಕೊಡು(10001)) != 0) {
      *(ಅಕ್ಷರ **)m2 = m1;
      m1 = m2;
    }
    ಆಗ (m1) {
      m2 = *(ಅಕ್ಷರ **)m1;
      ಬಿಡುಗಡೆ(m1);
      m1 = m2;
    }
    m1 = ಸ್ಮರಣೆ_ಕೊಡು(1024 * 20);
    ಆದರೆ (m1 == 0) {
      ಬರೆ("%s: couldn't allocate mem?!!\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಬಿಡುಗಡೆ(m1);
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಪೂರ್ಣಾಂಕ xstatus;
    wait(&xstatus);
    ಆದರೆ (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      ನಿರ್ಗಮಿಸು(0);
    }
    ನಿರ್ಗಮಿಸು(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
ಶೂನ್ಯ
sharedfd(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, pid, i, n, nc, np;
  ಎಣಿಕೆ { N = 1000, SZ = 10 };
  ಅಕ್ಷರ buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot open sharedfd for writing", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', ಗಾತ್ರ(buf));
  ಗಾಗಿ (i = 0; i < N; i++) {
    ಆದರೆ (ಲಿಖ(fd, buf, ಗಾತ್ರ(buf)) != ಗಾತ್ರ(buf)) {
      ಬರೆ("%s: write sharedfd failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  ಆದರೆ (pid == 0) {
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಪೂರ್ಣಾಂಕ xstatus;
    wait(&xstatus);
    ಆದರೆ (xstatus != 0)
      ನಿರ್ಗಮಿಸು(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot open sharedfd for reading\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  nc = np = 0;
  ಆಗ ((n = ಪಠ(fd, buf, ಗಾತ್ರ(buf))) > 0) {
    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ(buf); i++) {
      ಆದರೆ (buf[i] == 'c')
        nc++;
      ಆದರೆ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ಆದರೆ (nc == N * SZ && np == N * SZ) {
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಬರೆ("%s: nc/np test fails\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
ಶೂನ್ಯ
fourfiles(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, pid, i, j, n, ಒಟ್ಟು, pi;
  ಅಕ್ಷರ *ಹೆಸರುಗಳು[] = {"f0", "f1", "f2", "f3"};
  ಅಕ್ಷರ *fname;
  ಎಣಿಕೆ { N = 12, NCHILD = 4, SZ = 500 };

  ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
    fname = ಹೆಸರುಗಳು[pi];
    unlink(fname);

    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ಆದರೆ (fd < 0) {
        ಬರೆ("%s: create failed\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }

      memset(buf, '0' + pi, SZ);
      ಗಾಗಿ (i = 0; i < N; i++) {
        ಆದರೆ ((n = ಲಿಖ(fd, buf, SZ)) != SZ) {
          ಬರೆ("write failed %d\n", n);
          ನಿರ್ಗಮಿಸು(1);
        }
      }
      ನಿರ್ಗಮಿಸು(0);
    }
  }

  ಪೂರ್ಣಾಂಕ xstatus;
  ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ಆದರೆ (xstatus != 0)
      ನಿರ್ಗಮಿಸು(xstatus);
  }

  ಗಾಗಿ (i = 0; i < NCHILD; i++) {
    fname = ಹೆಸರುಗಳು[i];
    fd = open(fname, 0);
    ಒಟ್ಟು = 0;
    ಆಗ ((n = ಪಠ(fd, buf, ಗಾತ್ರ(buf))) > 0) {
      ಗಾಗಿ (j = 0; j < n; j++) {
        ಆದರೆ (buf[j] != '0' + i) {
          ಬರೆ("%s: wrong char\n", s);
          ನಿರ್ಗಮಿಸು(1);
        }
      }
      ಒಟ್ಟು += n;
    }
    close(fd);
    ಆದರೆ (ಒಟ್ಟು != N * SZ) {
      ಬರೆ("wrong length %d\n", ಒಟ್ಟು);
      ನಿರ್ಗಮಿಸು(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
ಶೂನ್ಯ
createdelete(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 20, NCHILD = 4 };
  ಪೂರ್ಣಾಂಕ pid, i, fd, pi;
  ಅಕ್ಷರ name[32];

  ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ಗಾಗಿ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ಆದರೆ (fd < 0) {
          ಬರೆ("%s: create failed\n", s);
          ನಿರ್ಗಮಿಸು(1);
        }
        close(fd);
        ಆದರೆ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ಆದರೆ (unlink(name) < 0) {
            ಬರೆ("%s: unlink failed\n", s);
            ನಿರ್ಗಮಿಸು(1);
          }
        }
      }
      ನಿರ್ಗಮಿಸು(0);
    }
  }

  ಪೂರ್ಣಾಂಕ xstatus;
  ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ಆದರೆ (xstatus != 0)
      ನಿರ್ಗಮಿಸು(1);
  }

  name[0] = name[1] = name[2] = 0;
  ಗಾಗಿ (i = 0; i < N; i++) {
    ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ಆದರೆ ((i == 0 || i >= N / 2) && fd < 0) {
        ಬರೆ("%s: oops createdelete %s didn't exist\n", s, name);
        ನಿರ್ಗಮಿಸು(1);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((i >= 1 && i < N / 2) && fd >= 0) {
        ಬರೆ("%s: oops createdelete %s did exist\n", s, name);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (fd >= 0)
        close(fd);
    }
  }

  ಗಾಗಿ (i = 0; i < N; i++) {
    ಗಾಗಿ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
ಶೂನ್ಯ
unlinkread(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { SZ = 5 };
  ಪೂರ್ಣಾಂಕ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: create unlinkread failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಲಿಖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open unlinkread failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("unlinkread") != 0) {
    ಬರೆ("%s: unlink unlinkread failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  ಲಿಖ(fd1, "yyy", 3);
  close(fd1);

  ಆದರೆ (ಪಠ(fd, buf, ಗಾತ್ರ(buf)) != SZ) {
    ಬರೆ("%s: unlinkread read failed", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (buf[0] != 'h') {
    ಬರೆ("%s: unlinkread wrong data\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಲಿಖ(fd, buf, 10) != 10) {
    ಬರೆ("%s: unlinkread write failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  unlink("unlinkread");
}

ಶೂನ್ಯ
linktest(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { SZ = 5 };
  ಪೂರ್ಣಾಂಕ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: create lf1 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಲಿಖ(fd, "hello", SZ) != SZ) {
    ಬರೆ("%s: write lf1 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (link("lf1", "lf2") < 0) {
    ಬರೆ("%s: link lf1 lf2 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  unlink("lf1");

  ಆದರೆ (open("lf1", 0) >= 0) {
    ಬರೆ("%s: unlinked lf1 but it is still there!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("lf2", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open lf2 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಪಠ(fd, buf, ಗಾತ್ರ(buf)) != SZ) {
    ಬರೆ("%s: read lf2 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (link("lf2", "lf2") >= 0) {
    ಬರೆ("%s: link lf2 lf2 succeeded! oops\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  unlink("lf2");
  ಆದರೆ (link("lf2", "lf1") >= 0) {
    ಬರೆ("%s: link non-existent succeeded! oops\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (link(".", "lf1") >= 0) {
    ಬರೆ("%s: link . lf1 succeeded! oops\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// test concurrent create/link/unlink of the same file
ಶೂನ್ಯ
concreate(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 40 };
  ಅಕ್ಷರ file[3];
  ಪೂರ್ಣಾಂಕ i, pid, n, fd;
  ಅಕ್ಷರ fa[N];
  ರಚನೆ {
    ushort inum;
    ಅಕ್ಷರ name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ಗಾಗಿ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ಆದರೆ (pid && (i % 3) == 1) {
      link("C0", file);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ಇಲ್ಲದಿದ್ದರೆ {
      fd = open(file, O_CREATE | O_RDWR);
      ಆದರೆ (fd < 0) {
        ಬರೆ("concreate create %s failed\n", file);
        ನಿರ್ಗಮಿಸು(1);
      }
      close(fd);
    }
    ಆದರೆ (pid == 0) {
      ನಿರ್ಗಮಿಸು(0);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಪೂರ್ಣಾಂಕ xstatus;
      wait(&xstatus);
      ಆದರೆ (xstatus != 0)
        ನಿರ್ಗಮಿಸು(1);
    }
  }

  memset(fa, 0, ಗಾತ್ರ(fa));
  fd = open(".", 0);
  n = 0;
  ಆಗ (ಪಠ(fd, &de, ಗಾತ್ರ(de)) > 0) {
    ಆದರೆ (de.inum == 0)
      ಮುಂದುವರಿ;
    ಆದರೆ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ಆದರೆ (i < 0 || i >= ಗಾತ್ರ(fa)) {
        ಬರೆ("%s: concreate weird file %s\n", s, de.name);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (fa[i]) {
        ಬರೆ("%s: concreate duplicate file %s\n", s, de.name);
        ನಿರ್ಗಮಿಸು(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ಆದರೆ (n != N) {
    ಬರೆ("%s: concreate not enough files in directory listing\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಗಾಗಿ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ಇಲ್ಲದಿದ್ದರೆ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ಆದರೆ (pid == 0)
      ನಿರ್ಗಮಿಸು(0);
    ಇಲ್ಲದಿದ್ದರೆ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
ಶೂನ್ಯ
linkunlink(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, i;

  unlink("x");
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ x = (pid ? 1 : 97);
  ಗಾಗಿ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ಆದರೆ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((x % 3) == 1) {
      link("cat", "x");
    } ಇಲ್ಲದಿದ್ದರೆ {
      unlink("x");
    }
  }

  ಆದರೆ (pid)
    wait(0);
  ಇಲ್ಲದಿದ್ದರೆ
    ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
subdir(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, cc;

  unlink("ff");
  ಆದರೆ (mkdir("dd") != 0) {
    ಬರೆ("%s: mkdir dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: create dd/ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಲಿಖ(fd, "ff", 2);
  close(fd);

  ಆದರೆ (unlink("dd") >= 0) {
    ಬರೆ("%s: unlink dd (non-empty dir) succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (mkdir("/dd/dd") != 0) {
    ಬರೆ("%s: subdir mkdir dd/dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: create dd/dd/ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಲಿಖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open dd/dd/../ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  cc = ಪಠ(fd, buf, ಗಾತ್ರ(buf));
  ಆದರೆ (cc != 2 || buf[0] != 'f') {
    ಬರೆ("%s: dd/dd/../ff wrong content\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    ಬರೆ("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (unlink("dd/dd/ff") != 0) {
    ಬರೆ("%s: unlink dd/dd/ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("dd/dd/ff", O_RDONLY) >= 0) {
    ಬರೆ("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (chdir("dd") != 0) {
    ಬರೆ("%s: chdir dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("dd/../../dd") != 0) {
    ಬರೆ("%s: chdir dd/../../dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("dd/../../../dd") != 0) {
    ಬರೆ("%s: chdir dd/../../../dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("./..") != 0) {
    ಬರೆ("%s: chdir ./.. failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("dd/dd/ffff", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open dd/dd/ffff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಪಠ(fd, buf, ಗಾತ್ರ(buf)) != 2) {
    ಬರೆ("%s: read dd/dd/ffff wrong len\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (open("dd/dd/ff", O_RDONLY) >= 0) {
    ಬರೆ("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    ಬರೆ("%s: create dd/ff/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    ಬರೆ("%s: create dd/xx/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("dd", O_CREATE) >= 0) {
    ಬರೆ("%s: create dd succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("dd", O_RDWR) >= 0) {
    ಬರೆ("%s: open dd rdwr succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("dd", O_WRONLY) >= 0) {
    ಬರೆ("%s: open dd wronly succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    ಬರೆ("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    ಬರೆ("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (link("dd/ff", "dd/dd/ffff") == 0) {
    ಬರೆ("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("dd/ff/ff") == 0) {
    ಬರೆ("%s: mkdir dd/ff/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("dd/xx/ff") == 0) {
    ಬರೆ("%s: mkdir dd/xx/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("dd/dd/ffff") == 0) {
    ಬರೆ("%s: mkdir dd/dd/ffff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd/xx/ff") == 0) {
    ಬರೆ("%s: unlink dd/xx/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd/ff/ff") == 0) {
    ಬರೆ("%s: unlink dd/ff/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("dd/ff") == 0) {
    ಬರೆ("%s: chdir dd/ff succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("dd/xx") == 0) {
    ಬರೆ("%s: chdir dd/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (unlink("dd/dd/ffff") != 0) {
    ಬರೆ("%s: unlink dd/dd/ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd/ff") != 0) {
    ಬರೆ("%s: unlink dd/ff failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd") == 0) {
    ಬರೆ("%s: unlink non-empty dd succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd/dd") < 0) {
    ಬರೆ("%s: unlink dd/dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dd") < 0) {
    ಬರೆ("%s: unlink dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// test writes that are larger than the log.
ಶೂನ್ಯ
bigwrite(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd, sz;

  unlink("bigwrite");
  ಗಾಗಿ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ಆದರೆ (fd < 0) {
      ಬರೆ("%s: cannot create bigwrite\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 0; i < 2; i++) {
      ಪೂರ್ಣಾಂಕ cc = ಲಿಖ(fd, buf, sz);
      ಆದರೆ (cc != sz) {
        ಬರೆ("%s: write(%d) ret %d\n", s, sz, cc);
        ನಿರ್ಗಮಿಸು(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

ಶೂನ್ಯ
bigfile(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 20, SZ = 600 };
  ಪೂರ್ಣಾಂಕ fd, i, ಒಟ್ಟು, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot create bigfile", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಗಾಗಿ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ಆದರೆ (ಲಿಖ(fd, buf, SZ) != SZ) {
      ಬರೆ("%s: write bigfile failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot open bigfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಒಟ್ಟು = 0;
  ಗಾಗಿ (i = 0;; i++) {
    cc = ಪಠ(fd, buf, SZ / 2);
    ಆದರೆ (cc < 0) {
      ಬರೆ("%s: read bigfile failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (cc == 0)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ (cc != SZ / 2) {
      ಬರೆ("%s: short read bigfile\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      ಬರೆ("%s: read bigfile wrong data\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಒಟ್ಟು += cc;
  }
  close(fd);
  ಆದರೆ (ಒಟ್ಟು != N * SZ) {
    ಬರೆ("%s: read bigfile wrong total\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  unlink("bigfile.dat");
}

ಶೂನ್ಯ
fourteen(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd;

  // DIRSIZ is 14.

  ಆದರೆ (mkdir("12345678901234") != 0) {
    ಬರೆ("%s: mkdir 12345678901234 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("12345678901234/123456789012345") != 0) {
    ಬರೆ("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ಆದರೆ (fd < 0) {
    ಬರೆ(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಆದರೆ (mkdir("12345678901234/12345678901234") == 0) {
    ಬರೆ("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("123456789012345/12345678901234") == 0) {
    ಬರೆ("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

ಶೂನ್ಯ
rmdot(ಅಕ್ಷರ *s)
{
  ಆದರೆ (mkdir("dots") != 0) {
    ಬರೆ("%s: mkdir dots failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("dots") != 0) {
    ಬರೆ("%s: chdir dots failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink(".") == 0) {
    ಬರೆ("%s: rm . worked!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("..") == 0) {
    ಬರೆ("%s: rm .. worked!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("/") != 0) {
    ಬರೆ("%s: chdir / failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dots/.") == 0) {
    ಬರೆ("%s: unlink dots/. worked!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dots/..") == 0) {
    ಬರೆ("%s: unlink dots/.. worked!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dots") != 0) {
    ಬರೆ("%s: unlink dots failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
dirfile(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd;

  fd = open("dirfile", O_CREATE);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: create dirfile failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  ಆದರೆ (chdir("dirfile") == 0) {
    ಬರೆ("%s: chdir dirfile succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  fd = open("dirfile/xx", 0);
  ಆದರೆ (fd >= 0) {
    ಬರೆ("%s: create dirfile/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ಆದರೆ (fd >= 0) {
    ಬರೆ("%s: create dirfile/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("dirfile/xx") == 0) {
    ಬರೆ("%s: mkdir dirfile/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dirfile/xx") == 0) {
    ಬರೆ("%s: unlink dirfile/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (link("README", "dirfile/xx") == 0) {
    ಬರೆ("%s: link to dirfile/xx succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("dirfile") != 0) {
    ಬರೆ("%s: unlink dirfile failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open(".", O_RDWR);
  ಆದರೆ (fd >= 0) {
    ಬರೆ("%s: open . for writing succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  fd = open(".", 0);
  ಆದರೆ (ಲಿಖ(fd, "x", 1) > 0) {
    ಬರೆ("%s: write . succeeded!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
ಶೂನ್ಯ
iref(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ i, fd;

  ಗಾಗಿ (i = 0; i < NINODE + 1; i++) {
    ಆದರೆ (mkdir("irefd") != 0) {
      ಬರೆ("%s: mkdir irefd failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (chdir("irefd") != 0) {
      ಬರೆ("%s: chdir irefd failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ಆದರೆ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ಆದರೆ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ಗಾಗಿ (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
ಶೂನ್ಯ
forktest(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 1000 };
  ಪೂರ್ಣಾಂಕ n, pid;

  ಗಾಗಿ (n = 0; n < N; n++) {
    pid = fork();
    ಆದರೆ (pid < 0)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ (pid == 0)
      ನಿರ್ಗಮಿಸು(0);
  }

  ಆದರೆ (n == 0) {
    ಬರೆ("%s: no fork at all!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (n == N) {
    ಬರೆ("%s: fork claimed to work 1000 times!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಗಾಗಿ (; n > 0; n--) {
    ಆದರೆ (wait(0) < 0) {
      ಬರೆ("%s: wait stopped early\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  ಆದರೆ (wait(0) != -1) {
    ಬರೆ("%s: wait got too many\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
sbrkbasic(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { TOOMUCH = 1024 * 1024 * 1024 };
  ಪೂರ್ಣಾಂಕ i, pid, xstatus;
  ಅಕ್ಷರ *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("fork failed in sbrkbasic\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    a = sbrk(TOOMUCH);
    ಆದರೆ (a == (ಅಕ್ಷರ *)SBRK_ERROR) {
      // it's OK if this fails.
      ನಿರ್ಗಮಿಸು(0);
    }

    ಗಾಗಿ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    ನಿರ್ಗಮಿಸು(1);
  }

  wait(&xstatus);
  ಆದರೆ (xstatus == 1) {
    ಬರೆ("%s: too much memory allocated!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ಗಾಗಿ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ಆದರೆ (b != a) {
      ಬರೆ("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      ನಿರ್ಗಮಿಸು(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: sbrk test fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ಆದರೆ (c != a + 1) {
    ಬರೆ("%s: sbrk test failed post-fork\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0)
    ನಿರ್ಗಮಿಸು(0);
  wait(&xstatus);
  ನಿರ್ಗಮಿಸು(xstatus);
}

ಶೂನ್ಯ
sbrkmuch(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { BIG = 100 * 1024 * 1024 };
  ಅಕ್ಷರ *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  ಆದರೆ (p != a) {
    ಬರೆ("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    ನಿರ್ಗಮಿಸು(1);
  }

  lastaddr = (ಅಕ್ಷರ *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ಆದರೆ (c == (ಅಕ್ಷರ *)SBRK_ERROR) {
    ಬರೆ("%s: sbrk could not deallocate\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  c = sbrk(0);
  ಆದರೆ (c != a - PGSIZE) {
    ಬರೆ("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    ನಿರ್ಗಮಿಸು(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ಆದರೆ (c != a || sbrk(0) != a + PGSIZE) {
    ಬರೆ("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (*lastaddr == 99) {
    // should be zero
    ಬರೆ("%s: sbrk de-allocation didn't really deallocate\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ಆದರೆ (c != a) {
    ಬರೆ("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    ನಿರ್ಗಮಿಸು(1);
  }
}

// can we read the kernel's memory?
ಶೂನ್ಯ
kernmem(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *a;
  ಪೂರ್ಣಾಂಕ pid;

  ಗಾಗಿ (a = (ಅಕ್ಷರ *)(KERNBASE); a < (ಅಕ್ಷರ *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid == 0) {
      ಬರೆ("%s: oops could read %p = %x\n", s, a, *a);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ xstatus;
    wait(&xstatus);
    ಆದರೆ (xstatus != -1) // did kernel kill child?
      ನಿರ್ಗಮಿಸು(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
ಶೂನ್ಯ
MAXVAplus(ಅಕ್ಷರ *s)
{
  ಅಸ್ಥಿರ uint64 a = MAXVA;
  ಗಾಗಿ (; a != 0; a <<= 1) {
    ಪೂರ್ಣಾಂಕ pid;
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid == 0) {
      *(ಅಕ್ಷರ *)a = 99;
      ಬರೆ("%s: oops wrote %p\n", s, (ಶೂನ್ಯ *)a);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಪೂರ್ಣಾಂಕ xstatus;
    wait(&xstatus);
    ಆದರೆ (xstatus != -1) // did kernel kill child?
      ನಿರ್ಗಮಿಸು(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
ಶೂನ್ಯ
sbrkfail(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { BIG = 100 * 1024 * 1024 };
  ಪೂರ್ಣಾಂಕ i, xstatus;
  ಪೂರ್ಣಾಂಕ fds[2];
  ಅಕ್ಷರ scratch;
  ಅಕ್ಷರ *c, *a;
  ಪೂರ್ಣಾಂಕ pids[10];
  ಪೂರ್ಣಾಂಕ pid;
  ಪೂರ್ಣಾಂಕ failed;

  failed = 0;
  ಆದರೆ (pipe(fds) != 0) {
    ಬರೆ("%s: pipe() failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ(pids) / ಗಾತ್ರ(pids[0]); i++) {
    ಆದರೆ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ಆದರೆ (sbrk(BIG - (uint64)sbrk(0)) == (ಅಕ್ಷರ *)SBRK_ERROR)
        ಲಿಖ(fds[1], "0", 1);
      ಇಲ್ಲದಿದ್ದರೆ
        ಲಿಖ(fds[1], "1", 1);
      // sit around until killed
      ಗಾಗಿ (;;)
        pause(1000);
    }
    ಆದರೆ (pids[i] != -1) {
      ಪಠ(fds[0], &scratch, 1);
      ಆದರೆ (scratch == '0')
        failed = 1;
    }
  }
  ಆದರೆ (!failed) {
    ಬರೆ("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ(pids) / ಗಾತ್ರ(pids[0]); i++) {
    ಆದರೆ (pids[i] == -1)
      ಮುಂದುವರಿ;
    kill(pids[i]);
    wait(0);
  }
  ಆದರೆ (c == (ಅಕ್ಷರ *)SBRK_ERROR) {
    ಬರೆ("%s: failed sbrk leaked memory\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ಆದರೆ (a == (ಅಕ್ಷರ *)SBRK_ERROR) {
      ನಿರ್ಗಮಿಸು(0);
    }
    ಬರೆ("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    ನಿರ್ಗಮಿಸು(1);
  }
  wait(&xstatus);
  ಆದರೆ (xstatus != 0)
    ನಿರ್ಗಮಿಸು(1);
}

// test reads/writes from/to allocated memory
ಶೂನ್ಯ
sbrkarg(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *a;
  ಪೂರ್ಣಾಂಕ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open sbrk failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ ((n = ಲಿಖ(fd, a, PGSIZE)) < 0) {
    ಬರೆ("%s: write sbrk failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ಆದರೆ (pipe((ಪೂರ್ಣಾಂಕ *)a) != 0) {
    ಬರೆ("%s: pipe() failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಶೂನ್ಯ
validatetest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ hi;
  uint64 p;

  hi = 1100 * 1024;
  ಗಾಗಿ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ಆದರೆ (link("nosuchfile", (ಅಕ್ಷರ *)p) != -1) {
      ಬರೆ("%s: link should not succeed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
}

// does uninitialized data start out zero?
ಅಕ್ಷರ uninit[10000];
ಶೂನ್ಯ
bsstest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ(uninit); i++) {
    ಆದರೆ (uninit[i] != '\0') {
      ಬರೆ("%s: bss test failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
ಶೂನ್ಯ
bigargtest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ಆದರೆ (pid == 0) {
    ಸ್ಥಿರ ಅಕ್ಷರ *ವಾದಗಳು[MAXARG];
    ಪೂರ್ಣಾಂಕ i;
    ಅಕ್ಷರ big[400];
    memset(big, ' ', ಗಾತ್ರ(big));
    big[ಗಾತ್ರ(big) - 1] = '\0';
    ಗಾಗಿ (i = 0; i < MAXARG - 1; i++)
      ವಾದಗಳು[i] = big;
    ವಾದಗಳು[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", ವಾದಗಳು);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
    ಬರೆ("%s: bigargtest: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  wait(&xstatus);
  ಆದರೆ (xstatus != 0)
    ನಿರ್ಗಮಿಸು(xstatus);
  fd = open("bigarg-ok", 0);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: bigarg test failed!\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
ಶೂನ್ಯ
fsfull()
{
  ಪೂರ್ಣಾಂಕ nfiles;
  ಪೂರ್ಣಾಂಕ fsblocks = 0;

  ಬರೆ("fsfull test\n");

  ಗಾಗಿ (nfiles = 0;; nfiles++) {
    ಅಕ್ಷರ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    ಬರೆ("writing %s\n", name);
    ಪೂರ್ಣಾಂಕ fd = open(name, O_CREATE | O_RDWR);
    ಆದರೆ (fd < 0) {
      ಬರೆ("open %s failed\n", name);
      ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ಣಾಂಕ ಒಟ್ಟು = 0;
    ಆಗ (1) {
      ಪೂರ್ಣಾಂಕ cc = ಲಿಖ(fd, buf, BSIZE);
      ಆದರೆ (cc < BSIZE)
        ನಿಲ್ಲಿಸು;
      ಒಟ್ಟು += cc;
      fsblocks++;
    }
    ಬರೆ("wrote %d bytes\n", ಒಟ್ಟು);
    close(fd);
    ಆದರೆ (ಒಟ್ಟು == 0)
      ನಿಲ್ಲಿಸು;
  }

  ಆಗ (nfiles >= 0) {
    ಅಕ್ಷರ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  ಬರೆ("fsfull test finished, %d blocks\n", fsblocks);
}

ಶೂನ್ಯ
argptest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fd;
  fd = open("init", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: open failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಪಠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
ಶೂನ್ಯ
stacktest(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid;
  ಪೂರ್ಣಾಂಕ xstatus;

  pid = fork();
  ಆದರೆ (pid == 0) {
    ಅಕ್ಷರ *sp = (ಅಕ್ಷರ *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    ಬರೆ("%s: stacktest: read below stack %d\n", s, *sp);
    ನಿರ್ಗಮಿಸು(1);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
    ಬರೆ("%s: fork failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  wait(&xstatus);
  ಆದರೆ (xstatus == -1) // kernel killed child?
    ನಿರ್ಗಮಿಸು(0);
  ಇಲ್ಲದಿದ್ದರೆ
    ನಿರ್ಗಮಿಸು(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
ಶೂನ್ಯ
nowrite(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid;
  ಪೂರ್ಣಾಂಕ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ai = 0; ai < ಗಾತ್ರ(addrs) / ಗಾತ್ರ(addrs[0]); ai++) {
    pid = fork();
    ಆದರೆ (pid == 0) {
      ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *addr = (ಪೂರ್ಣಾಂಕ *)addrs[ai];
      *addr = 10;
      ಬರೆ("%s: write to %p did not fail!\n", s, addr);
      ನಿರ್ಗಮಿಸು(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
      ಬರೆ("%s: fork failed\n", s);
      ನಿರ್ಗಮಿಸು(1);
    }
    wait(&xstatus);
    ಆದರೆ (xstatus == 0) {
      // kernel did not kill child!
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  ನಿರ್ಗಮಿಸು(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
ಶೂನ್ಯ *big = (ಶೂನ್ಯ *)0xeaeb0b5b00002f5e;
ಶೂನ್ಯ
pgbug(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  ನಿರ್ಗಮಿಸು(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
ಶೂನ್ಯ
sbrkbugs(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಪೂರ್ಣಾಂಕ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    ನಿರ್ಗಮಿಸು(0);
  }
  wait(0);

  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    ಪೂರ್ಣಾಂಕ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    ನಿರ್ಗಮಿಸು(0);
  }
  wait(0);

  pid = fork();
  ಆದರೆ (pid < 0) {
    ಬರೆ("fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    ನಿರ್ಗಮಿಸು(0);
  }
  wait(0);

  ನಿರ್ಗಮಿಸು(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
ಶೂನ್ಯ
sbrklast(ಅಕ್ಷರ *s)
{
  uint64 ತುದಿ = (uint64)sbrk(0);
  ಆದರೆ ((ತುದಿ % PGSIZE) != 0)
    sbrk(PGSIZE - (ತುದಿ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  ತುದಿ = (uint64)sbrk(0);
  ಅಕ್ಷರ *p = (ಅಕ್ಷರ *)(ತುದಿ - 64);
  p[0] = 'x';
  p[1] = '\0';
  ಪೂರ್ಣಾಂಕ fd = open(p, O_RDWR | O_CREATE);
  ಲಿಖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  ಪಠ(fd, p, 1);
  ಆದರೆ (p[0] != 'x')
    ನಿರ್ಗಮಿಸು(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
ಶೂನ್ಯ
sbrk8000(ಅಕ್ಷರ *s)
{
  sbrk(0x80000004);
  ಅಸ್ಥಿರ ಅಕ್ಷರ *ತುದಿ = sbrk(0);
  *(ತುದಿ - 1) = *(ತುದಿ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
ಶೂನ್ಯ
badarg(ಅಕ್ಷರ *s)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50000; i++) {
    ಅಕ್ಷರ *argv[2];
    argv[0] = (ಅಕ್ಷರ *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  ನಿರ್ಗಮಿಸು(0);
}

#ವ್ಯಾಖ್ಯಾನಿಸು REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
ಶೂನ್ಯ
lazy_alloc(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *i, *prev_ಕೊನೆ, *new_ಕೊನೆ;

  prev_ಕೊನೆ = sbrklazy(REGION_SZ);
  ಆದರೆ (prev_ಕೊನೆ == (ಅಕ್ಷರ *)SBRK_ERROR) {
    ಬರೆ("sbrklazy() failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  new_ಕೊನೆ = prev_ಕೊನೆ + REGION_SZ;

  ಗಾಗಿ (i = prev_ಕೊನೆ + PGSIZE; i < new_ಕೊನೆ; i += 64 * PGSIZE)
    *(ಅಕ್ಷರ **)i = i;

  ಗಾಗಿ (i = prev_ಕೊನೆ + PGSIZE; i < new_ಕೊನೆ; i += 64 * PGSIZE) {
    ಆದರೆ (*(ಅಕ್ಷರ **)i != i) {
      ಬರೆ("failed to read value from memory\n");
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  ನಿರ್ಗಮಿಸು(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
ಶೂನ್ಯ
lazy_unmap(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid;
  ಅಕ್ಷರ *i, *prev_ಕೊನೆ, *new_ಕೊನೆ;

  prev_ಕೊನೆ = sbrklazy(REGION_SZ);
  ಆದರೆ (prev_ಕೊನೆ == (ಅಕ್ಷರ *)SBRK_ERROR) {
    ಬರೆ("sbrklazy() failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  new_ಕೊನೆ = prev_ಕೊನೆ + REGION_SZ;

  ಗಾಗಿ (i = prev_ಕೊನೆ + PGSIZE; i < new_ಕೊನೆ; i += PGSIZE * PGSIZE)
    *(ಅಕ್ಷರ **)i = i;

  ಗಾಗಿ (i = prev_ಕೊನೆ + PGSIZE; i < new_ಕೊನೆ; i += PGSIZE * PGSIZE) {
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("error forking\n");
      ನಿರ್ಗಮಿಸು(1);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(ಅಕ್ಷರ **)i = i;
      ನಿರ್ಗಮಿಸು(0);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
      wait(&ಸ್ಥಿತಿ_2);
      ಆದರೆ (ಸ್ಥಿತಿ_2 == 0) {
        ಬರೆ("memory not unmapped\n");
        ನಿರ್ಗಮಿಸು(1);
      }
    }
  }

  ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
lazy_copy(ಅಕ್ಷರ *s)
{
  // copyinstr on lazy page
  {
    ಅಕ್ಷರ *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    ಶೂನ್ಯ *xx = sbrk(0);
    ಶೂನ್ಯ *ret = sbrk(-(((uint64)xx) + 1));
    ಆದರೆ (ret != xx) {
      ಬರೆ("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  // read() and write() to these addresses should fail.
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಾತ್ರ(bad) / ಗಾತ್ರ(bad[0]); i++) {
    ಪೂರ್ಣಾಂಕ fd = open("README", 0);
    ಆದರೆ (fd < 0) {
      ಬರೆ("cannot open README\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (ಪಠ(fd, (ಅಕ್ಷರ *)bad[i], 512) >= 0) {
      ಬರೆ("read succeeded\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ಆದರೆ (fd < 0) {
      ಬರೆ("cannot open junk\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (ಲಿಖ(fd, (ಅಕ್ಷರ *)bad[i], 512) >= 0) {
      ಬರೆ("write succeeded\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    close(fd);
  }

  ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
lazy_copyinstr(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ಆದರೆ ((uint64)p % PGSIZE != 0) {
    ಬರೆ("%s: sbrk did not align\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ಪೂರ್ಣಾಂಕ fd = open(&p[4095], O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("could not open /");
    ನಿರ್ಗಮಿಸು(1);
  }

  ರಚನೆ stat st;
  ಪೂರ್ಣಾಂಕ r = fstat(fd, &st);
  ಆದರೆ (r < 0) {
    ಬರೆ("could not stat /");
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (st.type != T_DIR) {
    ಬರೆ("/ is not T_DIR");
    ನಿರ್ಗಮಿಸು(1);
  }

  close(fd);
}

ಶೂನ್ಯ
lazy_sbrk(ಅಕ್ಷರ *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  ಅಕ್ಷರ *p = sbrk(0);
  ಆಗ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ಆದರೆ (p < 0) {
      ಬರೆ("sbrklazy(%d) returned %p\n", 1 << 30, p);
      ನಿರ್ಗಮಿಸು(1);
    }

    p = sbrklazy(0);
  }

  ಪೂರ್ಣಾಂಕ n = TRAPFRAME - PGSIZE - (uint64)p;

  ಅಕ್ಷರ *p1 = sbrklazy(n);
  ಆದರೆ (p1 < 0 || p1 != p) {
    ಬರೆ("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    ನಿರ್ಗಮಿಸು(1);
  }

  p = sbrk(PGSIZE);
  ಆದರೆ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    ಬರೆ("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    ನಿರ್ಗಮಿಸು(1);
  }

  p[0] = 1;
  ಆದರೆ (p[1] != 0) {
    ಬರೆ("sbrk() returned non-zero-filled memory\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  p = sbrk(1);
  ಆದರೆ ((uint64)p != -1) {
    ಬರೆ("sbrk(1) returned %p, expected error\n", p);
    ನಿರ್ಗಮಿಸು(1);
  }

  p = sbrklazy(1);
  ಆದರೆ ((uint64)p != -1) {
    ಬರೆ("sbrklazy(1) returned %p, expected error\n", p);
    ನಿರ್ಗಮಿಸು(1);
  }

  ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
partial_write(ಅಕ್ಷರ *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ಪೂರ್ಣಾಂಕ fd = open("testfile", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot create testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಪೂರ್ಣಾಂಕ cc = ಲಿಖ(fd, "A", 1);
  ಆದರೆ (cc != 1) {
    ಬರೆ("%s: could not write A\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot re-open testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಅಕ್ಷರ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ಆದರೆ ((uint64)p % PGSIZE != 0) {
    ಬರೆ("%s: sbrk did not align\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  p[-1] = 'X';

  cc = ಲಿಖ(fd, p - 1, 2);
  ಆದರೆ (cc != -1) {
    ಬರೆ("%s: write succeeded, should have failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot re-open testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಅಕ್ಷರ b;
  cc = ಪಠ(fd, &b, 1);
  ಆದರೆ (cc != 1) {
    ಬರೆ("%s: cannot read testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  close(fd);

  ಆದರೆ (b != 'X') {
    ಬರೆ("%s: read returned %c, expected X\n", s, b);
    ನಿರ್ಗಮಿಸು(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 64; i++) {
    ಅಕ್ಷರ buf[1024];
    memset(buf, 0, ಗಾತ್ರ(buf));
    cc = ಲಿಖ(fd, buf, ಗಾತ್ರ(buf));
    ಆದರೆ (cc != ಗಾತ್ರ(buf)) {
      ಬರೆ("%s: could not write to bigfile\n", s);
      ನಿರ್ಗಮಿಸು(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot re-open testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  cc = ಪಠ(fd, &b, 1);
  ಆದರೆ (cc != 1) {
    ಬರೆ("%s: cannot read testfile\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  close(fd);

  ಆದರೆ (b != 'X') {
    ಬರೆ("%s: read returned %c, expected X\n", s, b);
    ನಿರ್ಗಮಿಸು(1);
  }

  unlink("testfile");
}

ಶೂನ್ಯ
unlinkcwd(ಅಕ್ಷರ *s)
{
  ಆದರೆ (mkdir("/a") < 0) {
    ಬರೆ("%s: mkdir /a failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (mkdir("/a/b") < 0) {
    ಬರೆ("%s: mkdir /a/b failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (chdir("/a/b") < 0) {
    ಬರೆ("%s: chdir failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("/a/b") < 0) {
    ಬರೆ("%s: unlink /a/b failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink("/a") < 0) {
    ಬರೆ("%s: unlink /a failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open("../", O_RDONLY) > 0) {
    ಬರೆ("%s: open ../ non-existing directory\n", s);
  }
  ಆದರೆ (open("../c", O_CREATE) > 0) {
    ಬರೆ("%s: create ../c non-existing file\n", s);
  }
}

ರಚನೆ test {
  ಶೂನ್ಯ (*f)(ಅಕ್ಷರ *);
  ಅಕ್ಷರ *s;
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
ಶೂನ್ಯ
bigdir(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { N = 500 };
  ಪೂರ್ಣಾಂಕ i, fd;
  ಅಕ್ಷರ name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: bigdir create failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಗಾಗಿ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ಆದರೆ (link("bd", name) != 0) {
      ಬರೆ("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  unlink("bd");
  ಗಾಗಿ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ಆದರೆ (unlink(name) != 0) {
      ಬರೆ("%s: bigdir unlink failed", s);
      ನಿರ್ಗಮಿಸು(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
ಶೂನ್ಯ
manywrites(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ nchildren = 4;
  ಪೂರ್ಣಾಂಕ howmany = 30; // increase to look for deadlock

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ci = 0; ci < nchildren; ci++) {
    ಪೂರ್ಣಾಂಕ pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("fork failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (pid == 0) {
      ಅಕ್ಷರ name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ iters = 0; iters < howmany; iters++) {
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ci + 1; i++) {
          ಪೂರ್ಣಾಂಕ fd = open(name, O_CREATE | O_RDWR);
          ಆದರೆ (fd < 0) {
            ಬರೆ("%s: cannot create %s\n", s, name);
            ನಿರ್ಗಮಿಸು(1);
          }
          ಪೂರ್ಣಾಂಕ sz = ಗಾತ್ರ(buf);
          ಪೂರ್ಣಾಂಕ cc = ಲಿಖ(fd, buf, sz);
          ಆದರೆ (cc != sz) {
            ಬರೆ("%s: write(%d) ret %d\n", s, sz, cc);
            ನಿರ್ಗಮಿಸು(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      ನಿರ್ಗಮಿಸು(0);
    }
  }

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ci = 0; ci < nchildren; ci++) {
    ಪೂರ್ಣಾಂಕ st = 0;
    wait(&st);
    ಆದರೆ (st != 0)
      ನಿರ್ಗಮಿಸು(st);
  }
  ನಿರ್ಗಮಿಸು(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
ಶೂನ್ಯ
badwrite(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ assumed_free = 600;

  unlink("junk");
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < assumed_free; i++) {
    ಪೂರ್ಣಾಂಕ fd = open("junk", O_CREATE | O_WRONLY);
    ಆದರೆ (fd < 0) {
      ಬರೆ("open junk failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಲಿಖ(fd, (ಅಕ್ಷರ *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ಪೂರ್ಣಾಂಕ fd = open("junk", O_CREATE | O_WRONLY);
  ಆದರೆ (fd < 0) {
    ಬರೆ("open junk failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (ಲಿಖ(fd, "x", 1) != 1) {
    ಬರೆ("write failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);
  unlink("junk");

  ನಿರ್ಗಮಿಸು(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
ಶೂನ್ಯ
execout(ಅಕ್ಷರ *s)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ avail = 0; avail < 15; avail++) {
    ಪೂರ್ಣಾಂಕ pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("fork failed\n");
      ನಿರ್ಗಮಿಸು(1);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid == 0) {
      // allocate all of memory.
      ಆಗ (1) {
        ಅಕ್ಷರ *a = sbrk(PGSIZE);
        ಆದರೆ (a == SBRK_ERROR)
          ನಿಲ್ಲಿಸು;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      ಅಕ್ಷರ *ವಾದಗಳು[] = {"echo", "x", 0};
      exec("echo", ವಾದಗಳು);
      ನಿರ್ಗಮಿಸು(0);
    } ಇಲ್ಲದಿದ್ದರೆ {
      wait((ಪೂರ್ಣಾಂಕ *)0);
    }
  }

  ನಿರ್ಗಮಿಸು(0);
}

// can the kernel tolerate running out of disk space?
ಶೂನ್ಯ
diskfull(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ fi;
  ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 0;

  unlink("diskfulldir");

  ಗಾಗಿ (fi = 0; ಮುಗಿದ == 0 && '0' + fi < 0177; fi++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಂಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಆದರೆ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      ಬರೆ("%s: could not create file %s\n", s, name);
      ಮುಗಿದ = 1;
      ನಿಲ್ಲಿಸು;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < MAXFILE; i++) {
      ಅಕ್ಷರ buf[BSIZE];
      ಆದರೆ (ಲಿಖ(fd, buf, BSIZE) != BSIZE) {
        ಮುಗಿದ = 1;
        close(fd);
        ನಿಲ್ಲಿಸು;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ಪೂರ್ಣಾಂಕ nzz = 128;
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nzz; i++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಂಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಆದರೆ (fd < 0)
      ನಿಲ್ಲಿಸು;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ಆದರೆ (mkdir("diskfulldir") == 0)
    ಬರೆ("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nzz; i++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; '0' + i < 0177; i++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

ಶೂನ್ಯ
outofinodes(ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ nzz = 32 * 32;
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nzz; i++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ಪೂರ್ಣಾಂಕ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ಆದರೆ (fd < 0) {
      // failure is eventually expected.
      ನಿಲ್ಲಿಸು;
    }
    close(fd);
  }

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nzz; i++) {
    ಅಕ್ಷರ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

ಶೂನ್ಯ
linkoverflow(ಅಕ್ಷರ *s)
{
  ಎಣಿಕೆ { ಗುರಿ_2 = 32768 };
  ಎಣಿಕೆ { DIRS = 64 };
  ರಚನೆ stat st;
  ಪೂರ್ಣಾಂಕ i;

  unlink("/lof");
  ಪೂರ್ಣಾಂಕ fd = open("/lof", O_CREATE | O_RDWR);
  ಆದರೆ (fd < 0) {
    ಬರೆ("%s: cannot create /lof\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  close(fd);

  ಗಾಗಿ (i = 0; i < ಗುರಿ_2; i++) {
    ಪೂರ್ಣಾಂಕ d = i % DIRS;
    ಪೂರ್ಣಾಂಕ f = i / DIRS;

    ಅಕ್ಷರ pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ಆದರೆ (f == 0 && mkdir(pn) < 0) {
      ಬರೆ("%s: mkdir(%s) failed\n", s, pn);
      ನಿರ್ಗಮಿಸು(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ಆದರೆ (link("/lof", pn) < 0) {
      ಆದರೆ (stat("/lof", &st) < 0) {
        ಬರೆ("%s: stat(/lof) failed\n", s);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (st.nlink >= 32767) {
        // overflow check succeeded.
        ನಿಲ್ಲಿಸು;
      }
      ಬರೆ("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (i % 100 == 0) {
      ಬರೆ("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ಆದರೆ (stat("/lof", &st) < 0) {
    ಬರೆ("%s: stat(/lof) failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  unlink("/lof");

  ಆದರೆ (st.nlink < 0) {
    ಬರೆ("%s: negative link count: %d\n", s, st.nlink);
    ನಿರ್ಗಮಿಸು(1);
  }
}

ರಚನೆ test slowtests[] = {
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
ಪೂರ್ಣಾಂಕ
ಸತತ(ಶೂನ್ಯ f(ಅಕ್ಷರ *), ಅಕ್ಷರ *s)
{
  ಪೂರ್ಣಾಂಕ pid;
  ಪೂರ್ಣಾಂಕ xstatus;

  ಬರೆ("test %s: ", s);
  ಆದರೆ ((pid = fork()) < 0) {
    ಬರೆ("runtest: fork error\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid == 0) {
    f(s);
    ನಿರ್ಗಮಿಸು(0);
  } ಇಲ್ಲದಿದ್ದರೆ {
    wait(&xstatus);
    ಆದರೆ (xstatus != 0)
      ಬರೆ("FAILED\n");
    ಇಲ್ಲದಿದ್ದರೆ
      ಬರೆ("OK\n");
    ಹಿಂತಿರುಗಿ xstatus == 0;
  }
}

ಪೂರ್ಣಾಂಕ
runtests(ರಚನೆ test *tests, ಅಕ್ಷರ *justone, ಪೂರ್ಣಾಂಕ continuous)
{
  ಪೂರ್ಣಾಂಕ ntests = 0;
  ಗಾಗಿ (ರಚನೆ test *t = tests; t->s != 0; t++) {
    ಆದರೆ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ಆದರೆ (!ಸತತ(t->f, t->s)) {
        ಆದರೆ (continuous != 2) {
          ಬರೆ("SOME TESTS FAILED\n");
          ಹಿಂತಿರುಗಿ -1;
        }
      }
    }
  }
  ಹಿಂತಿರುಗಿ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ಪೂರ್ಣಾಂಕ
countfree()
{
  ಪೂರ್ಣಾಂಕ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ಆಗ (1) {
    ಅಕ್ಷರ *a = sbrk(PGSIZE);
    ಆದರೆ (a == SBRK_ERROR) {
      ನಿಲ್ಲಿಸು;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ
drivetests(ಪೂರ್ಣಾಂಕ ಶೀಘ್ರ, ಪೂರ್ಣಾಂಕ continuous, ಅಕ್ಷರ *justone)
{
  ಮಾಡು {
    ಬರೆ("usertests starting\n");
    ಪೂರ್ಣಾಂಕ free0 = countfree();
    ಪೂರ್ಣಾಂಕ free1 = 0;
    ಪೂರ್ಣಾಂಕ ntests = 0;
    ಪೂರ್ಣಾಂಕ n;
    n = runtests(quicktests, justone, continuous);
    ಆದರೆ (n < 0) {
      ಆದರೆ (continuous != 2) {
        ಹಿಂತಿರುಗಿ 1;
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      ntests += n;
    }
    ಆದರೆ (!ಶೀಘ್ರ) {
      ಆದರೆ (justone == 0)
        ಬರೆ("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ಆದರೆ (n < 0) {
        ಆದರೆ (continuous != 2) {
          ಹಿಂತಿರುಗಿ 1;
        }
      } ಇಲ್ಲದಿದ್ದರೆ {
        ntests += n;
      }
    }
    ಆದರೆ ((free1 = countfree()) < free0) {
      ಬರೆ("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ಆದರೆ (continuous != 2) {
        ಹಿಂತಿರುಗಿ 1;
      }
    }
    ಆದರೆ (justone != 0 && ntests == 0) {
      ಬರೆ("NO TESTS EXECUTED\n");
      ಹಿಂತಿರುಗಿ 1;
    }
  } ಆಗ (continuous);
  ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ continuous = 0;
  ಪೂರ್ಣಾಂಕ ಶೀಘ್ರ = 0;
  ಅಕ್ಷರ *justone = 0;

  ಆದರೆ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    ಶೀಘ್ರ = 1;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc > 1) {
    ಬರೆ("Usage: usertests [-c] [-C] [-q] [testname]\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (drivetests(ಶೀಘ್ರ, continuous, justone)) {
    ನಿರ್ಗಮಿಸು(1);
  }
  ಬರೆ("ALL TESTS PASSED\n");
  ನಿರ್ಗಮಿಸು(0);
}
