#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/syscall.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/riscv.h"

//
// Tests xv6 system calls.  usertests without arguments runs them all
// and usertests <name> runs <name> test. The test runner creates for
// each test a process and based on the exit status of the process,
// the test runner reports "OK" or "FAILED".  Some tests result in
// kernel printing usertrap messages, which can be ignored if test
// prints "OK".
//

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSZ ((MAXOPBLOCKS + 2) * BSIZE)

ਅੱਖਰ buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
ਖਾਲੀ
copyin(ਅੱਖਰ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ਲਈ (ਪੂਰਨ ai = 0; ai < ਆਕਾਰ(addrs) / ਆਕਾਰ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ਪੂਰਨ fd = open("copyin1", O_CREATE | O_WRONLY);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("open(copyin1) failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ n = ਲਿਖ(fd, (ਖਾਲੀ *)addr, 8192);
    ਜੇ (n >= 0) {
      ਲਿਖੋ("write(fd, %p, 8192) returned %d, not -1\n", (ਖਾਲੀ *)addr, n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fd);
    unlink("copyin1");

    n = ਲਿਖ(1, (ਅੱਖਰ *)addr, 8192);
    ਜੇ (n > 0) {
      ਲਿਖੋ("write(1, %p, 8192) returned %d, not -1 or 0\n", (ਖਾਲੀ *)addr, n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਪੂਰਨ fds[2];
    ਜੇ (pipe(fds) < 0) {
      ਲਿਖੋ("pipe() failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    n = ਲਿਖ(fds[1], (ਅੱਖਰ *)addr, 8192);
    ਜੇ (n > 0) {
      ਲਿਖੋ("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (ਖਾਲੀ *)addr,
             n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
ਖਾਲੀ
copyout(ਅੱਖਰ *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  ਲਈ (ਪੂਰਨ ai = 0; ai < ਆਕਾਰ(addrs) / ਆਕਾਰ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ਪੂਰਨ fd = open("README", 0);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("open(README) failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ n = ਪਠ(fd, (ਖਾਲੀ *)addr, 8192);
    ਜੇ (n > 0) {
      ਲਿਖੋ("read(fd, %p, 8192) returned %d, not -1 or 0\n", (ਖਾਲੀ *)addr, n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fd);

    ਪੂਰਨ fds[2];
    ਜੇ (pipe(fds) < 0) {
      ਲਿਖੋ("pipe() failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    n = ਲਿਖ(fds[1], "x", 1);
    ਜੇ (n != 1) {
      ਲਿਖੋ("pipe write failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    n = ਪਠ(fds[0], (ਖਾਲੀ *)addr, 8192);
    ਜੇ (n > 0) {
      ਲਿਖੋ("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (ਖਾਲੀ *)addr,
             n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fds[0]);
    close(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
ਖਾਲੀ
copyinstr1(ਅੱਖਰ *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  ਲਈ (ਪੂਰਨ ai = 0; ai < ਆਕਾਰ(addrs) / ਆਕਾਰ(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    ਪੂਰਨ fd = open((ਅੱਖਰ *)addr, O_CREATE | O_WRONLY);
    ਜੇ (fd >= 0) {
      ਲਿਖੋ("open(%p) returned %d, not -1\n", (ਖਾਲੀ *)addr, fd);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
ਖਾਲੀ
copyinstr2(ਅੱਖਰ *s)
{
  ਅੱਖਰ b[MAXPATH + 1];

  ਲਈ (ਪੂਰਨ i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  ਪੂਰਨ ret = unlink(b);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("unlink(%s) returned %d, not -1\n", b, ret);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਪੂਰਨ fd = open(b, O_CREATE | O_WRONLY);
  ਜੇ (fd != -1) {
    ਲਿਖੋ("open(%s) returned %d, not -1\n", b, fd);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ret = link(b, b);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("link(%s, %s) returned %d, not -1\n", b, b, ret);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਅੱਖਰ *ਦਲੀਲਾਂ[] = {"xx", 0};
  ret = exec(b, ਦਲੀਲਾਂ);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("exec(%s) returned %d, not -1\n", b, fd);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਪੂਰਨ pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਸਥਿਰ ਅੱਖਰ big[PGSIZE + 1];
    ਲਈ (ਪੂਰਨ i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    ਅੱਖਰ *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    ਜੇ (ret != -1) {
      ਲਿਖੋ("exec(echo, BIG) returned %d, not -1\n", fd);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਬਾਹਰ_ਨਿਕਲੋ(747); // OK
  }

  ਪੂਰਨ st = 0;
  wait(&st);
  ਜੇ (st != 747) {
    ਲਿਖੋ("exec(echo, BIG) succeeded, should have failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// what if a string argument crosses over the end of last user page?
ਖਾਲੀ
copyinstr3(ਅੱਖਰ *s)
{
  sbrk(8192);
  uint64 ਸਿਖਰ = (uint64)sbrk(0);
  ਜੇ ((ਸਿਖਰ % PGSIZE) != 0) {
    sbrk(PGSIZE - (ਸਿਖਰ % PGSIZE));
  }
  ਸਿਖਰ = (uint64)sbrk(0);
  ਜੇ (ਸਿਖਰ % PGSIZE) {
    ਲਿਖੋ("oops\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਅੱਖਰ *b = (ਅੱਖਰ *)(ਸਿਖਰ - 1);
  *b = 'x';

  ਪੂਰਨ ret = unlink(b);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("unlink(%s) returned %d, not -1\n", b, ret);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਪੂਰਨ fd = open(b, O_CREATE | O_WRONLY);
  ਜੇ (fd != -1) {
    ਲਿਖੋ("open(%s) returned %d, not -1\n", b, fd);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ret = link(b, b);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("link(%s, %s) returned %d, not -1\n", b, b, ret);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਅੱਖਰ *ਦਲੀਲਾਂ[] = {"xx", 0};
  ret = exec(b, ਦਲੀਲਾਂ);
  ਜੇ (ret != -1) {
    ਲਿਖੋ("exec(%s) returned %d, not -1\n", b, fd);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
ਖਾਲੀ
rwsbrk(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, n;

  uint64 a = (uint64)sbrk(8192);

  ਜੇ (a == (uint64)SBRK_ERROR) {
    ਲਿਖੋ("sbrk(rwsbrk) failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (sbrk(-8192) == SBRK_ERROR) {
    ਲਿਖੋ("sbrk(rwsbrk) shrink failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("rwsbrk", O_CREATE | O_WRONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("open(rwsbrk) failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  n = ਲਿਖ(fd, (ਖਾਲੀ *)(a + PGSIZE), 1024);
  ਜੇ (n >= 0) {
    ਲਿਖੋ("write(fd, %p, 1024) returned %d, not -1\n", (ਖਾਲੀ *)a + PGSIZE, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  unlink("rwsbrk");

  fd = open("README", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("open(README) failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  n = ਪਠ(fd, (ਖਾਲੀ *)(a + PGSIZE), 10);
  ਜੇ (n >= 0) {
    ਲਿਖੋ("read(fd, %p, 10) returned %d, not -1\n", (ਖਾਲੀ *)a + PGSIZE, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// test O_TRUNC.
ਖਾਲੀ
truncate1(ਅੱਖਰ *s)
{
  ਅੱਖਰ buf[32];

  unlink("truncfile");
  ਪੂਰਨ fd1 = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  ਲਿਖ(fd1, "abcd", 4);
  close(fd1);

  ਪੂਰਨ fd2 = open("truncfile", O_RDONLY);
  ਪੂਰਨ n = ਪਠ(fd2, buf, ਆਕਾਰ(buf));
  ਜੇ (n != 4) {
    ਲਿਖੋ("%s: read %d bytes, wanted 4\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd1 = open("truncfile", O_WRONLY | O_TRUNC);

  ਪੂਰਨ fd3 = open("truncfile", O_RDONLY);
  n = ਪਠ(fd3, buf, ਆਕਾਰ(buf));
  ਜੇ (n != 0) {
    ਲਿਖੋ("aaa fd3=%d\n", fd3);
    ਲਿਖੋ("%s: read %d bytes, wanted 0\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  n = ਪਠ(fd2, buf, ਆਕਾਰ(buf));
  ਜੇ (n != 0) {
    ਲਿਖੋ("bbb fd2=%d\n", fd2);
    ਲਿਖੋ("%s: read %d bytes, wanted 0\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਿਖ(fd1, "abcdef", 6);

  n = ਪਠ(fd3, buf, ਆਕਾਰ(buf));
  ਜੇ (n != 6) {
    ਲਿਖੋ("%s: read %d bytes, wanted 6\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  n = ਪਠ(fd2, buf, ਆਕਾਰ(buf));
  ਜੇ (n != 2) {
    ਲਿਖੋ("%s: read %d bytes, wanted 2\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
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
ਖਾਲੀ
truncate2(ਅੱਖਰ *s)
{
  unlink("truncfile");

  ਪੂਰਨ fd1 = open("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  ਲਿਖ(fd1, "abcd", 4);

  ਪੂਰਨ fd2 = open("truncfile", O_TRUNC | O_WRONLY);

  ਪੂਰਨ n = ਲਿਖ(fd1, "x", 1);
  ਜੇ (n != -1) {
    ਲਿਖੋ("%s: write returned %d, expected -1\n", s, n);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  unlink("truncfile");
  close(fd1);
  close(fd2);
}

ਖਾਲੀ
truncate3(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, xstatus;

  close(open("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (pid == 0) {
    ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
      ਅੱਖਰ buf[32];
      ਪੂਰਨ fd = open("truncfile", O_WRONLY);
      ਜੇ (fd < 0) {
        ਲਿਖੋ("%s: open failed\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਪੂਰਨ n = ਲਿਖ(fd, "1234567890", 10);
      ਜੇ (n != 10) {
        ਲਿਖੋ("%s: write got %d, expected 10\n", s, n);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      close(fd);
      fd = open("truncfile", O_RDONLY);
      ਪਠ(fd, buf, ਆਕਾਰ(buf));
      close(fd);
    }
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਲਈ (ਪੂਰਨ i = 0; i < 150; i++) {
    ਪੂਰਨ fd = open("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("%s: open failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ n = ਲਿਖ(fd, "xxx", 3);
    ਜੇ (n != 3) {
      ਲਿਖੋ("%s: write got %d, expected 3\n", s, n);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fd);
  }

  wait(&xstatus);
  unlink("truncfile");
  ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
ਖਾਲੀ
iputtest(ਅੱਖਰ *s)
{
  ਜੇ (mkdir("iputdir") < 0) {
    ਲਿਖੋ("%s: mkdir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("iputdir") < 0) {
    ਲਿਖੋ("%s: chdir iputdir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("../iputdir") < 0) {
    ਲਿਖੋ("%s: unlink ../iputdir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("/") < 0) {
    ਲਿਖੋ("%s: chdir / failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
ਖਾਲੀ
exitiputtest(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, xstatus;

  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਜੇ (mkdir("iputdir") < 0) {
      ਲਿਖੋ("%s: mkdir failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (chdir("iputdir") < 0) {
      ਲਿਖੋ("%s: child chdir failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (unlink("../iputdir") < 0) {
      ਲਿਖੋ("%s: unlink ../iputdir failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  wait(&xstatus);
  ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
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
ਖਾਲੀ
openiputtest(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, xstatus;

  ਜੇ (mkdir("oidir") < 0) {
    ਲਿਖੋ("%s: mkdir oidir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਪੂਰਨ fd = open("oidir", O_RDWR);
    ਜੇ (fd >= 0) {
      ਲਿਖੋ("%s: open directory for write succeeded\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  pause(1);
  ਜੇ (unlink("oidir") != 0) {
    ਲਿਖੋ("%s: unlink failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  wait(&xstatus);
  ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
}

// simple file system tests

ਖਾਲੀ
opentest(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd;

  fd = open("echo", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open echo failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  fd = open("doesnotexist", 0);
  ਜੇ (fd >= 0) {
    ਲਿਖੋ("%s: open doesnotexist succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
writetest(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd;
  ਪੂਰਨ i;
  ਗਿਣਤੀ { N = 100, SZ = 10 };

  fd = open("small", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: error: creat small failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਈ (i = 0; i < N; i++) {
    ਜੇ (ਲਿਖ(fd, "aaaaaaaaaa", SZ) != SZ) {
      ਲਿਖੋ("%s: error: write aa %d new file failed\n", s, i);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (ਲਿਖ(fd, "bbbbbbbbbb", SZ) != SZ) {
      ਲਿਖੋ("%s: error: write bb %d new file failed\n", s, i);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: error: open small failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  i = ਪਠ(fd, buf, N * SZ * 2);
  ਜੇ (i != N * SZ * 2) {
    ਲਿਖੋ("%s: read failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (unlink("small") < 0) {
    ਲਿਖੋ("%s: unlink small failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
writebig(ਅੱਖਰ *s)
{
  ਪੂਰਨ i, fd, n;

  fd = open("big", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: error: creat big failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਈ (i = 0; i < MAXFILE; i++) {
    ((ਪੂਰਨ *)buf)[0] = i;
    ਜੇ (ਲਿਖ(fd, buf, BSIZE) != BSIZE) {
      ਲਿਖੋ("%s: error: write big file failed i=%d\n", s, i);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: error: open big failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  n = 0;
  ਲਈ (;;) {
    i = ਪਠ(fd, buf, BSIZE);
    ਜੇ (i == 0) {
      ਜੇ (n != MAXFILE) {
        ਲਿਖੋ("%s: read only %d blocks from big", s, n);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਰੋਕੋ;
    } ਨਹੀਂਤਾਂ ਜੇ (i != BSIZE) {
      ਲਿਖੋ("%s: read failed %d\n", s, i);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (((ਪੂਰਨ *)buf)[0] != n) {
      ਲਿਖੋ("%s: read content of block %d is %d\n", s, n, ((ਪੂਰਨ *)buf)[0]);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    n++;
  }
  close(fd);
  ਜੇ (unlink("big") < 0) {
    ਲਿਖੋ("%s: unlink big failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// many creates, followed by unlink test
ਖਾਲੀ
createtest(ਅੱਖਰ *s)
{
  ਪੂਰਨ i, fd;
  ਗਿਣਤੀ { N = 52 };

  ਅੱਖਰ name[3];
  name[0] = 'a';
  name[2] = '\0';
  ਲਈ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  ਲਈ (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

ਖਾਲੀ
dirtest(ਅੱਖਰ *s)
{
  ਜੇ (mkdir("dir0") < 0) {
    ਲਿਖੋ("%s: mkdir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (chdir("dir0") < 0) {
    ਲਿਖੋ("%s: chdir dir0 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (chdir("..") < 0) {
    ਲਿਖੋ("%s: chdir .. failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (unlink("dir0") < 0) {
    ਲਿਖੋ("%s: unlink dir0 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
exectest(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, xstatus, pid;
  ਅੱਖਰ *echoargv[] = {"echo", "OK", 0};
  ਅੱਖਰ buf[3];

  unlink("echo-ok");
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਪੂਰਨ errfd = dup(1);
    ਜੇ (errfd < 0) {
      ਲਿਖੋ("%s: dup failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(1);
    fd = open("echo-ok", O_CREATE | O_WRONLY);
    ਜੇ (fd < 0) {
      fprintf(errfd, "%s: create failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (fd != 1) {
      fprintf(errfd, "%s: wrong fd\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (exec("echo", echoargv) < 0) {
      fprintf(errfd, "%s: exec echo failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    // won't get to here
  }
  ਜੇ (wait(&xstatus) != pid) {
    ਲਿਖੋ("%s: wait failed!\n", s);
  }
  ਜੇ (xstatus != 0) {
    ਲਿਖੋ("%s: nonzero wait status %d\n", s, xstatus);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("echo-ok", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਪਠ(fd, buf, 2) != 2) {
    ਲਿਖੋ("%s: read failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  unlink("echo-ok");
  ਜੇ (buf[0] == 'O' && buf[1] == 'K')
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  ਨਹੀਂਤਾਂ {
    ਲਿਖੋ("%s: wrong output\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// simple fork and pipe read/write

ਖਾਲੀ
pipe1(ਅੱਖਰ *s)
{
  ਪੂਰਨ fds[2], pid, xstatus;
  ਪੂਰਨ seq, i, n, cc, ਕੁੱਲ;
  ਗਿਣਤੀ { N = 5, SZ = 1033 };

  ਜੇ (pipe(fds) != 0) {
    ਲਿਖੋ("%s: pipe() failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  pid = fork();
  seq = 0;
  ਜੇ (pid == 0) {
    close(fds[0]);
    ਲਈ (n = 0; n < N; n++) {
      ਲਈ (i = 0; i < SZ; i++)
        buf[i] = seq++;
      ਜੇ (ਲਿਖ(fds[1], buf, SZ) != SZ) {
        ਲਿਖੋ("%s: pipe1 oops 1\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    }
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ ਜੇ (pid > 0) {
    close(fds[1]);
    ਕੁੱਲ = 0;
    cc = 1;
    ਜਦੋਂ ((n = ਪਠ(fds[0], buf, cc)) > 0) {
      ਲਈ (i = 0; i < n; i++) {
        ਜੇ ((buf[i] & 0xff) != (seq++ & 0xff)) {
          ਲਿਖੋ("%s: pipe1 oops 2\n", s);
          ਵਾਪਸ;
        }
      }
      ਕੁੱਲ += n;
      cc = cc * 2;
      ਜੇ (cc > ਆਕਾਰ(buf))
        cc = ਆਕਾਰ(buf);
    }
    ਜੇ (ਕੁੱਲ != N * SZ) {
      ਲਿਖੋ("%s: pipe1 oops 3 total %d\n", s, ਕੁੱਲ);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fds[0]);
    wait(&xstatus);
    ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  } ਨਹੀਂਤਾਂ {
    ਲਿਖੋ("%s: fork() failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// test if child is killed (status = -1)
ਖਾਲੀ
killstatus(ਅੱਖਰ *s)
{
  ਪੂਰਨ xst;

  ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
    ਪੂਰਨ pid1 = fork();
    ਜੇ (pid1 < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid1 == 0) {
      ਜਦੋਂ (1) {
        getpid();
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
    pause(1);
    kill(pid1);
    wait(&xst);
    ਜੇ (xst != -1) {
      ਲਿਖੋ("%s: status should be -1\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// test that kill on unused process 0 doesn't set the killed flag
ਖਾਲੀ
killzero(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, xst;

  kill(0);

  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਬਾਹਰ_ਨਿਕਲੋ(7);
  }
  ਜੇ (wait(&xst) != pid) {
    ਲਿਖੋ("%s: wait wrong pid\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (xst != 7) {
    ਲਿਖੋ("%s: child exited with status %d, expected 7\n", s, xst);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// meant to be run w/ at most two CPUs
ਖਾਲੀ
preempt(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid1, pid2, pid3;
  ਪੂਰਨ pfds[2];

  pid1 = fork();
  ਜੇ (pid1 < 0) {
    ਲਿਖੋ("%s: fork failed", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid1 == 0)
    ਲਈ (;;)
      ;

  pid2 = fork();
  ਜੇ (pid2 < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid2 == 0)
    ਲਈ (;;)
      ;

  pipe(pfds);
  pid3 = fork();
  ਜੇ (pid3 < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid3 == 0) {
    close(pfds[0]);
    ਜੇ (ਲਿਖ(pfds[1], "x", 1) != 1)
      ਲਿਖੋ("%s: preempt write error", s);
    close(pfds[1]);
    ਲਈ (;;)
      ;
  }

  close(pfds[1]);
  ਜੇ (ਪਠ(pfds[0], buf, ਆਕਾਰ(buf)) != 1) {
    ਲਿਖੋ("%s: preempt read error", s);
    ਵਾਪਸ;
  }
  close(pfds[0]);
  ਲਿਖੋ("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  ਲਿਖੋ("wait... ");
  wait(0);
  wait(0);
  wait(0);
}

// try to find any races between exit and wait
ਖਾਲੀ
exitwait(ਅੱਖਰ *s)
{
  ਪੂਰਨ i, pid;

  ਲਈ (i = 0; i < 100; i++) {
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid) {
      ਪੂਰਨ xstate;
      ਜੇ (wait(&xstate) != pid) {
        ਲਿਖੋ("%s: wait wrong pid\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (i != xstate) {
        ਲਿਖੋ("%s: wait wrong exit status\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    } ਨਹੀਂਤਾਂ {
      ਬਾਹਰ_ਨਿਕਲੋ(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
ਖਾਲੀ
reparent(ਅੱਖਰ *s)
{
  ਪੂਰਨ master_pid = getpid();
  ਲਈ (ਪੂਰਨ i = 0; i < 200; i++) {
    ਪੂਰਨ pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid) {
      ਜੇ (wait(0) != pid) {
        ਲਿਖੋ("%s: wait wrong pid\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    } ਨਹੀਂਤਾਂ {
      ਪੂਰਨ pid2 = fork();
      ਜੇ (pid2 < 0) {
        kill(master_pid);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// what if two children exit() at the same time?
ਖਾਲੀ
twochildren(ਅੱਖਰ *s)
{
  ਲਈ (ਪੂਰਨ i = 0; i < 1000; i++) {
    ਪੂਰਨ pid1 = fork();
    ਜੇ (pid1 < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid1 == 0) {
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    } ਨਹੀਂਤਾਂ {
      ਪੂਰਨ pid2 = fork();
      ਜੇ (pid2 < 0) {
        ਲਿਖੋ("%s: fork failed\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (pid2 == 0) {
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ {
        wait(0);
        wait(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
ਖਾਲੀ
forkfork(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 2 };

  ਲਈ (ਪੂਰਨ i = 0; i < N; i++) {
    ਪੂਰਨ pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid == 0) {
      ਲਈ (ਪੂਰਨ j = 0; j < 200; j++) {
        ਪੂਰਨ pid1 = fork();
        ਜੇ (pid1 < 0) {
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
        ਜੇ (pid1 == 0) {
          ਬਾਹਰ_ਨਿਕਲੋ(0);
        }
        wait(0);
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }

  ਪੂਰਨ xstatus;
  ਲਈ (ਪੂਰਨ i = 0; i < N; i++) {
    wait(&xstatus);
    ਜੇ (xstatus != 0) {
      ਲਿਖੋ("%s: fork in child failed", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
}

ਖਾਲੀ
forkforkfork(ਅੱਖਰ *s)
{
  unlink("stopforking");

  ਪੂਰਨ pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਜਦੋਂ (1) {
      ਪੂਰਨ fd = open("stopforking", 0);
      ਜੇ (fd >= 0) {
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      }
      ਜੇ (fork() < 0) {
        close(open("stopforking", O_CREATE | O_RDWR));
      }
    }

    ਬਾਹਰ_ਨਿਕਲੋ(0);
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
ਖਾਲੀ
reparent2(ਅੱਖਰ *s)
{
  ਲਈ (ਪੂਰਨ i = 0; i < 800; i++) {
    ਪੂਰਨ pid1 = fork();
    ਜੇ (pid1 < 0) {
      ਲਿਖੋ("fork failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid1 == 0) {
      fork();
      fork();
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
    wait(0);
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// allocate all mem, free it, and allocate again
ਖਾਲੀ
mem(ਅੱਖਰ *s)
{
  ਖਾਲੀ *m1, *m2;
  ਪੂਰਨ pid;

  ਜੇ ((pid = fork()) == 0) {
    m1 = 0;
    ਜਦੋਂ ((m2 = ਸਮ੍ਰਿਤੀ_ਦਿਓ(10001)) != 0) {
      *(ਅੱਖਰ **)m2 = m1;
      m1 = m2;
    }
    ਜਦੋਂ (m1) {
      m2 = *(ਅੱਖਰ **)m1;
      ਮੁਕਤ(m1);
      m1 = m2;
    }
    m1 = ਸਮ੍ਰਿਤੀ_ਦਿਓ(1024 * 20);
    ਜੇ (m1 == 0) {
      ਲਿਖੋ("%s: couldn't allocate mem?!!\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਮੁਕਤ(m1);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ {
    ਪੂਰਨ xstatus;
    wait(&xstatus);
    ਜੇ (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
    ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
ਖਾਲੀ
sharedfd(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, pid, i, n, nc, np;
  ਗਿਣਤੀ { N = 1000, SZ = 10 };
  ਅੱਖਰ buf[SZ];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot open sharedfd for writing", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  pid = fork();
  memset(buf, pid == 0 ? 'c' : 'p', ਆਕਾਰ(buf));
  ਲਈ (i = 0; i < N; i++) {
    ਜੇ (ਲਿਖ(fd, buf, ਆਕਾਰ(buf)) != ਆਕਾਰ(buf)) {
      ਲਿਖੋ("%s: write sharedfd failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  ਜੇ (pid == 0) {
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ {
    ਪੂਰਨ xstatus;
    wait(&xstatus);
    ਜੇ (xstatus != 0)
      ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  }

  close(fd);
  fd = open("sharedfd", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot open sharedfd for reading\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  nc = np = 0;
  ਜਦੋਂ ((n = ਪਠ(fd, buf, ਆਕਾਰ(buf))) > 0) {
    ਲਈ (i = 0; i < ਆਕਾਰ(buf); i++) {
      ਜੇ (buf[i] == 'c')
        nc++;
      ਜੇ (buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  unlink("sharedfd");
  ਜੇ (nc == N * SZ && np == N * SZ) {
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ {
    ਲਿਖੋ("%s: nc/np test fails\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
ਖਾਲੀ
fourfiles(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, pid, i, j, n, ਕੁੱਲ, pi;
  ਅੱਖਰ *ਨਾਂ[] = {"f0", "f1", "f2", "f3"};
  ਅੱਖਰ *fname;
  ਗਿਣਤੀ { N = 12, NCHILD = 4, SZ = 500 };

  ਲਈ (pi = 0; pi < NCHILD; pi++) {
    fname = ਨਾਂ[pi];
    unlink(fname);

    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (pid == 0) {
      fd = open(fname, O_CREATE | O_RDWR);
      ਜੇ (fd < 0) {
        ਲਿਖੋ("%s: create failed\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }

      memset(buf, '0' + pi, SZ);
      ਲਈ (i = 0; i < N; i++) {
        ਜੇ ((n = ਲਿਖ(fd, buf, SZ)) != SZ) {
          ਲਿਖੋ("write failed %d\n", n);
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }

  ਪੂਰਨ xstatus;
  ਲਈ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ਜੇ (xstatus != 0)
      ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  }

  ਲਈ (i = 0; i < NCHILD; i++) {
    fname = ਨਾਂ[i];
    fd = open(fname, 0);
    ਕੁੱਲ = 0;
    ਜਦੋਂ ((n = ਪਠ(fd, buf, ਆਕਾਰ(buf))) > 0) {
      ਲਈ (j = 0; j < n; j++) {
        ਜੇ (buf[j] != '0' + i) {
          ਲਿਖੋ("%s: wrong char\n", s);
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
      }
      ਕੁੱਲ += n;
    }
    close(fd);
    ਜੇ (ਕੁੱਲ != N * SZ) {
      ਲਿਖੋ("wrong length %d\n", ਕੁੱਲ);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
ਖਾਲੀ
createdelete(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 20, NCHILD = 4 };
  ਪੂਰਨ pid, i, fd, pi;
  ਅੱਖਰ name[32];

  ਲਈ (pi = 0; pi < NCHILD; pi++) {
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      ਲਈ (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        ਜੇ (fd < 0) {
          ਲਿਖੋ("%s: create failed\n", s);
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
        close(fd);
        ਜੇ (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          ਜੇ (unlink(name) < 0) {
            ਲਿਖੋ("%s: unlink failed\n", s);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
          }
        }
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }

  ਪੂਰਨ xstatus;
  ਲਈ (pi = 0; pi < NCHILD; pi++) {
    wait(&xstatus);
    ਜੇ (xstatus != 0)
      ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  name[0] = name[1] = name[2] = 0;
  ਲਈ (i = 0; i < N; i++) {
    ਲਈ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      ਜੇ ((i == 0 || i >= N / 2) && fd < 0) {
        ਲਿਖੋ("%s: oops createdelete %s didn't exist\n", s, name);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      } ਨਹੀਂਤਾਂ ਜੇ ((i >= 1 && i < N / 2) && fd >= 0) {
        ਲਿਖੋ("%s: oops createdelete %s did exist\n", s, name);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (fd >= 0)
        close(fd);
    }
  }

  ਲਈ (i = 0; i < N; i++) {
    ਲਈ (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
ਖਾਲੀ
unlinkread(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { SZ = 5 };
  ਪੂਰਨ fd, fd1;

  fd = open("unlinkread", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: create unlinkread failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖ(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open unlinkread failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("unlinkread") != 0) {
    ਲਿਖੋ("%s: unlink unlinkread failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  ਲਿਖ(fd1, "yyy", 3);
  close(fd1);

  ਜੇ (ਪਠ(fd, buf, ਆਕਾਰ(buf)) != SZ) {
    ਲਿਖੋ("%s: unlinkread read failed", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (buf[0] != 'h') {
    ਲਿਖੋ("%s: unlinkread wrong data\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਲਿਖ(fd, buf, 10) != 10) {
    ਲਿਖੋ("%s: unlinkread write failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  unlink("unlinkread");
}

ਖਾਲੀ
linktest(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { SZ = 5 };
  ਪੂਰਨ fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: create lf1 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਲਿਖ(fd, "hello", SZ) != SZ) {
    ਲਿਖੋ("%s: write lf1 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (link("lf1", "lf2") < 0) {
    ਲਿਖੋ("%s: link lf1 lf2 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  unlink("lf1");

  ਜੇ (open("lf1", 0) >= 0) {
    ਲਿਖੋ("%s: unlinked lf1 but it is still there!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("lf2", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open lf2 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਪਠ(fd, buf, ਆਕਾਰ(buf)) != SZ) {
    ਲਿਖੋ("%s: read lf2 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (link("lf2", "lf2") >= 0) {
    ਲਿਖੋ("%s: link lf2 lf2 succeeded! oops\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  unlink("lf2");
  ਜੇ (link("lf2", "lf1") >= 0) {
    ਲਿਖੋ("%s: link non-existent succeeded! oops\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (link(".", "lf1") >= 0) {
    ਲਿਖੋ("%s: link . lf1 succeeded! oops\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// test concurrent create/link/unlink of the same file
ਖਾਲੀ
concreate(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 40 };
  ਅੱਖਰ file[3];
  ਪੂਰਨ i, pid, n, fd;
  ਅੱਖਰ fa[N];
  ਬਣਤਰ {
    ushort inum;
    ਅੱਖਰ name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  ਲਈ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    ਜੇ (pid && (i % 3) == 1) {
      link("C0", file);
    } ਨਹੀਂਤਾਂ ਜੇ (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } ਨਹੀਂਤਾਂ {
      fd = open(file, O_CREATE | O_RDWR);
      ਜੇ (fd < 0) {
        ਲਿਖੋ("concreate create %s failed\n", file);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      close(fd);
    }
    ਜੇ (pid == 0) {
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    } ਨਹੀਂਤਾਂ {
      ਪੂਰਨ xstatus;
      wait(&xstatus);
      ਜੇ (xstatus != 0)
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  memset(fa, 0, ਆਕਾਰ(fa));
  fd = open(".", 0);
  n = 0;
  ਜਦੋਂ (ਪਠ(fd, &de, ਆਕਾਰ(de)) > 0) {
    ਜੇ (de.inum == 0)
      ਜਾਰੀ;
    ਜੇ (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      ਜੇ (i < 0 || i >= ਆਕਾਰ(fa)) {
        ਲਿਖੋ("%s: concreate weird file %s\n", s, de.name);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (fa[i]) {
        ਲਿਖੋ("%s: concreate duplicate file %s\n", s, de.name);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  ਜੇ (n != N) {
    ਲਿਖੋ("%s: concreate not enough files in directory listing\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਈ (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } ਨਹੀਂਤਾਂ {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    ਜੇ (pid == 0)
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    ਨਹੀਂਤਾਂ
      wait(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
ਖਾਲੀ
linkunlink(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, i;

  unlink("x");
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ x = (pid ? 1 : 97);
  ਲਈ (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    ਜੇ ((x % 3) == 0) {
      close(open("x", O_RDWR | O_CREATE));
    } ਨਹੀਂਤਾਂ ਜੇ ((x % 3) == 1) {
      link("cat", "x");
    } ਨਹੀਂਤਾਂ {
      unlink("x");
    }
  }

  ਜੇ (pid)
    wait(0);
  ਨਹੀਂਤਾਂ
    ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
subdir(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, cc;

  unlink("ff");
  ਜੇ (mkdir("dd") != 0) {
    ਲਿਖੋ("%s: mkdir dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: create dd/ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖ(fd, "ff", 2);
  close(fd);

  ਜੇ (unlink("dd") >= 0) {
    ਲਿਖੋ("%s: unlink dd (non-empty dir) succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (mkdir("/dd/dd") != 0) {
    ਲਿਖੋ("%s: subdir mkdir dd/dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: create dd/dd/ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖ(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open dd/dd/../ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  cc = ਪਠ(fd, buf, ਆਕਾਰ(buf));
  ਜੇ (cc != 2 || buf[0] != 'f') {
    ਲਿਖੋ("%s: dd/dd/../ff wrong content\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    ਲਿਖੋ("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (unlink("dd/dd/ff") != 0) {
    ਲਿਖੋ("%s: unlink dd/dd/ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("dd/dd/ff", O_RDONLY) >= 0) {
    ਲਿਖੋ("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (chdir("dd") != 0) {
    ਲਿਖੋ("%s: chdir dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("dd/../../dd") != 0) {
    ਲਿਖੋ("%s: chdir dd/../../dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("dd/../../../dd") != 0) {
    ਲਿਖੋ("%s: chdir dd/../../../dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("./..") != 0) {
    ਲਿਖੋ("%s: chdir ./.. failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("dd/dd/ffff", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open dd/dd/ffff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਪਠ(fd, buf, ਆਕਾਰ(buf)) != 2) {
    ਲਿਖੋ("%s: read dd/dd/ffff wrong len\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (open("dd/dd/ff", O_RDONLY) >= 0) {
    ਲਿਖੋ("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (open("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    ਲਿਖੋ("%s: create dd/ff/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    ਲਿਖੋ("%s: create dd/xx/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("dd", O_CREATE) >= 0) {
    ਲਿਖੋ("%s: create dd succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("dd", O_RDWR) >= 0) {
    ਲਿਖੋ("%s: open dd rdwr succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("dd", O_WRONLY) >= 0) {
    ਲਿਖੋ("%s: open dd wronly succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (link("dd/ff/ff", "dd/dd/xx") == 0) {
    ਲਿਖੋ("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (link("dd/xx/ff", "dd/dd/xx") == 0) {
    ਲਿਖੋ("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (link("dd/ff", "dd/dd/ffff") == 0) {
    ਲਿਖੋ("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("dd/ff/ff") == 0) {
    ਲਿਖੋ("%s: mkdir dd/ff/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("dd/xx/ff") == 0) {
    ਲਿਖੋ("%s: mkdir dd/xx/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("dd/dd/ffff") == 0) {
    ਲਿਖੋ("%s: mkdir dd/dd/ffff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd/xx/ff") == 0) {
    ਲਿਖੋ("%s: unlink dd/xx/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd/ff/ff") == 0) {
    ਲਿਖੋ("%s: unlink dd/ff/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("dd/ff") == 0) {
    ਲਿਖੋ("%s: chdir dd/ff succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("dd/xx") == 0) {
    ਲਿਖੋ("%s: chdir dd/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (unlink("dd/dd/ffff") != 0) {
    ਲਿਖੋ("%s: unlink dd/dd/ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd/ff") != 0) {
    ਲਿਖੋ("%s: unlink dd/ff failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd") == 0) {
    ਲਿਖੋ("%s: unlink non-empty dd succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd/dd") < 0) {
    ਲਿਖੋ("%s: unlink dd/dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dd") < 0) {
    ਲਿਖੋ("%s: unlink dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// test writes that are larger than the log.
ਖਾਲੀ
bigwrite(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd, sz;

  unlink("bigwrite");
  ਲਈ (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = open("bigwrite", O_CREATE | O_RDWR);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("%s: cannot create bigwrite\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ i;
    ਲਈ (i = 0; i < 2; i++) {
      ਪੂਰਨ cc = ਲਿਖ(fd, buf, sz);
      ਜੇ (cc != sz) {
        ਲਿਖੋ("%s: write(%d) ret %d\n", s, sz, cc);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    }
    close(fd);
    unlink("bigwrite");
  }
}

ਖਾਲੀ
bigfile(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 20, SZ = 600 };
  ਪੂਰਨ fd, i, ਕੁੱਲ, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot create bigfile", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਈ (i = 0; i < N; i++) {
    memset(buf, i, SZ);
    ਜੇ (ਲਿਖ(fd, buf, SZ) != SZ) {
      ਲਿਖੋ("%s: write bigfile failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot open bigfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਕੁੱਲ = 0;
  ਲਈ (i = 0;; i++) {
    cc = ਪਠ(fd, buf, SZ / 2);
    ਜੇ (cc < 0) {
      ਲਿਖੋ("%s: read bigfile failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (cc == 0)
      ਰੋਕੋ;
    ਜੇ (cc != SZ / 2) {
      ਲਿਖੋ("%s: short read bigfile\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      ਲਿਖੋ("%s: read bigfile wrong data\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਕੁੱਲ += cc;
  }
  close(fd);
  ਜੇ (ਕੁੱਲ != N * SZ) {
    ਲਿਖੋ("%s: read bigfile wrong total\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  unlink("bigfile.dat");
}

ਖਾਲੀ
fourteen(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd;

  // DIRSIZ is 14.

  ਜੇ (mkdir("12345678901234") != 0) {
    ਲਿਖੋ("%s: mkdir 12345678901234 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("12345678901234/123456789012345") != 0) {
    ਲਿਖੋ("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  ਜੇ (fd < 0) {
    ਲਿਖੋ(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਜੇ (mkdir("12345678901234/12345678901234") == 0) {
    ਲਿਖੋ("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("123456789012345/12345678901234") == 0) {
    ਲਿਖੋ("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  // clean up
  unlink("123456789012345/12345678901234");
  unlink("12345678901234/12345678901234");
  unlink("12345678901234/12345678901234/12345678901234");
  unlink("123456789012345/123456789012345/123456789012345");
  unlink("12345678901234/123456789012345");
  unlink("12345678901234");
}

ਖਾਲੀ
rmdot(ਅੱਖਰ *s)
{
  ਜੇ (mkdir("dots") != 0) {
    ਲਿਖੋ("%s: mkdir dots failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("dots") != 0) {
    ਲਿਖੋ("%s: chdir dots failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink(".") == 0) {
    ਲਿਖੋ("%s: rm . worked!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("..") == 0) {
    ਲਿਖੋ("%s: rm .. worked!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("/") != 0) {
    ਲਿਖੋ("%s: chdir / failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dots/.") == 0) {
    ਲਿਖੋ("%s: unlink dots/. worked!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dots/..") == 0) {
    ਲਿਖੋ("%s: unlink dots/.. worked!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dots") != 0) {
    ਲਿਖੋ("%s: unlink dots failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
dirfile(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd;

  fd = open("dirfile", O_CREATE);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: create dirfile failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  ਜੇ (chdir("dirfile") == 0) {
    ਲਿਖੋ("%s: chdir dirfile succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  fd = open("dirfile/xx", 0);
  ਜੇ (fd >= 0) {
    ਲਿਖੋ("%s: create dirfile/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  fd = open("dirfile/xx", O_CREATE);
  ਜੇ (fd >= 0) {
    ਲਿਖੋ("%s: create dirfile/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("dirfile/xx") == 0) {
    ਲਿਖੋ("%s: mkdir dirfile/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dirfile/xx") == 0) {
    ਲਿਖੋ("%s: unlink dirfile/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (link("README", "dirfile/xx") == 0) {
    ਲਿਖੋ("%s: link to dirfile/xx succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("dirfile") != 0) {
    ਲਿਖੋ("%s: unlink dirfile failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open(".", O_RDWR);
  ਜੇ (fd >= 0) {
    ਲਿਖੋ("%s: open . for writing succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  fd = open(".", 0);
  ਜੇ (ਲਿਖ(fd, "x", 1) > 0) {
    ਲਿਖੋ("%s: write . succeeded!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
ਖਾਲੀ
iref(ਅੱਖਰ *s)
{
  ਪੂਰਨ i, fd;

  ਲਈ (i = 0; i < NINODE + 1; i++) {
    ਜੇ (mkdir("irefd") != 0) {
      ਲਿਖੋ("%s: mkdir irefd failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (chdir("irefd") != 0) {
      ਲਿਖੋ("%s: chdir irefd failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    ਜੇ (fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    ਜੇ (fd >= 0)
      close(fd);
    unlink("xx");
  }

  // clean up
  ਲਈ (i = 0; i < NINODE + 1; i++) {
    chdir("..");
    unlink("irefd");
  }

  chdir("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
ਖਾਲੀ
forktest(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 1000 };
  ਪੂਰਨ n, pid;

  ਲਈ (n = 0; n < N; n++) {
    pid = fork();
    ਜੇ (pid < 0)
      ਰੋਕੋ;
    ਜੇ (pid == 0)
      ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਜੇ (n == 0) {
    ਲਿਖੋ("%s: no fork at all!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (n == N) {
    ਲਿਖੋ("%s: fork claimed to work 1000 times!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਈ (; n > 0; n--) {
    ਜੇ (wait(0) < 0) {
      ਲਿਖੋ("%s: wait stopped early\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  ਜੇ (wait(0) != -1) {
    ਲਿਖੋ("%s: wait got too many\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
sbrkbasic(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { TOOMUCH = 1024 * 1024 * 1024 };
  ਪੂਰਨ i, pid, xstatus;
  ਅੱਖਰ *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("fork failed in sbrkbasic\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    a = sbrk(TOOMUCH);
    ਜੇ (a == (ਅੱਖਰ *)SBRK_ERROR) {
      // it's OK if this fails.
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }

    ਲਈ (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  wait(&xstatus);
  ਜੇ (xstatus == 1) {
    ਲਿਖੋ("%s: too much memory allocated!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  ਲਈ (i = 0; i < 5000; i++) {
    b = sbrk(1);
    ਜੇ (b != a) {
      ਲਿਖੋ("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: sbrk test fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  ਜੇ (c != a + 1) {
    ਲਿਖੋ("%s: sbrk test failed post-fork\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0)
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  wait(&xstatus);
  ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
}

ਖਾਲੀ
sbrkmuch(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { BIG = 100 * 1024 * 1024 };
  ਅੱਖਰ *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  ਜੇ (p != a) {
    ਲਿਖੋ("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  lastaddr = (ਅੱਖਰ *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  ਜੇ (c == (ਅੱਖਰ *)SBRK_ERROR) {
    ਲਿਖੋ("%s: sbrk could not deallocate\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  c = sbrk(0);
  ਜੇ (c != a - PGSIZE) {
    ਲਿਖੋ("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  ਜੇ (c != a || sbrk(0) != a + PGSIZE) {
    ਲਿਖੋ("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (*lastaddr == 99) {
    // should be zero
    ਲਿਖੋ("%s: sbrk de-allocation didn't really deallocate\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  ਜੇ (c != a) {
    ਲਿਖੋ("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// can we read the kernel's memory?
ਖਾਲੀ
kernmem(ਅੱਖਰ *s)
{
  ਅੱਖਰ *a;
  ਪੂਰਨ pid;

  ਲਈ (a = (ਅੱਖਰ *)(KERNBASE); a < (ਅੱਖਰ *)(KERNBASE + 2000000); a += 50000) {
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid == 0) {
      ਲਿਖੋ("%s: oops could read %p = %x\n", s, a, *a);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ xstatus;
    wait(&xstatus);
    ਜੇ (xstatus != -1) // did kernel kill child?
      ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
ਖਾਲੀ
MAXVAplus(ਅੱਖਰ *s)
{
  ਅਸਥਿਰ uint64 a = MAXVA;
  ਲਈ (; a != 0; a <<= 1) {
    ਪੂਰਨ pid;
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid == 0) {
      *(ਅੱਖਰ *)a = 99;
      ਲਿਖੋ("%s: oops wrote %p\n", s, (ਖਾਲੀ *)a);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਪੂਰਨ xstatus;
    wait(&xstatus);
    ਜੇ (xstatus != -1) // did kernel kill child?
      ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
ਖਾਲੀ
sbrkfail(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { BIG = 100 * 1024 * 1024 };
  ਪੂਰਨ i, xstatus;
  ਪੂਰਨ fds[2];
  ਅੱਖਰ scratch;
  ਅੱਖਰ *c, *a;
  ਪੂਰਨ pids[10];
  ਪੂਰਨ pid;
  ਪੂਰਨ failed;

  failed = 0;
  ਜੇ (pipe(fds) != 0) {
    ਲਿਖੋ("%s: pipe() failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਈ (i = 0; i < ਆਕਾਰ(pids) / ਆਕਾਰ(pids[0]); i++) {
    ਜੇ ((pids[i] = fork()) == 0) {
      // allocate a lot of memory
      ਜੇ (sbrk(BIG - (uint64)sbrk(0)) == (ਅੱਖਰ *)SBRK_ERROR)
        ਲਿਖ(fds[1], "0", 1);
      ਨਹੀਂਤਾਂ
        ਲਿਖ(fds[1], "1", 1);
      // sit around until killed
      ਲਈ (;;)
        pause(1000);
    }
    ਜੇ (pids[i] != -1) {
      ਪਠ(fds[0], &scratch, 1);
      ਜੇ (scratch == '0')
        failed = 1;
    }
  }
  ਜੇ (!failed) {
    ਲਿਖੋ("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  ਲਈ (i = 0; i < ਆਕਾਰ(pids) / ਆਕਾਰ(pids[0]); i++) {
    ਜੇ (pids[i] == -1)
      ਜਾਰੀ;
    kill(pids[i]);
    wait(0);
  }
  ਜੇ (c == (ਅੱਖਰ *)SBRK_ERROR) {
    ਲਿਖੋ("%s: failed sbrk leaked memory\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  // test running fork with the above allocated page
  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    ਜੇ (a == (ਅੱਖਰ *)SBRK_ERROR) {
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
    ਲਿਖੋ("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  wait(&xstatus);
  ਜੇ (xstatus != 0)
    ਬਾਹਰ_ਨਿਕਲੋ(1);
}

// test reads/writes from/to allocated memory
ਖਾਲੀ
sbrkarg(ਅੱਖਰ *s)
{
  ਅੱਖਰ *a;
  ਪੂਰਨ fd, n;

  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open sbrk failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ ((n = ਲਿਖ(fd, a, PGSIZE)) < 0) {
    ਲਿਖੋ("%s: write sbrk failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  ਜੇ (pipe((ਪੂਰਨ *)a) != 0) {
    ਲਿਖੋ("%s: pipe() failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਖਾਲੀ
validatetest(ਅੱਖਰ *s)
{
  ਪੂਰਨ hi;
  uint64 p;

  hi = 1100 * 1024;
  ਲਈ (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    ਜੇ (link("nosuchfile", (ਅੱਖਰ *)p) != -1) {
      ਲਿਖੋ("%s: link should not succeed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
}

// does uninitialized data start out zero?
ਅੱਖਰ uninit[10000];
ਖਾਲੀ
bsstest(ਅੱਖਰ *s)
{
  ਪੂਰਨ i;

  ਲਈ (i = 0; i < ਆਕਾਰ(uninit); i++) {
    ਜੇ (uninit[i] != '\0') {
      ਲਿਖੋ("%s: bss test failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
ਖਾਲੀ
bigargtest(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = fork();
  ਜੇ (pid == 0) {
    ਸਥਿਰ ਅੱਖਰ *ਦਲੀਲਾਂ[MAXARG];
    ਪੂਰਨ i;
    ਅੱਖਰ big[400];
    memset(big, ' ', ਆਕਾਰ(big));
    big[ਆਕਾਰ(big) - 1] = '\0';
    ਲਈ (i = 0; i < MAXARG - 1; i++)
      ਦਲੀਲਾਂ[i] = big;
    ਦਲੀਲਾਂ[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", ਦਲੀਲਾਂ);
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: bigargtest: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  wait(&xstatus);
  ਜੇ (xstatus != 0)
    ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  fd = open("bigarg-ok", 0);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: bigarg test failed!\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
ਖਾਲੀ
fsfull()
{
  ਪੂਰਨ nfiles;
  ਪੂਰਨ fsblocks = 0;

  ਲਿਖੋ("fsfull test\n");

  ਲਈ (nfiles = 0;; nfiles++) {
    ਅੱਖਰ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    ਲਿਖੋ("writing %s\n", name);
    ਪੂਰਨ fd = open(name, O_CREATE | O_RDWR);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("open %s failed\n", name);
      ਰੋਕੋ;
    }
    ਪੂਰਨ ਕੁੱਲ = 0;
    ਜਦੋਂ (1) {
      ਪੂਰਨ cc = ਲਿਖ(fd, buf, BSIZE);
      ਜੇ (cc < BSIZE)
        ਰੋਕੋ;
      ਕੁੱਲ += cc;
      fsblocks++;
    }
    ਲਿਖੋ("wrote %d bytes\n", ਕੁੱਲ);
    close(fd);
    ਜੇ (ਕੁੱਲ == 0)
      ਰੋਕੋ;
  }

  ਜਦੋਂ (nfiles >= 0) {
    ਅੱਖਰ name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  ਲਿਖੋ("fsfull test finished, %d blocks\n", fsblocks);
}

ਖਾਲੀ
argptest(ਅੱਖਰ *s)
{
  ਪੂਰਨ fd;
  fd = open("init", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: open failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਪਠ(fd, sbrk(0) - 1, -1);
  close(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
ਖਾਲੀ
stacktest(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid;
  ਪੂਰਨ xstatus;

  pid = fork();
  ਜੇ (pid == 0) {
    ਅੱਖਰ *sp = (ਅੱਖਰ *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    ਲਿਖੋ("%s: stacktest: read below stack %d\n", s, *sp);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
    ਲਿਖੋ("%s: fork failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  wait(&xstatus);
  ਜੇ (xstatus == -1) // kernel killed child?
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  ਨਹੀਂਤਾਂ
    ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
ਖਾਲੀ
nowrite(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid;
  ਪੂਰਨ xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  ਲਈ (ਪੂਰਨ ai = 0; ai < ਆਕਾਰ(addrs) / ਆਕਾਰ(addrs[0]); ai++) {
    pid = fork();
    ਜੇ (pid == 0) {
      ਅਸਥਿਰ ਪੂਰਨ *addr = (ਪੂਰਨ *)addrs[ai];
      *addr = 10;
      ਲਿਖੋ("%s: write to %p did not fail!\n", s, addr);
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
      ਲਿਖੋ("%s: fork failed\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    wait(&xstatus);
    ਜੇ (xstatus == 0) {
      // kernel did not kill child!
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
ਖਾਲੀ *big = (ਖਾਲੀ *)0xeaeb0b5b00002f5e;
ਖਾਲੀ
pgbug(ਅੱਖਰ *s)
{
  ਅੱਖਰ *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
ਖਾਲੀ
sbrkbugs(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਪੂਰਨ sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  wait(0);

  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    ਪੂਰਨ sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  wait(0);

  pid = fork();
  ਜੇ (pid < 0) {
    ਲਿਖੋ("fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  wait(0);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
ਖਾਲੀ
sbrklast(ਅੱਖਰ *s)
{
  uint64 ਸਿਖਰ = (uint64)sbrk(0);
  ਜੇ ((ਸਿਖਰ % PGSIZE) != 0)
    sbrk(PGSIZE - (ਸਿਖਰ % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  ਸਿਖਰ = (uint64)sbrk(0);
  ਅੱਖਰ *p = (ਅੱਖਰ *)(ਸਿਖਰ - 64);
  p[0] = 'x';
  p[1] = '\0';
  ਪੂਰਨ fd = open(p, O_RDWR | O_CREATE);
  ਲਿਖ(fd, p, 1);
  close(fd);
  fd = open(p, O_RDWR);
  p[0] = '\0';
  ਪਠ(fd, p, 1);
  ਜੇ (p[0] != 'x')
    ਬਾਹਰ_ਨਿਕਲੋ(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
ਖਾਲੀ
sbrk8000(ਅੱਖਰ *s)
{
  sbrk(0x80000004);
  ਅਸਥਿਰ ਅੱਖਰ *ਸਿਖਰ = sbrk(0);
  *(ਸਿਖਰ - 1) = *(ਸਿਖਰ - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
ਖਾਲੀ
badarg(ਅੱਖਰ *s)
{
  ਲਈ (ਪੂਰਨ i = 0; i < 50000; i++) {
    ਅੱਖਰ *argv[2];
    argv[0] = (ਅੱਖਰ *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
ਖਾਲੀ
lazy_alloc(ਅੱਖਰ *s)
{
  ਅੱਖਰ *i, *prev_ਸਿਰਾ, *new_ਸਿਰਾ;

  prev_ਸਿਰਾ = sbrklazy(REGION_SZ);
  ਜੇ (prev_ਸਿਰਾ == (ਅੱਖਰ *)SBRK_ERROR) {
    ਲਿਖੋ("sbrklazy() failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  new_ਸਿਰਾ = prev_ਸਿਰਾ + REGION_SZ;

  ਲਈ (i = prev_ਸਿਰਾ + PGSIZE; i < new_ਸਿਰਾ; i += 64 * PGSIZE)
    *(ਅੱਖਰ **)i = i;

  ਲਈ (i = prev_ਸਿਰਾ + PGSIZE; i < new_ਸਿਰਾ; i += 64 * PGSIZE) {
    ਜੇ (*(ਅੱਖਰ **)i != i) {
      ਲਿਖੋ("failed to read value from memory\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
ਖਾਲੀ
lazy_unmap(ਅੱਖਰ *s)
{
  ਪੂਰਨ pid;
  ਅੱਖਰ *i, *prev_ਸਿਰਾ, *new_ਸਿਰਾ;

  prev_ਸਿਰਾ = sbrklazy(REGION_SZ);
  ਜੇ (prev_ਸਿਰਾ == (ਅੱਖਰ *)SBRK_ERROR) {
    ਲਿਖੋ("sbrklazy() failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  new_ਸਿਰਾ = prev_ਸਿਰਾ + REGION_SZ;

  ਲਈ (i = prev_ਸਿਰਾ + PGSIZE; i < new_ਸਿਰਾ; i += PGSIZE * PGSIZE)
    *(ਅੱਖਰ **)i = i;

  ਲਈ (i = prev_ਸਿਰਾ + PGSIZE; i < new_ਸਿਰਾ; i += PGSIZE * PGSIZE) {
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("error forking\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    } ਨਹੀਂਤਾਂ ਜੇ (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(ਅੱਖਰ **)i = i;
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    } ਨਹੀਂਤਾਂ {
      ਪੂਰਨ ਸਥਿਤੀ;
      wait(&ਸਥਿਤੀ);
      ਜੇ (ਸਥਿਤੀ == 0) {
        ਲਿਖੋ("memory not unmapped\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    }
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
lazy_copy(ਅੱਖਰ *s)
{
  // copyinstr on lazy page
  {
    ਅੱਖਰ *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    open(p + 8192, 0);
  }

  {
    ਖਾਲੀ *xx = sbrk(0);
    ਖਾਲੀ *ret = sbrk(-(((uint64)xx) + 1));
    ਜੇ (ret != xx) {
      ਲਿਖੋ("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  // read() and write() to these addresses should fail.
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  ਲਈ (ਪੂਰਨ i = 0; i < ਆਕਾਰ(bad) / ਆਕਾਰ(bad[0]); i++) {
    ਪੂਰਨ fd = open("README", 0);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("cannot open README\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (ਪਠ(fd, (ਅੱਖਰ *)bad[i], 512) >= 0) {
      ਲਿਖੋ("read succeeded\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fd);
    fd = open("junk", O_CREATE | O_RDWR | O_TRUNC);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("cannot open junk\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (ਲਿਖ(fd, (ਅੱਖਰ *)bad[i], 512) >= 0) {
      ਲਿਖੋ("write succeeded\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    close(fd);
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
lazy_copyinstr(ਅੱਖਰ *s)
{
  ਅੱਖਰ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ਜੇ ((uint64)p % PGSIZE != 0) {
    ਲਿਖੋ("%s: sbrk did not align\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  ਪੂਰਨ fd = open(&p[4095], O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("could not open /");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਬਣਤਰ stat st;
  ਪੂਰਨ r = fstat(fd, &st);
  ਜੇ (r < 0) {
    ਲਿਖੋ("could not stat /");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (st.type != T_DIR) {
    ਲਿਖੋ("/ is not T_DIR");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  close(fd);
}

ਖਾਲੀ
lazy_sbrk(ਅੱਖਰ *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  ਅੱਖਰ *p = sbrk(0);
  ਜਦੋਂ ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    ਜੇ (p < 0) {
      ਲਿਖੋ("sbrklazy(%d) returned %p\n", 1 << 30, p);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    p = sbrklazy(0);
  }

  ਪੂਰਨ n = TRAPFRAME - PGSIZE - (uint64)p;

  ਅੱਖਰ *p1 = sbrklazy(n);
  ਜੇ (p1 < 0 || p1 != p) {
    ਲਿਖੋ("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  p = sbrk(PGSIZE);
  ਜੇ (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    ਲਿਖੋ("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  p[0] = 1;
  ਜੇ (p[1] != 0) {
    ਲਿਖੋ("sbrk() returned non-zero-filled memory\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  p = sbrk(1);
  ਜੇ ((uint64)p != -1) {
    ਲਿਖੋ("sbrk(1) returned %p, expected error\n", p);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  p = sbrklazy(1);
  ਜੇ ((uint64)p != -1) {
    ਲਿਖੋ("sbrklazy(1) returned %p, expected error\n", p);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
partial_write(ਅੱਖਰ *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  ਪੂਰਨ fd = open("testfile", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot create testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਪੂਰਨ cc = ਲਿਖ(fd, "A", 1);
  ਜੇ (cc != 1) {
    ਲਿਖੋ("%s: could not write A\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  close(fd);
  fd = open("testfile", O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot re-open testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਅੱਖਰ *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  ਜੇ ((uint64)p % PGSIZE != 0) {
    ਲਿਖੋ("%s: sbrk did not align\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  p[-1] = 'X';

  cc = ਲਿਖ(fd, p - 1, 2);
  ਜੇ (cc != -1) {
    ਲਿਖੋ("%s: write succeeded, should have failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  close(fd);

  fd = open("testfile", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot re-open testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਅੱਖਰ b;
  cc = ਪਠ(fd, &b, 1);
  ਜੇ (cc != 1) {
    ਲਿਖੋ("%s: cannot read testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  close(fd);

  ਜੇ (b != 'X') {
    ਲਿਖੋ("%s: read returned %c, expected X\n", s, b);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  fd = open("bigfile", O_CREATE | O_RDWR);
  ਲਈ (ਪੂਰਨ i = 0; i < 64; i++) {
    ਅੱਖਰ buf[1024];
    memset(buf, 0, ਆਕਾਰ(buf));
    cc = ਲਿਖ(fd, buf, ਆਕਾਰ(buf));
    ਜੇ (cc != ਆਕਾਰ(buf)) {
      ਲਿਖੋ("%s: could not write to bigfile\n", s);
      ਬਾਹਰ_ਨਿਕਲੋ(-1);
    }
  }
  close(fd);

  unlink("bigfile");

  fd = open("testfile", O_RDONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot re-open testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  cc = ਪਠ(fd, &b, 1);
  ਜੇ (cc != 1) {
    ਲਿਖੋ("%s: cannot read testfile\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  close(fd);

  ਜੇ (b != 'X') {
    ਲਿਖੋ("%s: read returned %c, expected X\n", s, b);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  unlink("testfile");
}

ਖਾਲੀ
unlinkcwd(ਅੱਖਰ *s)
{
  ਜੇ (mkdir("/a") < 0) {
    ਲਿਖੋ("%s: mkdir /a failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (mkdir("/a/b") < 0) {
    ਲਿਖੋ("%s: mkdir /a/b failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (chdir("/a/b") < 0) {
    ਲਿਖੋ("%s: chdir failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("/a/b") < 0) {
    ਲਿਖੋ("%s: unlink /a/b failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink("/a") < 0) {
    ਲਿਖੋ("%s: unlink /a failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open("../", O_RDONLY) > 0) {
    ਲਿਖੋ("%s: open ../ non-existing directory\n", s);
  }
  ਜੇ (open("../c", O_CREATE) > 0) {
    ਲਿਖੋ("%s: create ../c non-existing file\n", s);
  }
}

ਬਣਤਰ test {
  ਖਾਲੀ (*f)(ਅੱਖਰ *);
  ਅੱਖਰ *s;
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
ਖਾਲੀ
bigdir(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { N = 500 };
  ਪੂਰਨ i, fd;
  ਅੱਖਰ name[10];

  unlink("bd");

  fd = open("bd", O_CREATE);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: bigdir create failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਲਈ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ਜੇ (link("bd", name) != 0) {
      ਲਿਖੋ("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  unlink("bd");
  ਲਈ (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    ਜੇ (unlink(name) != 0) {
      ਲਿਖੋ("%s: bigdir unlink failed", s);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
ਖਾਲੀ
manywrites(ਅੱਖਰ *s)
{
  ਪੂਰਨ nchildren = 4;
  ਪੂਰਨ howmany = 30; // increase to look for deadlock

  ਲਈ (ਪੂਰਨ ci = 0; ci < nchildren; ci++) {
    ਪੂਰਨ pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("fork failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (pid == 0) {
      ਅੱਖਰ name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      ਲਈ (ਪੂਰਨ iters = 0; iters < howmany; iters++) {
        ਲਈ (ਪੂਰਨ i = 0; i < ci + 1; i++) {
          ਪੂਰਨ fd = open(name, O_CREATE | O_RDWR);
          ਜੇ (fd < 0) {
            ਲਿਖੋ("%s: cannot create %s\n", s, name);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
          }
          ਪੂਰਨ sz = ਆਕਾਰ(buf);
          ਪੂਰਨ cc = ਲਿਖ(fd, buf, sz);
          ਜੇ (cc != sz) {
            ਲਿਖੋ("%s: write(%d) ret %d\n", s, sz, cc);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
          }
          close(fd);
        }
        unlink(name);
      }

      unlink(name);
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }

  ਲਈ (ਪੂਰਨ ci = 0; ci < nchildren; ci++) {
    ਪੂਰਨ st = 0;
    wait(&st);
    ਜੇ (st != 0)
      ਬਾਹਰ_ਨਿਕਲੋ(st);
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
ਖਾਲੀ
badwrite(ਅੱਖਰ *s)
{
  ਪੂਰਨ assumed_free = 600;

  unlink("junk");
  ਲਈ (ਪੂਰਨ i = 0; i < assumed_free; i++) {
    ਪੂਰਨ fd = open("junk", O_CREATE | O_WRONLY);
    ਜੇ (fd < 0) {
      ਲਿਖੋ("open junk failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਲਿਖ(fd, (ਅੱਖਰ *)0xffffffffffL, 1);
    close(fd);
    unlink("junk");
  }

  ਪੂਰਨ fd = open("junk", O_CREATE | O_WRONLY);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("open junk failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (ਲਿਖ(fd, "x", 1) != 1) {
    ਲਿਖੋ("write failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);
  unlink("junk");

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
ਖਾਲੀ
execout(ਅੱਖਰ *s)
{
  ਲਈ (ਪੂਰਨ avail = 0; avail < 15; avail++) {
    ਪੂਰਨ pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("fork failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    } ਨਹੀਂਤਾਂ ਜੇ (pid == 0) {
      // allocate all of memory.
      ਜਦੋਂ (1) {
        ਅੱਖਰ *a = sbrk(PGSIZE);
        ਜੇ (a == SBRK_ERROR)
          ਰੋਕੋ;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      ਲਈ (ਪੂਰਨ i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      close(1);
      ਅੱਖਰ *ਦਲੀਲਾਂ[] = {"echo", "x", 0};
      exec("echo", ਦਲੀਲਾਂ);
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    } ਨਹੀਂਤਾਂ {
      wait((ਪੂਰਨ *)0);
    }
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// can the kernel tolerate running out of disk space?
ਖਾਲੀ
diskfull(ਅੱਖਰ *s)
{
  ਪੂਰਨ fi;
  ਪੂਰਨ ਪੂਰਾ = 0;

  unlink("diskfulldir");

  ਲਈ (fi = 0; ਪੂਰਾ == 0 && '0' + fi < 0177; fi++) {
    ਅੱਖਰ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    ਪੂਰਨ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ਜੇ (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      ਲਿਖੋ("%s: could not create file %s\n", s, name);
      ਪੂਰਾ = 1;
      ਰੋਕੋ;
    }
    ਲਈ (ਪੂਰਨ i = 0; i < MAXFILE; i++) {
      ਅੱਖਰ buf[BSIZE];
      ਜੇ (ਲਿਖ(fd, buf, BSIZE) != BSIZE) {
        ਪੂਰਾ = 1;
        close(fd);
        ਰੋਕੋ;
      }
    }
    close(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  ਪੂਰਨ nzz = 128;
  ਲਈ (ਪੂਰਨ i = 0; i < nzz; i++) {
    ਅੱਖਰ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ਪੂਰਨ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ਜੇ (fd < 0)
      ਰੋਕੋ;
    close(fd);
  }

  // this mkdir() is expected to fail.
  ਜੇ (mkdir("diskfulldir") == 0)
    ਲਿਖੋ("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  ਲਈ (ਪੂਰਨ i = 0; i < nzz; i++) {
    ਅੱਖਰ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  ਲਈ (ਪੂਰਨ i = 0; '0' + i < 0177; i++) {
    ਅੱਖਰ name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

ਖਾਲੀ
outofinodes(ਅੱਖਰ *s)
{
  ਪੂਰਨ nzz = 32 * 32;
  ਲਈ (ਪੂਰਨ i = 0; i < nzz; i++) {
    ਅੱਖਰ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    ਪੂਰਨ fd = open(name, O_CREATE | O_RDWR | O_TRUNC);
    ਜੇ (fd < 0) {
      // failure is eventually expected.
      ਰੋਕੋ;
    }
    close(fd);
  }

  ਲਈ (ਪੂਰਨ i = 0; i < nzz; i++) {
    ਅੱਖਰ name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

ਖਾਲੀ
linkoverflow(ਅੱਖਰ *s)
{
  ਗਿਣਤੀ { ਨਿਸ਼ਾਨਾ_2 = 32768 };
  ਗਿਣਤੀ { DIRS = 64 };
  ਬਣਤਰ stat st;
  ਪੂਰਨ i;

  unlink("/lof");
  ਪੂਰਨ fd = open("/lof", O_CREATE | O_RDWR);
  ਜੇ (fd < 0) {
    ਲਿਖੋ("%s: cannot create /lof\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  close(fd);

  ਲਈ (i = 0; i < ਨਿਸ਼ਾਨਾ_2; i++) {
    ਪੂਰਨ d = i % DIRS;
    ਪੂਰਨ f = i / DIRS;

    ਅੱਖਰ pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    ਜੇ (f == 0 && mkdir(pn) < 0) {
      ਲਿਖੋ("%s: mkdir(%s) failed\n", s, pn);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    ਜੇ (link("/lof", pn) < 0) {
      ਜੇ (stat("/lof", &st) < 0) {
        ਲਿਖੋ("%s: stat(/lof) failed\n", s);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (st.nlink >= 32767) {
        // overflow check succeeded.
        ਰੋਕੋ;
      }
      ਲਿਖੋ("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (i % 100 == 0) {
      ਲਿਖੋ("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  ਜੇ (stat("/lof", &st) < 0) {
    ਲਿਖੋ("%s: stat(/lof) failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  unlink("/lof");

  ਜੇ (st.nlink < 0) {
    ਲਿਖੋ("%s: negative link count: %d\n", s, st.nlink);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਬਣਤਰ test slowtests[] = {
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
ਪੂਰਨ
ਲੜੀਵਾਰ(ਖਾਲੀ f(ਅੱਖਰ *), ਅੱਖਰ *s)
{
  ਪੂਰਨ pid;
  ਪੂਰਨ xstatus;

  ਲਿਖੋ("test %s: ", s);
  ਜੇ ((pid = fork()) < 0) {
    ਲਿਖੋ("runtest: fork error\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid == 0) {
    f(s);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  } ਨਹੀਂਤਾਂ {
    wait(&xstatus);
    ਜੇ (xstatus != 0)
      ਲਿਖੋ("FAILED\n");
    ਨਹੀਂਤਾਂ
      ਲਿਖੋ("OK\n");
    ਵਾਪਸ xstatus == 0;
  }
}

ਪੂਰਨ
runtests(ਬਣਤਰ test *tests, ਅੱਖਰ *justone, ਪੂਰਨ continuous)
{
  ਪੂਰਨ ntests = 0;
  ਲਈ (ਬਣਤਰ test *t = tests; t->s != 0; t++) {
    ਜੇ ((justone == 0) || strcmp(t->s, justone) == 0) {
      ntests++;
      ਜੇ (!ਲੜੀਵਾਰ(t->f, t->s)) {
        ਜੇ (continuous != 2) {
          ਲਿਖੋ("SOME TESTS FAILED\n");
          ਵਾਪਸ -1;
        }
      }
    }
  }
  ਵਾਪਸ ntests;
}

// use sbrk() to count how many free physical memory pages there are.
ਪੂਰਨ
countfree()
{
  ਪੂਰਨ n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  ਜਦੋਂ (1) {
    ਅੱਖਰ *a = sbrk(PGSIZE);
    ਜੇ (a == SBRK_ERROR) {
      ਰੋਕੋ;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  ਵਾਪਸ n;
}

ਪੂਰਨ
drivetests(ਪੂਰਨ ਤੇਜ਼, ਪੂਰਨ continuous, ਅੱਖਰ *justone)
{
  ਕਰੋ {
    ਲਿਖੋ("usertests starting\n");
    ਪੂਰਨ free0 = countfree();
    ਪੂਰਨ free1 = 0;
    ਪੂਰਨ ntests = 0;
    ਪੂਰਨ n;
    n = runtests(quicktests, justone, continuous);
    ਜੇ (n < 0) {
      ਜੇ (continuous != 2) {
        ਵਾਪਸ 1;
      }
    } ਨਹੀਂਤਾਂ {
      ntests += n;
    }
    ਜੇ (!ਤੇਜ਼) {
      ਜੇ (justone == 0)
        ਲਿਖੋ("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      ਜੇ (n < 0) {
        ਜੇ (continuous != 2) {
          ਵਾਪਸ 1;
        }
      } ਨਹੀਂਤਾਂ {
        ntests += n;
      }
    }
    ਜੇ ((free1 = countfree()) < free0) {
      ਲਿਖੋ("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      ਜੇ (continuous != 2) {
        ਵਾਪਸ 1;
      }
    }
    ਜੇ (justone != 0 && ntests == 0) {
      ਲਿਖੋ("NO TESTS EXECUTED\n");
      ਵਾਪਸ 1;
    }
  } ਜਦੋਂ (continuous);
  ਵਾਪਸ 0;
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ continuous = 0;
  ਪੂਰਨ ਤੇਜ਼ = 0;
  ਅੱਖਰ *justone = 0;

  ਜੇ (argc == 2 && strcmp(argv[1], "-q") == 0) {
    ਤੇਜ਼ = 1;
  } ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
  } ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && strcmp(argv[1], "-C") == 0) {
    continuous = 2;
  } ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } ਨਹੀਂਤਾਂ ਜੇ (argc > 1) {
    ਲਿਖੋ("Usage: usertests [-c] [-C] [-q] [testname]\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (drivetests(ਤੇਜ਼, continuous, justone)) {
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖੋ("ALL TESTS PASSED\n");
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
