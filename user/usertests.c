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

अक्षर buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
शून्य
copyin(अक्षर *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  साठी (पूर्णांक ai = 0; ai < आकार(addrs) / आकार(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णांक fd = खोलो("copyin1", O_CREATE | O_WRONLY);
    जर (fd < 0) {
      म_लिखो("open(copyin1) failed\n");
      निकास(1);
    }
    पूर्णांक n = लिख(fd, (शून्य *)addr, 8192);
    जर (n >= 0) {
      म_लिखो("write(fd, %p, 8192) returned %d, not -1\n", (शून्य *)addr, n);
      निकास(1);
    }
    बंद(fd);
    unlink("copyin1");

    n = लिख(1, (अक्षर *)addr, 8192);
    जर (n > 0) {
      म_लिखो("write(1, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      निकास(1);
    }

    पूर्णांक fds[2];
    जर (pipe(fds) < 0) {
      म_लिखो("pipe() failed\n");
      निकास(1);
    }
    n = लिख(fds[1], (अक्षर *)addr, 8192);
    जर (n > 0) {
      म_लिखो("write(pipe, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr,
             n);
      निकास(1);
    }
    बंद(fds[0]);
    बंद(fds[1]);
  }
}

// what if you pass ridiculous pointers to system calls
// that write user memory with copyout?
शून्य
copyout(अक्षर *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  साठी (पूर्णांक ai = 0; ai < आकार(addrs) / आकार(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णांक fd = खोलो("README", 0);
    जर (fd < 0) {
      म_लिखो("open(README) failed\n");
      निकास(1);
    }
    पूर्णांक n = पठ(fd, (शून्य *)addr, 8192);
    जर (n > 0) {
      म_लिखो("read(fd, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      निकास(1);
    }
    बंद(fd);

    पूर्णांक fds[2];
    जर (pipe(fds) < 0) {
      म_लिखो("pipe() failed\n");
      निकास(1);
    }
    n = लिख(fds[1], "x", 1);
    जर (n != 1) {
      म_लिखो("pipe write failed\n");
      निकास(1);
    }
    n = पठ(fds[0], (शून्य *)addr, 8192);
    जर (n > 0) {
      म_लिखो("read(pipe, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr,
             n);
      निकास(1);
    }
    बंद(fds[0]);
    बंद(fds[1]);
  }
}

// what if you pass ridiculous string pointers to system calls?
शून्य
copyinstr1(अक्षर *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  साठी (पूर्णांक ai = 0; ai < आकार(addrs) / आकार(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णांक fd = खोलो((अक्षर *)addr, O_CREATE | O_WRONLY);
    जर (fd >= 0) {
      म_लिखो("open(%p) returned %d, not -1\n", (शून्य *)addr, fd);
      निकास(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
शून्य
copyinstr2(अक्षर *s)
{
  अक्षर b[MAXPATH + 1];

  साठी (पूर्णांक i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  पूर्णांक ret = unlink(b);
  जर (ret != -1) {
    म_लिखो("unlink(%s) returned %d, not -1\n", b, ret);
    निकास(1);
  }

  पूर्णांक fd = खोलो(b, O_CREATE | O_WRONLY);
  जर (fd != -1) {
    म_लिखो("open(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  ret = link(b, b);
  जर (ret != -1) {
    म_लिखो("link(%s, %s) returned %d, not -1\n", b, b, ret);
    निकास(1);
  }

  अक्षर *args[] = {"xx", 0};
  ret = exec(b, args);
  जर (ret != -1) {
    म_लिखो("exec(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  पूर्णांक pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  जर (pid == 0) {
    स्थिर अक्षर big[PGSIZE + 1];
    साठी (पूर्णांक i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    अक्षर *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    जर (ret != -1) {
      म_लिखो("exec(echo, BIG) returned %d, not -1\n", fd);
      निकास(1);
    }
    निकास(747); // OK
  }

  पूर्णांक st = 0;
  रुको(&st);
  जर (st != 747) {
    म_लिखो("exec(echo, BIG) succeeded, should have failed\n");
    निकास(1);
  }
}

// what if a string argument crosses over the end of last user page?
शून्य
copyinstr3(अक्षर *s)
{
  sbrk(8192);
  uint64 शीर्ष = (uint64)sbrk(0);
  जर ((शीर्ष % PGSIZE) != 0) {
    sbrk(PGSIZE - (शीर्ष % PGSIZE));
  }
  शीर्ष = (uint64)sbrk(0);
  जर (शीर्ष % PGSIZE) {
    म_लिखो("oops\n");
    निकास(1);
  }

  अक्षर *b = (अक्षर *)(शीर्ष - 1);
  *b = 'x';

  पूर्णांक ret = unlink(b);
  जर (ret != -1) {
    म_लिखो("unlink(%s) returned %d, not -1\n", b, ret);
    निकास(1);
  }

  पूर्णांक fd = खोलो(b, O_CREATE | O_WRONLY);
  जर (fd != -1) {
    म_लिखो("open(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  ret = link(b, b);
  जर (ret != -1) {
    म_लिखो("link(%s, %s) returned %d, not -1\n", b, b, ret);
    निकास(1);
  }

  अक्षर *args[] = {"xx", 0};
  ret = exec(b, args);
  जर (ret != -1) {
    म_लिखो("exec(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
शून्य
rwsbrk(अक्षर *s)
{
  पूर्णांक fd, n;

  uint64 a = (uint64)sbrk(8192);

  जर (a == (uint64)SBRK_ERROR) {
    म_लिखो("sbrk(rwsbrk) failed\n");
    निकास(1);
  }

  जर (sbrk(-8192) == SBRK_ERROR) {
    म_लिखो("sbrk(rwsbrk) shrink failed\n");
    निकास(1);
  }

  fd = खोलो("rwsbrk", O_CREATE | O_WRONLY);
  जर (fd < 0) {
    म_लिखो("open(rwsbrk) failed\n");
    निकास(1);
  }
  n = लिख(fd, (शून्य *)(a + PGSIZE), 1024);
  जर (n >= 0) {
    म_लिखो("write(fd, %p, 1024) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    निकास(1);
  }
  बंद(fd);
  unlink("rwsbrk");

  fd = खोलो("README", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("open(README) failed\n");
    निकास(1);
  }
  n = पठ(fd, (शून्य *)(a + PGSIZE), 10);
  जर (n >= 0) {
    म_लिखो("read(fd, %p, 10) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    निकास(1);
  }
  बंद(fd);

  निकास(0);
}

// test O_TRUNC.
शून्य
truncate1(अक्षर *s)
{
  अक्षर buf[32];

  unlink("truncfile");
  पूर्णांक fd1 = खोलो("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  लिख(fd1, "abcd", 4);
  बंद(fd1);

  पूर्णांक fd2 = खोलो("truncfile", O_RDONLY);
  पूर्णांक n = पठ(fd2, buf, आकार(buf));
  जर (n != 4) {
    म_लिखो("%s: read %d bytes, wanted 4\n", s, n);
    निकास(1);
  }

  fd1 = खोलो("truncfile", O_WRONLY | O_TRUNC);

  पूर्णांक fd3 = खोलो("truncfile", O_RDONLY);
  n = पठ(fd3, buf, आकार(buf));
  जर (n != 0) {
    म_लिखो("aaa fd3=%d\n", fd3);
    म_लिखो("%s: read %d bytes, wanted 0\n", s, n);
    निकास(1);
  }

  n = पठ(fd2, buf, आकार(buf));
  जर (n != 0) {
    म_लिखो("bbb fd2=%d\n", fd2);
    म_लिखो("%s: read %d bytes, wanted 0\n", s, n);
    निकास(1);
  }

  लिख(fd1, "abcdef", 6);

  n = पठ(fd3, buf, आकार(buf));
  जर (n != 6) {
    म_लिखो("%s: read %d bytes, wanted 6\n", s, n);
    निकास(1);
  }

  n = पठ(fd2, buf, आकार(buf));
  जर (n != 2) {
    म_लिखो("%s: read %d bytes, wanted 2\n", s, n);
    निकास(1);
  }

  unlink("truncfile");

  बंद(fd1);
  बंद(fd2);
  बंद(fd3);
}

// write to an open FD whose file has just been truncated.
// this causes a write at an offset beyond the end of the file.
// such writes fail on xv6 (unlike POSIX) but at least
// they don't crash.
शून्य
truncate2(अक्षर *s)
{
  unlink("truncfile");

  पूर्णांक fd1 = खोलो("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  लिख(fd1, "abcd", 4);

  पूर्णांक fd2 = खोलो("truncfile", O_TRUNC | O_WRONLY);

  पूर्णांक n = लिख(fd1, "x", 1);
  जर (n != -1) {
    म_लिखो("%s: write returned %d, expected -1\n", s, n);
    निकास(1);
  }

  unlink("truncfile");
  बंद(fd1);
  बंद(fd2);
}

शून्य
truncate3(अक्षर *s)
{
  पूर्णांक pid, xstatus;

  बंद(खोलो("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }

  जर (pid == 0) {
    साठी (पूर्णांक i = 0; i < 100; i++) {
      अक्षर buf[32];
      पूर्णांक fd = खोलो("truncfile", O_WRONLY);
      जर (fd < 0) {
        म_लिखो("%s: open failed\n", s);
        निकास(1);
      }
      पूर्णांक n = लिख(fd, "1234567890", 10);
      जर (n != 10) {
        म_लिखो("%s: write got %d, expected 10\n", s, n);
        निकास(1);
      }
      बंद(fd);
      fd = खोलो("truncfile", O_RDONLY);
      पठ(fd, buf, आकार(buf));
      बंद(fd);
    }
    निकास(0);
  }

  साठी (पूर्णांक i = 0; i < 150; i++) {
    पूर्णांक fd = खोलो("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    जर (fd < 0) {
      म_लिखो("%s: open failed\n", s);
      निकास(1);
    }
    पूर्णांक n = लिख(fd, "xxx", 3);
    जर (n != 3) {
      म_लिखो("%s: write got %d, expected 3\n", s, n);
      निकास(1);
    }
    बंद(fd);
  }

  रुको(&xstatus);
  unlink("truncfile");
  निकास(xstatus);
}

// does chdir() call iput(p->cwd) in a transaction?
शून्य
iputtest(अक्षर *s)
{
  जर (सूची_गढ़ो("iputdir") < 0) {
    म_लिखो("%s: mkdir failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("iputdir") < 0) {
    म_लिखो("%s: chdir iputdir failed\n", s);
    निकास(1);
  }
  जर (unlink("../iputdir") < 0) {
    म_लिखो("%s: unlink ../iputdir failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("/") < 0) {
    म_लिखो("%s: chdir / failed\n", s);
    निकास(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
शून्य
exitiputtest(अक्षर *s)
{
  पूर्णांक pid, xstatus;

  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid == 0) {
    जर (सूची_गढ़ो("iputdir") < 0) {
      म_लिखो("%s: mkdir failed\n", s);
      निकास(1);
    }
    जर (स_बदलो("iputdir") < 0) {
      म_लिखो("%s: child chdir failed\n", s);
      निकास(1);
    }
    जर (unlink("../iputdir") < 0) {
      म_लिखो("%s: unlink ../iputdir failed\n", s);
      निकास(1);
    }
    निकास(0);
  }
  रुको(&xstatus);
  निकास(xstatus);
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
openiputtest(अक्षर *s)
{
  पूर्णांक pid, xstatus;

  जर (सूची_गढ़ो("oidir") < 0) {
    म_लिखो("%s: mkdir oidir failed\n", s);
    निकास(1);
  }
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid == 0) {
    पूर्णांक fd = खोलो("oidir", O_RDWR);
    जर (fd >= 0) {
      म_लिखो("%s: open directory for write succeeded\n", s);
      निकास(1);
    }
    निकास(0);
  }
  छोड़ो(1);
  जर (unlink("oidir") != 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  रुको(&xstatus);
  निकास(xstatus);
}

// simple file system tests

शून्य
opentest(अक्षर *s)
{
  पूर्णांक fd;

  fd = खोलो("echo", 0);
  जर (fd < 0) {
    म_लिखो("%s: open echo failed!\n", s);
    निकास(1);
  }
  बंद(fd);
  fd = खोलो("doesnotexist", 0);
  जर (fd >= 0) {
    म_लिखो("%s: open doesnotexist succeeded!\n", s);
    निकास(1);
  }
}

शून्य
writetest(अक्षर *s)
{
  पूर्णांक fd;
  पूर्णांक i;
  गणना { N = 100, SZ = 10 };

  fd = खोलो("small", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: error: creat small failed!\n", s);
    निकास(1);
  }
  साठी (i = 0; i < N; i++) {
    जर (लिख(fd, "aaaaaaaaaa", SZ) != SZ) {
      म_लिखो("%s: error: write aa %d new file failed\n", s, i);
      निकास(1);
    }
    जर (लिख(fd, "bbbbbbbbbb", SZ) != SZ) {
      म_लिखो("%s: error: write bb %d new file failed\n", s, i);
      निकास(1);
    }
  }
  बंद(fd);
  fd = खोलो("small", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: error: open small failed!\n", s);
    निकास(1);
  }
  i = पठ(fd, buf, N * SZ * 2);
  जर (i != N * SZ * 2) {
    म_लिखो("%s: read failed\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (unlink("small") < 0) {
    म_लिखो("%s: unlink small failed\n", s);
    निकास(1);
  }
}

शून्य
writebig(अक्षर *s)
{
  पूर्णांक i, fd, n;

  fd = खोलो("big", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: error: creat big failed!\n", s);
    निकास(1);
  }

  साठी (i = 0; i < MAXFILE; i++) {
    ((पूर्णांक *)buf)[0] = i;
    जर (लिख(fd, buf, BSIZE) != BSIZE) {
      म_लिखो("%s: error: write big file failed i=%d\n", s, i);
      निकास(1);
    }
  }

  बंद(fd);

  fd = खोलो("big", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: error: open big failed!\n", s);
    निकास(1);
  }

  n = 0;
  साठी (;;) {
    i = पठ(fd, buf, BSIZE);
    जर (i == 0) {
      जर (n != MAXFILE) {
        म_लिखो("%s: read only %d blocks from big", s, n);
        निकास(1);
      }
      थांबा;
    } अन्यथा जर (i != BSIZE) {
      म_लिखो("%s: read failed %d\n", s, i);
      निकास(1);
    }
    जर (((पूर्णांक *)buf)[0] != n) {
      म_लिखो("%s: read content of block %d is %d\n", s, n, ((पूर्णांक *)buf)[0]);
      निकास(1);
    }
    n++;
  }
  बंद(fd);
  जर (unlink("big") < 0) {
    म_लिखो("%s: unlink big failed\n", s);
    निकास(1);
  }
}

// many creates, followed by unlink test
शून्य
createtest(अक्षर *s)
{
  पूर्णांक i, fd;
  गणना { N = 52 };

  अक्षर name[3];
  name[0] = 'a';
  name[2] = '\0';
  साठी (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = खोलो(name, O_CREATE | O_RDWR);
    बंद(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  साठी (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

शून्य
dirtest(अक्षर *s)
{
  जर (सूची_गढ़ो("dir0") < 0) {
    म_लिखो("%s: mkdir failed\n", s);
    निकास(1);
  }

  जर (स_बदलो("dir0") < 0) {
    म_लिखो("%s: chdir dir0 failed\n", s);
    निकास(1);
  }

  जर (स_बदलो("..") < 0) {
    म_लिखो("%s: chdir .. failed\n", s);
    निकास(1);
  }

  जर (unlink("dir0") < 0) {
    म_लिखो("%s: unlink dir0 failed\n", s);
    निकास(1);
  }
}

शून्य
exectest(अक्षर *s)
{
  पूर्णांक fd, xstatus, pid;
  अक्षर *echoargv[] = {"echo", "OK", 0};
  अक्षर buf[3];

  unlink("echo-ok");
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid == 0) {
    पूर्णांक errfd = dup(1);
    जर (errfd < 0) {
      म_लिखो("%s: dup failed\n", s);
      निकास(1);
    }
    बंद(1);
    fd = खोलो("echo-ok", O_CREATE | O_WRONLY);
    जर (fd < 0) {
      ख_लिखो(errfd, "%s: create failed\n", s);
      निकास(1);
    }
    जर (fd != 1) {
      ख_लिखो(errfd, "%s: wrong fd\n", s);
      निकास(1);
    }
    जर (exec("echo", echoargv) < 0) {
      ख_लिखो(errfd, "%s: exec echo failed\n", s);
      निकास(1);
    }
    // won't get to here
  }
  जर (रुको(&xstatus) != pid) {
    म_लिखो("%s: wait failed!\n", s);
  }
  जर (xstatus != 0) {
    म_लिखो("%s: nonzero wait status %d\n", s, xstatus);
    निकास(1);
  }

  fd = खोलो("echo-ok", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  जर (पठ(fd, buf, 2) != 2) {
    म_लिखो("%s: read failed\n", s);
    निकास(1);
  }
  unlink("echo-ok");
  जर (buf[0] == 'O' && buf[1] == 'K')
    निकास(0);
  अन्यथा {
    म_लिखो("%s: wrong output\n", s);
    निकास(1);
  }
}

// simple fork and pipe read/write

शून्य
pipe1(अक्षर *s)
{
  पूर्णांक fds[2], pid, xstatus;
  पूर्णांक seq, i, n, cc, total;
  गणना { N = 5, SZ = 1033 };

  जर (pipe(fds) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
  pid = विभाजन();
  seq = 0;
  जर (pid == 0) {
    बंद(fds[0]);
    साठी (n = 0; n < N; n++) {
      साठी (i = 0; i < SZ; i++)
        buf[i] = seq++;
      जर (लिख(fds[1], buf, SZ) != SZ) {
        म_लिखो("%s: pipe1 oops 1\n", s);
        निकास(1);
      }
    }
    निकास(0);
  } अन्यथा जर (pid > 0) {
    बंद(fds[1]);
    total = 0;
    cc = 1;
    पर्यंत ((n = पठ(fds[0], buf, cc)) > 0) {
      साठी (i = 0; i < n; i++) {
        जर ((buf[i] & 0xff) != (seq++ & 0xff)) {
          म_लिखो("%s: pipe1 oops 2\n", s);
          परत;
        }
      }
      total += n;
      cc = cc * 2;
      जर (cc > आकार(buf))
        cc = आकार(buf);
    }
    जर (total != N * SZ) {
      म_लिखो("%s: pipe1 oops 3 total %d\n", s, total);
      निकास(1);
    }
    बंद(fds[0]);
    रुको(&xstatus);
    निकास(xstatus);
  } अन्यथा {
    म_लिखो("%s: fork() failed\n", s);
    निकास(1);
  }
}

// test if child is killed (status = -1)
शून्य
killstatus(अक्षर *s)
{
  पूर्णांक xst;

  साठी (पूर्णांक i = 0; i < 100; i++) {
    पूर्णांक pid1 = विभाजन();
    जर (pid1 < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid1 == 0) {
      पर्यंत (1) {
        getpid();
      }
      निकास(0);
    }
    छोड़ो(1);
    समाप्त(pid1);
    रुको(&xst);
    जर (xst != -1) {
      म_लिखो("%s: status should be -1\n", s);
      निकास(1);
    }
  }
  निकास(0);
}

// test that kill on unused process 0 doesn't set the killed flag
शून्य
killzero(अक्षर *s)
{
  पूर्णांक pid, xst;

  समाप्त(0);

  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid == 0) {
    निकास(7);
  }
  जर (रुको(&xst) != pid) {
    म_लिखो("%s: wait wrong pid\n", s);
    निकास(1);
  }
  जर (xst != 7) {
    म_लिखो("%s: child exited with status %d, expected 7\n", s, xst);
    निकास(1);
  }
  निकास(0);
}

// meant to be run w/ at most two CPUs
शून्य
preempt(अक्षर *s)
{
  पूर्णांक pid1, pid2, pid3;
  पूर्णांक pfds[2];

  pid1 = विभाजन();
  जर (pid1 < 0) {
    म_लिखो("%s: fork failed", s);
    निकास(1);
  }
  जर (pid1 == 0)
    साठी (;;)
      ;

  pid2 = विभाजन();
  जर (pid2 < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid2 == 0)
    साठी (;;)
      ;

  pipe(pfds);
  pid3 = विभाजन();
  जर (pid3 < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid3 == 0) {
    बंद(pfds[0]);
    जर (लिख(pfds[1], "x", 1) != 1)
      म_लिखो("%s: preempt write error", s);
    बंद(pfds[1]);
    साठी (;;)
      ;
  }

  बंद(pfds[1]);
  जर (पठ(pfds[0], buf, आकार(buf)) != 1) {
    म_लिखो("%s: preempt read error", s);
    परत;
  }
  बंद(pfds[0]);
  म_लिखो("kill... ");
  समाप्त(pid1);
  समाप्त(pid2);
  समाप्त(pid3);
  म_लिखो("wait... ");
  रुको(0);
  रुको(0);
  रुको(0);
}

// try to find any races between exit and wait
शून्य
exitwait(अक्षर *s)
{
  पूर्णांक i, pid;

  साठी (i = 0; i < 100; i++) {
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid) {
      पूर्णांक xstate;
      जर (रुको(&xstate) != pid) {
        म_लिखो("%s: wait wrong pid\n", s);
        निकास(1);
      }
      जर (i != xstate) {
        म_लिखो("%s: wait wrong exit status\n", s);
        निकास(1);
      }
    } अन्यथा {
      निकास(i);
    }
  }
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
शून्य
reparent(अक्षर *s)
{
  पूर्णांक master_pid = getpid();
  साठी (पूर्णांक i = 0; i < 200; i++) {
    पूर्णांक pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid) {
      जर (रुको(0) != pid) {
        म_लिखो("%s: wait wrong pid\n", s);
        निकास(1);
      }
    } अन्यथा {
      पूर्णांक pid2 = विभाजन();
      जर (pid2 < 0) {
        समाप्त(master_pid);
        निकास(1);
      }
      निकास(0);
    }
  }
  निकास(0);
}

// what if two children exit() at the same time?
शून्य
twochildren(अक्षर *s)
{
  साठी (पूर्णांक i = 0; i < 1000; i++) {
    पूर्णांक pid1 = विभाजन();
    जर (pid1 < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid1 == 0) {
      निकास(0);
    } अन्यथा {
      पूर्णांक pid2 = विभाजन();
      जर (pid2 < 0) {
        म_लिखो("%s: fork failed\n", s);
        निकास(1);
      }
      जर (pid2 == 0) {
        निकास(0);
      } अन्यथा {
        रुको(0);
        रुको(0);
      }
    }
  }
}

// concurrent forks to try to expose locking bugs.
शून्य
forkfork(अक्षर *s)
{
  गणना { N = 2 };

  साठी (पूर्णांक i = 0; i < N; i++) {
    पूर्णांक pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed", s);
      निकास(1);
    }
    जर (pid == 0) {
      साठी (पूर्णांक j = 0; j < 200; j++) {
        पूर्णांक pid1 = विभाजन();
        जर (pid1 < 0) {
          निकास(1);
        }
        जर (pid1 == 0) {
          निकास(0);
        }
        रुको(0);
      }
      निकास(0);
    }
  }

  पूर्णांक xstatus;
  साठी (पूर्णांक i = 0; i < N; i++) {
    रुको(&xstatus);
    जर (xstatus != 0) {
      म_लिखो("%s: fork in child failed", s);
      निकास(1);
    }
  }
}

शून्य
forkforkfork(अक्षर *s)
{
  unlink("stopforking");

  पूर्णांक pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed", s);
    निकास(1);
  }
  जर (pid == 0) {
    पर्यंत (1) {
      पूर्णांक fd = खोलो("stopforking", 0);
      जर (fd >= 0) {
        निकास(0);
      }
      जर (विभाजन() < 0) {
        बंद(खोलो("stopforking", O_CREATE | O_RDWR));
      }
    }

    निकास(0);
  }

  छोड़ो(20); // two seconds
  बंद(खोलो("stopforking", O_CREATE | O_RDWR));
  रुको(0);
  छोड़ो(10); // one second
}

// regression test. does reparent() violate the parent-then-child
// locking order when giving away a child to init, so that exit()
// deadlocks against init's wait()? also used to trigger a "panic:
// release" due to exit() releasing a different p->parent->lock than
// it acquired.
शून्य
reparent2(अक्षर *s)
{
  साठी (पूर्णांक i = 0; i < 800; i++) {
    पूर्णांक pid1 = विभाजन();
    जर (pid1 < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    }
    जर (pid1 == 0) {
      विभाजन();
      विभाजन();
      निकास(0);
    }
    रुको(0);
  }

  निकास(0);
}

// allocate all mem, free it, and allocate again
शून्य
mem(अक्षर *s)
{
  शून्य *m1, *m2;
  पूर्णांक pid;

  जर ((pid = विभाजन()) == 0) {
    m1 = 0;
    पर्यंत ((m2 = दो_स्मृति(10001)) != 0) {
      *(अक्षर **)m2 = m1;
      m1 = m2;
    }
    पर्यंत (m1) {
      m2 = *(अक्षर **)m1;
      मुक्त(m1);
      m1 = m2;
    }
    m1 = दो_स्मृति(1024 * 20);
    जर (m1 == 0) {
      म_लिखो("%s: couldn't allocate mem?!!\n", s);
      निकास(1);
    }
    मुक्त(m1);
    निकास(0);
  } अन्यथा {
    पूर्णांक xstatus;
    रुको(&xstatus);
    जर (xstatus == -1) {
      // probably page fault, so might be lazy lab,
      // so OK.
      निकास(0);
    }
    निकास(xstatus);
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
शून्य
sharedfd(अक्षर *s)
{
  पूर्णांक fd, pid, i, n, nc, np;
  गणना { N = 1000, SZ = 10 };
  अक्षर buf[SZ];

  unlink("sharedfd");
  fd = खोलो("sharedfd", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: cannot open sharedfd for writing", s);
    निकास(1);
  }
  pid = विभाजन();
  स_रखो(buf, pid == 0 ? 'c' : 'p', आकार(buf));
  साठी (i = 0; i < N; i++) {
    जर (लिख(fd, buf, आकार(buf)) != आकार(buf)) {
      म_लिखो("%s: write sharedfd failed\n", s);
      निकास(1);
    }
  }
  जर (pid == 0) {
    निकास(0);
  } अन्यथा {
    पूर्णांक xstatus;
    रुको(&xstatus);
    जर (xstatus != 0)
      निकास(xstatus);
  }

  बंद(fd);
  fd = खोलो("sharedfd", 0);
  जर (fd < 0) {
    म_लिखो("%s: cannot open sharedfd for reading\n", s);
    निकास(1);
  }
  nc = np = 0;
  पर्यंत ((n = पठ(fd, buf, आकार(buf))) > 0) {
    साठी (i = 0; i < आकार(buf); i++) {
      जर (buf[i] == 'c')
        nc++;
      जर (buf[i] == 'p')
        np++;
    }
  }
  बंद(fd);
  unlink("sharedfd");
  जर (nc == N * SZ && np == N * SZ) {
    निकास(0);
  } अन्यथा {
    म_लिखो("%s: nc/np test fails\n", s);
    निकास(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
शून्य
fourfiles(अक्षर *s)
{
  पूर्णांक fd, pid, i, j, n, total, pi;
  अक्षर *names[] = {"f0", "f1", "f2", "f3"};
  अक्षर *fname;
  गणना { N = 12, NCHILD = 4, SZ = 500 };

  साठी (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }

    जर (pid == 0) {
      fd = खोलो(fname, O_CREATE | O_RDWR);
      जर (fd < 0) {
        म_लिखो("%s: create failed\n", s);
        निकास(1);
      }

      स_रखो(buf, '0' + pi, SZ);
      साठी (i = 0; i < N; i++) {
        जर ((n = लिख(fd, buf, SZ)) != SZ) {
          म_लिखो("write failed %d\n", n);
          निकास(1);
        }
      }
      निकास(0);
    }
  }

  पूर्णांक xstatus;
  साठी (pi = 0; pi < NCHILD; pi++) {
    रुको(&xstatus);
    जर (xstatus != 0)
      निकास(xstatus);
  }

  साठी (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = खोलो(fname, 0);
    total = 0;
    पर्यंत ((n = पठ(fd, buf, आकार(buf))) > 0) {
      साठी (j = 0; j < n; j++) {
        जर (buf[j] != '0' + i) {
          म_लिखो("%s: wrong char\n", s);
          निकास(1);
        }
      }
      total += n;
    }
    बंद(fd);
    जर (total != N * SZ) {
      म_लिखो("wrong length %d\n", total);
      निकास(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
शून्य
createdelete(अक्षर *s)
{
  गणना { N = 20, NCHILD = 4 };
  पूर्णांक pid, i, fd, pi;
  अक्षर name[32];

  साठी (pi = 0; pi < NCHILD; pi++) {
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }

    जर (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      साठी (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = खोलो(name, O_CREATE | O_RDWR);
        जर (fd < 0) {
          म_लिखो("%s: create failed\n", s);
          निकास(1);
        }
        बंद(fd);
        जर (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          जर (unlink(name) < 0) {
            म_लिखो("%s: unlink failed\n", s);
            निकास(1);
          }
        }
      }
      निकास(0);
    }
  }

  पूर्णांक xstatus;
  साठी (pi = 0; pi < NCHILD; pi++) {
    रुको(&xstatus);
    जर (xstatus != 0)
      निकास(1);
  }

  name[0] = name[1] = name[2] = 0;
  साठी (i = 0; i < N; i++) {
    साठी (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = खोलो(name, 0);
      जर ((i == 0 || i >= N / 2) && fd < 0) {
        म_लिखो("%s: oops createdelete %s didn't exist\n", s, name);
        निकास(1);
      } अन्यथा जर ((i >= 1 && i < N / 2) && fd >= 0) {
        म_लिखो("%s: oops createdelete %s did exist\n", s, name);
        निकास(1);
      }
      जर (fd >= 0)
        बंद(fd);
    }
  }

  साठी (i = 0; i < N; i++) {
    साठी (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
शून्य
unlinkread(अक्षर *s)
{
  गणना { SZ = 5 };
  पूर्णांक fd, fd1;

  fd = खोलो("unlinkread", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: create unlinkread failed\n", s);
    निकास(1);
  }
  लिख(fd, "hello", SZ);
  बंद(fd);

  fd = खोलो("unlinkread", O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: open unlinkread failed\n", s);
    निकास(1);
  }
  जर (unlink("unlinkread") != 0) {
    म_लिखो("%s: unlink unlinkread failed\n", s);
    निकास(1);
  }

  fd1 = खोलो("unlinkread", O_CREATE | O_RDWR);
  लिख(fd1, "yyy", 3);
  बंद(fd1);

  जर (पठ(fd, buf, आकार(buf)) != SZ) {
    म_लिखो("%s: unlinkread read failed", s);
    निकास(1);
  }
  जर (buf[0] != 'h') {
    म_लिखो("%s: unlinkread wrong data\n", s);
    निकास(1);
  }
  जर (लिख(fd, buf, 10) != 10) {
    म_लिखो("%s: unlinkread write failed\n", s);
    निकास(1);
  }
  बंद(fd);
  unlink("unlinkread");
}

शून्य
linktest(अक्षर *s)
{
  गणना { SZ = 5 };
  पूर्णांक fd;

  unlink("lf1");
  unlink("lf2");

  fd = खोलो("lf1", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: create lf1 failed\n", s);
    निकास(1);
  }
  जर (लिख(fd, "hello", SZ) != SZ) {
    म_लिखो("%s: write lf1 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (link("lf1", "lf2") < 0) {
    म_लिखो("%s: link lf1 lf2 failed\n", s);
    निकास(1);
  }
  unlink("lf1");

  जर (खोलो("lf1", 0) >= 0) {
    म_लिखो("%s: unlinked lf1 but it is still there!\n", s);
    निकास(1);
  }

  fd = खोलो("lf2", 0);
  जर (fd < 0) {
    म_लिखो("%s: open lf2 failed\n", s);
    निकास(1);
  }
  जर (पठ(fd, buf, आकार(buf)) != SZ) {
    म_लिखो("%s: read lf2 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (link("lf2", "lf2") >= 0) {
    म_लिखो("%s: link lf2 lf2 succeeded! oops\n", s);
    निकास(1);
  }

  unlink("lf2");
  जर (link("lf2", "lf1") >= 0) {
    म_लिखो("%s: link non-existent succeeded! oops\n", s);
    निकास(1);
  }

  जर (link(".", "lf1") >= 0) {
    म_लिखो("%s: link . lf1 succeeded! oops\n", s);
    निकास(1);
  }
}

// test concurrent create/link/unlink of the same file
शून्य
concreate(अक्षर *s)
{
  गणना { N = 40 };
  अक्षर file[3];
  पूर्णांक i, pid, n, fd;
  अक्षर fa[N];
  संरचना {
    ushort inum;
    अक्षर name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  साठी (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = विभाजन();
    जर (pid && (i % 3) == 1) {
      link("C0", file);
    } अन्यथा जर (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } अन्यथा {
      fd = खोलो(file, O_CREATE | O_RDWR);
      जर (fd < 0) {
        म_लिखो("concreate create %s failed\n", file);
        निकास(1);
      }
      बंद(fd);
    }
    जर (pid == 0) {
      निकास(0);
    } अन्यथा {
      पूर्णांक xstatus;
      रुको(&xstatus);
      जर (xstatus != 0)
        निकास(1);
    }
  }

  स_रखो(fa, 0, आकार(fa));
  fd = खोलो(".", 0);
  n = 0;
  पर्यंत (पठ(fd, &de, आकार(de)) > 0) {
    जर (de.inum == 0)
      सुरूठेवा;
    जर (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      जर (i < 0 || i >= आकार(fa)) {
        म_लिखो("%s: concreate weird file %s\n", s, de.name);
        निकास(1);
      }
      जर (fa[i]) {
        म_लिखो("%s: concreate duplicate file %s\n", s, de.name);
        निकास(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  बंद(fd);

  जर (n != N) {
    म_लिखो("%s: concreate not enough files in directory listing\n", s);
    निकास(1);
  }

  साठी (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
      बंद(खोलो(file, 0));
      बंद(खोलो(file, 0));
      बंद(खोलो(file, 0));
      बंद(खोलो(file, 0));
      बंद(खोलो(file, 0));
      बंद(खोलो(file, 0));
    } अन्यथा {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    जर (pid == 0)
      निकास(0);
    अन्यथा
      रुको(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
शून्य
linkunlink(अक्षर *s)
{
  पूर्णांक pid, i;

  unlink("x");
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }

  अचिन्हित पूर्णांक x = (pid ? 1 : 97);
  साठी (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    जर ((x % 3) == 0) {
      बंद(खोलो("x", O_RDWR | O_CREATE));
    } अन्यथा जर ((x % 3) == 1) {
      link("cat", "x");
    } अन्यथा {
      unlink("x");
    }
  }

  जर (pid)
    रुको(0);
  अन्यथा
    निकास(0);
}

शून्य
subdir(अक्षर *s)
{
  पूर्णांक fd, cc;

  unlink("ff");
  जर (सूची_गढ़ो("dd") != 0) {
    म_लिखो("%s: mkdir dd failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/ff", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: create dd/ff failed\n", s);
    निकास(1);
  }
  लिख(fd, "ff", 2);
  बंद(fd);

  जर (unlink("dd") >= 0) {
    म_लिखो("%s: unlink dd (non-empty dir) succeeded!\n", s);
    निकास(1);
  }

  जर (सूची_गढ़ो("/dd/dd") != 0) {
    म_लिखो("%s: subdir mkdir dd/dd failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/dd/ff", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: create dd/dd/ff failed\n", s);
    निकास(1);
  }
  लिख(fd, "FF", 2);
  बंद(fd);

  fd = खोलो("dd/dd/../ff", 0);
  जर (fd < 0) {
    म_लिखो("%s: open dd/dd/../ff failed\n", s);
    निकास(1);
  }
  cc = पठ(fd, buf, आकार(buf));
  जर (cc != 2 || buf[0] != 'f') {
    म_लिखो("%s: dd/dd/../ff wrong content\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    म_लिखो("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    निकास(1);
  }

  जर (unlink("dd/dd/ff") != 0) {
    म_लिखो("%s: unlink dd/dd/ff failed\n", s);
    निकास(1);
  }
  जर (खोलो("dd/dd/ff", O_RDONLY) >= 0) {
    म_लिखो("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    निकास(1);
  }

  जर (स_बदलो("dd") != 0) {
    म_लिखो("%s: chdir dd failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("dd/../../dd") != 0) {
    म_लिखो("%s: chdir dd/../../dd failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("dd/../../../dd") != 0) {
    म_लिखो("%s: chdir dd/../../../dd failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("./..") != 0) {
    म_लिखो("%s: chdir ./.. failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/dd/ffff", 0);
  जर (fd < 0) {
    म_लिखो("%s: open dd/dd/ffff failed\n", s);
    निकास(1);
  }
  जर (पठ(fd, buf, आकार(buf)) != 2) {
    म_लिखो("%s: read dd/dd/ffff wrong len\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (खोलो("dd/dd/ff", O_RDONLY) >= 0) {
    म_लिखो("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    निकास(1);
  }

  जर (खोलो("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    म_लिखो("%s: create dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  जर (खोलो("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    म_लिखो("%s: create dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  जर (खोलो("dd", O_CREATE) >= 0) {
    म_लिखो("%s: create dd succeeded!\n", s);
    निकास(1);
  }
  जर (खोलो("dd", O_RDWR) >= 0) {
    म_लिखो("%s: open dd rdwr succeeded!\n", s);
    निकास(1);
  }
  जर (खोलो("dd", O_WRONLY) >= 0) {
    म_लिखो("%s: open dd wronly succeeded!\n", s);
    निकास(1);
  }
  जर (link("dd/ff/ff", "dd/dd/xx") == 0) {
    म_लिखो("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    निकास(1);
  }
  जर (link("dd/xx/ff", "dd/dd/xx") == 0) {
    म_लिखो("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    निकास(1);
  }
  जर (link("dd/ff", "dd/dd/ffff") == 0) {
    म_लिखो("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("dd/ff/ff") == 0) {
    म_लिखो("%s: mkdir dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("dd/xx/ff") == 0) {
    म_लिखो("%s: mkdir dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("dd/dd/ffff") == 0) {
    म_लिखो("%s: mkdir dd/dd/ffff succeeded!\n", s);
    निकास(1);
  }
  जर (unlink("dd/xx/ff") == 0) {
    म_लिखो("%s: unlink dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  जर (unlink("dd/ff/ff") == 0) {
    म_लिखो("%s: unlink dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  जर (स_बदलो("dd/ff") == 0) {
    म_लिखो("%s: chdir dd/ff succeeded!\n", s);
    निकास(1);
  }
  जर (स_बदलो("dd/xx") == 0) {
    म_लिखो("%s: chdir dd/xx succeeded!\n", s);
    निकास(1);
  }

  जर (unlink("dd/dd/ffff") != 0) {
    म_लिखो("%s: unlink dd/dd/ff failed\n", s);
    निकास(1);
  }
  जर (unlink("dd/ff") != 0) {
    म_लिखो("%s: unlink dd/ff failed\n", s);
    निकास(1);
  }
  जर (unlink("dd") == 0) {
    म_लिखो("%s: unlink non-empty dd succeeded!\n", s);
    निकास(1);
  }
  जर (unlink("dd/dd") < 0) {
    म_लिखो("%s: unlink dd/dd failed\n", s);
    निकास(1);
  }
  जर (unlink("dd") < 0) {
    म_लिखो("%s: unlink dd failed\n", s);
    निकास(1);
  }
}

// test writes that are larger than the log.
शून्य
bigwrite(अक्षर *s)
{
  पूर्णांक fd, sz;

  unlink("bigwrite");
  साठी (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = खोलो("bigwrite", O_CREATE | O_RDWR);
    जर (fd < 0) {
      म_लिखो("%s: cannot create bigwrite\n", s);
      निकास(1);
    }
    पूर्णांक i;
    साठी (i = 0; i < 2; i++) {
      पूर्णांक cc = लिख(fd, buf, sz);
      जर (cc != sz) {
        म_लिखो("%s: write(%d) ret %d\n", s, sz, cc);
        निकास(1);
      }
    }
    बंद(fd);
    unlink("bigwrite");
  }
}

शून्य
bigfile(अक्षर *s)
{
  गणना { N = 20, SZ = 600 };
  पूर्णांक fd, i, total, cc;

  unlink("bigfile.dat");
  fd = खोलो("bigfile.dat", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: cannot create bigfile", s);
    निकास(1);
  }
  साठी (i = 0; i < N; i++) {
    स_रखो(buf, i, SZ);
    जर (लिख(fd, buf, SZ) != SZ) {
      म_लिखो("%s: write bigfile failed\n", s);
      निकास(1);
    }
  }
  बंद(fd);

  fd = खोलो("bigfile.dat", 0);
  जर (fd < 0) {
    म_लिखो("%s: cannot open bigfile\n", s);
    निकास(1);
  }
  total = 0;
  साठी (i = 0;; i++) {
    cc = पठ(fd, buf, SZ / 2);
    जर (cc < 0) {
      म_लिखो("%s: read bigfile failed\n", s);
      निकास(1);
    }
    जर (cc == 0)
      थांबा;
    जर (cc != SZ / 2) {
      म_लिखो("%s: short read bigfile\n", s);
      निकास(1);
    }
    जर (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      म_लिखो("%s: read bigfile wrong data\n", s);
      निकास(1);
    }
    total += cc;
  }
  बंद(fd);
  जर (total != N * SZ) {
    म_लिखो("%s: read bigfile wrong total\n", s);
    निकास(1);
  }
  unlink("bigfile.dat");
}

शून्य
fourteen(अक्षर *s)
{
  पूर्णांक fd;

  // DIRSIZ is 14.

  जर (सूची_गढ़ो("12345678901234") != 0) {
    म_लिखो("%s: mkdir 12345678901234 failed\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("12345678901234/123456789012345") != 0) {
    म_लिखो("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    निकास(1);
  }
  fd = खोलो("123456789012345/123456789012345/123456789012345", O_CREATE);
  जर (fd < 0) {
    म_लिखो(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    निकास(1);
  }
  बंद(fd);
  fd = खोलो("12345678901234/12345678901234/12345678901234", 0);
  जर (fd < 0) {
    म_लिखो("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  जर (सूची_गढ़ो("12345678901234/12345678901234") == 0) {
    म_लिखो("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("123456789012345/12345678901234") == 0) {
    म_लिखो("%s: mkdir 12345678901234/123456789012345 succeeded!\n", s);
    निकास(1);
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
rmdot(अक्षर *s)
{
  जर (सूची_गढ़ो("dots") != 0) {
    म_लिखो("%s: mkdir dots failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("dots") != 0) {
    म_लिखो("%s: chdir dots failed\n", s);
    निकास(1);
  }
  जर (unlink(".") == 0) {
    म_लिखो("%s: rm . worked!\n", s);
    निकास(1);
  }
  जर (unlink("..") == 0) {
    म_लिखो("%s: rm .. worked!\n", s);
    निकास(1);
  }
  जर (स_बदलो("/") != 0) {
    म_लिखो("%s: chdir / failed\n", s);
    निकास(1);
  }
  जर (unlink("dots/.") == 0) {
    म_लिखो("%s: unlink dots/. worked!\n", s);
    निकास(1);
  }
  जर (unlink("dots/..") == 0) {
    म_लिखो("%s: unlink dots/.. worked!\n", s);
    निकास(1);
  }
  जर (unlink("dots") != 0) {
    म_लिखो("%s: unlink dots failed!\n", s);
    निकास(1);
  }
}

शून्य
dirfile(अक्षर *s)
{
  पूर्णांक fd;

  fd = खोलो("dirfile", O_CREATE);
  जर (fd < 0) {
    म_लिखो("%s: create dirfile failed\n", s);
    निकास(1);
  }
  बंद(fd);
  जर (स_बदलो("dirfile") == 0) {
    म_लिखो("%s: chdir dirfile succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो("dirfile/xx", 0);
  जर (fd >= 0) {
    म_लिखो("%s: create dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो("dirfile/xx", O_CREATE);
  जर (fd >= 0) {
    म_लिखो("%s: create dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("dirfile/xx") == 0) {
    म_लिखो("%s: mkdir dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  जर (unlink("dirfile/xx") == 0) {
    म_लिखो("%s: unlink dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  जर (link("README", "dirfile/xx") == 0) {
    म_लिखो("%s: link to dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  जर (unlink("dirfile") != 0) {
    म_लिखो("%s: unlink dirfile failed!\n", s);
    निकास(1);
  }

  fd = खोलो(".", O_RDWR);
  जर (fd >= 0) {
    म_लिखो("%s: open . for writing succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो(".", 0);
  जर (लिख(fd, "x", 1) > 0) {
    म_लिखो("%s: write . succeeded!\n", s);
    निकास(1);
  }
  बंद(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
शून्य
iref(अक्षर *s)
{
  पूर्णांक i, fd;

  साठी (i = 0; i < NINODE + 1; i++) {
    जर (सूची_गढ़ो("irefd") != 0) {
      म_लिखो("%s: mkdir irefd failed\n", s);
      निकास(1);
    }
    जर (स_बदलो("irefd") != 0) {
      म_लिखो("%s: chdir irefd failed\n", s);
      निकास(1);
    }

    सूची_गढ़ो("");
    link("README", "");
    fd = खोलो("", O_CREATE);
    जर (fd >= 0)
      बंद(fd);
    fd = खोलो("xx", O_CREATE);
    जर (fd >= 0)
      बंद(fd);
    unlink("xx");
  }

  // clean up
  साठी (i = 0; i < NINODE + 1; i++) {
    स_बदलो("..");
    unlink("irefd");
  }

  स_बदलो("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
शून्य
forktest(अक्षर *s)
{
  गणना { N = 1000 };
  पूर्णांक n, pid;

  साठी (n = 0; n < N; n++) {
    pid = विभाजन();
    जर (pid < 0)
      थांबा;
    जर (pid == 0)
      निकास(0);
  }

  जर (n == 0) {
    म_लिखो("%s: no fork at all!\n", s);
    निकास(1);
  }

  जर (n == N) {
    म_लिखो("%s: fork claimed to work 1000 times!\n", s);
    निकास(1);
  }

  साठी (; n > 0; n--) {
    जर (रुको(0) < 0) {
      म_लिखो("%s: wait stopped early\n", s);
      निकास(1);
    }
  }

  जर (रुको(0) != -1) {
    म_लिखो("%s: wait got too many\n", s);
    निकास(1);
  }
}

शून्य
sbrkbasic(अक्षर *s)
{
  गणना { TOOMUCH = 1024 * 1024 * 1024 };
  पूर्णांक i, pid, xstatus;
  अक्षर *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("fork failed in sbrkbasic\n");
    निकास(1);
  }
  जर (pid == 0) {
    a = sbrk(TOOMUCH);
    जर (a == (अक्षर *)SBRK_ERROR) {
      // it's OK if this fails.
      निकास(0);
    }

    साठी (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    निकास(1);
  }

  रुको(&xstatus);
  जर (xstatus == 1) {
    म_लिखो("%s: too much memory allocated!\n", s);
    निकास(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  साठी (i = 0; i < 5000; i++) {
    b = sbrk(1);
    जर (b != a) {
      म_लिखो("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      निकास(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: sbrk test fork failed\n", s);
    निकास(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  जर (c != a + 1) {
    म_लिखो("%s: sbrk test failed post-fork\n", s);
    निकास(1);
  }
  जर (pid == 0)
    निकास(0);
  रुको(&xstatus);
  निकास(xstatus);
}

शून्य
sbrkmuch(अक्षर *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  अक्षर *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  जर (p != a) {
    म_लिखो("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    निकास(1);
  }

  lastaddr = (अक्षर *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  जर (c == (अक्षर *)SBRK_ERROR) {
    म_लिखो("%s: sbrk could not deallocate\n", s);
    निकास(1);
  }
  c = sbrk(0);
  जर (c != a - PGSIZE) {
    म_लिखो("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    निकास(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  जर (c != a || sbrk(0) != a + PGSIZE) {
    म_लिखो("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    निकास(1);
  }
  जर (*lastaddr == 99) {
    // should be zero
    म_लिखो("%s: sbrk de-allocation didn't really deallocate\n", s);
    निकास(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  जर (c != a) {
    म_लिखो("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    निकास(1);
  }
}

// can we read the kernel's memory?
शून्य
kernmem(अक्षर *s)
{
  अक्षर *a;
  पूर्णांक pid;

  साठी (a = (अक्षर *)(KERNBASE); a < (अक्षर *)(KERNBASE + 2000000); a += 50000) {
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid == 0) {
      म_लिखो("%s: oops could read %p = %x\n", s, a, *a);
      निकास(1);
    }
    पूर्णांक xstatus;
    रुको(&xstatus);
    जर (xstatus != -1) // did kernel kill child?
      निकास(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
शून्य
MAXVAplus(अक्षर *s)
{
  अस्थिर uint64 a = MAXVA;
  साठी (; a != 0; a <<= 1) {
    पूर्णांक pid;
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    जर (pid == 0) {
      *(अक्षर *)a = 99;
      म_लिखो("%s: oops wrote %p\n", s, (शून्य *)a);
      निकास(1);
    }
    पूर्णांक xstatus;
    रुको(&xstatus);
    जर (xstatus != -1) // did kernel kill child?
      निकास(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
शून्य
sbrkfail(अक्षर *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  पूर्णांक i, xstatus;
  पूर्णांक fds[2];
  अक्षर scratch;
  अक्षर *c, *a;
  पूर्णांक pids[10];
  पूर्णांक pid;
  पूर्णांक failed;

  failed = 0;
  जर (pipe(fds) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
  साठी (i = 0; i < आकार(pids) / आकार(pids[0]); i++) {
    जर ((pids[i] = विभाजन()) == 0) {
      // allocate a lot of memory
      जर (sbrk(BIG - (uint64)sbrk(0)) == (अक्षर *)SBRK_ERROR)
        लिख(fds[1], "0", 1);
      अन्यथा
        लिख(fds[1], "1", 1);
      // sit around until killed
      साठी (;;)
        छोड़ो(1000);
    }
    जर (pids[i] != -1) {
      पठ(fds[0], &scratch, 1);
      जर (scratch == '0')
        failed = 1;
    }
  }
  जर (!failed) {
    म_लिखो("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  साठी (i = 0; i < आकार(pids) / आकार(pids[0]); i++) {
    जर (pids[i] == -1)
      सुरूठेवा;
    समाप्त(pids[i]);
    रुको(0);
  }
  जर (c == (अक्षर *)SBRK_ERROR) {
    म_लिखो("%s: failed sbrk leaked memory\n", s);
    निकास(1);
  }

  // test running fork with the above allocated page
  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  जर (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    जर (a == (अक्षर *)SBRK_ERROR) {
      निकास(0);
    }
    म_लिखो("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    निकास(1);
  }
  रुको(&xstatus);
  जर (xstatus != 0)
    निकास(1);
}

// test reads/writes from/to allocated memory
शून्य
sbrkarg(अक्षर *s)
{
  अक्षर *a;
  पूर्णांक fd, n;

  a = sbrk(PGSIZE);
  fd = खोलो("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  जर (fd < 0) {
    म_लिखो("%s: open sbrk failed\n", s);
    निकास(1);
  }
  जर ((n = लिख(fd, a, PGSIZE)) < 0) {
    म_लिखो("%s: write sbrk failed\n", s);
    निकास(1);
  }
  बंद(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  जर (pipe((पूर्णांक *)a) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
}

शून्य
validatetest(अक्षर *s)
{
  पूर्णांक hi;
  uint64 p;

  hi = 1100 * 1024;
  साठी (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    जर (link("nosuchfile", (अक्षर *)p) != -1) {
      म_लिखो("%s: link should not succeed\n", s);
      निकास(1);
    }
  }
}

// does uninitialized data start out zero?
अक्षर uninit[10000];
शून्य
bsstest(अक्षर *s)
{
  पूर्णांक i;

  साठी (i = 0; i < आकार(uninit); i++) {
    जर (uninit[i] != '\0') {
      म_लिखो("%s: bss test failed\n", s);
      निकास(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
शून्य
bigargtest(अक्षर *s)
{
  पूर्णांक pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = विभाजन();
  जर (pid == 0) {
    स्थिर अक्षर *args[MAXARG];
    पूर्णांक i;
    अक्षर big[400];
    स_रखो(big, ' ', आकार(big));
    big[आकार(big) - 1] = '\0';
    साठी (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = खोलो("bigarg-ok", O_CREATE);
    बंद(fd);
    निकास(0);
  } अन्यथा जर (pid < 0) {
    म_लिखो("%s: bigargtest: fork failed\n", s);
    निकास(1);
  }

  रुको(&xstatus);
  जर (xstatus != 0)
    निकास(xstatus);
  fd = खोलो("bigarg-ok", 0);
  जर (fd < 0) {
    म_लिखो("%s: bigarg test failed!\n", s);
    निकास(1);
  }
  बंद(fd);
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
शून्य
fsfull()
{
  पूर्णांक nfiles;
  पूर्णांक fsblocks = 0;

  म_लिखो("fsfull test\n");

  साठी (nfiles = 0;; nfiles++) {
    अक्षर name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    म_लिखो("writing %s\n", name);
    पूर्णांक fd = खोलो(name, O_CREATE | O_RDWR);
    जर (fd < 0) {
      म_लिखो("open %s failed\n", name);
      थांबा;
    }
    पूर्णांक total = 0;
    पर्यंत (1) {
      पूर्णांक cc = लिख(fd, buf, BSIZE);
      जर (cc < BSIZE)
        थांबा;
      total += cc;
      fsblocks++;
    }
    म_लिखो("wrote %d bytes\n", total);
    बंद(fd);
    जर (total == 0)
      थांबा;
  }

  पर्यंत (nfiles >= 0) {
    अक्षर name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  म_लिखो("fsfull test finished, %d blocks\n", fsblocks);
}

शून्य
argptest(अक्षर *s)
{
  पूर्णांक fd;
  fd = खोलो("init", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  पठ(fd, sbrk(0) - 1, -1);
  बंद(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
शून्य
stacktest(अक्षर *s)
{
  पूर्णांक pid;
  पूर्णांक xstatus;

  pid = विभाजन();
  जर (pid == 0) {
    अक्षर *sp = (अक्षर *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    म_लिखो("%s: stacktest: read below stack %d\n", s, *sp);
    निकास(1);
  } अन्यथा जर (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  रुको(&xstatus);
  जर (xstatus == -1) // kernel killed child?
    निकास(0);
  अन्यथा
    निकास(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
शून्य
nowrite(अक्षर *s)
{
  पूर्णांक pid;
  पूर्णांक xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  साठी (पूर्णांक ai = 0; ai < आकार(addrs) / आकार(addrs[0]); ai++) {
    pid = विभाजन();
    जर (pid == 0) {
      अस्थिर पूर्णांक *addr = (पूर्णांक *)addrs[ai];
      *addr = 10;
      म_लिखो("%s: write to %p did not fail!\n", s, addr);
      निकास(0);
    } अन्यथा जर (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    रुको(&xstatus);
    जर (xstatus == 0) {
      // kernel did not kill child!
      निकास(1);
    }
  }
  निकास(0);
}

// regression test. copyin(), copyout(), and copyinstr() used to cast
// the virtual page address to uint, which (with certain wild system
// call arguments) resulted in a kernel page faults.
शून्य *big = (शून्य *)0xeaeb0b5b00002f5e;
शून्य
pgbug(अक्षर *s)
{
  अक्षर *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  निकास(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
शून्य
sbrkbugs(अक्षर *s)
{
  पूर्णांक pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  जर (pid == 0) {
    पूर्णांक sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    निकास(0);
  }
  रुको(0);

  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  जर (pid == 0) {
    पूर्णांक sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    निकास(0);
  }
  रुको(0);

  pid = विभाजन();
  जर (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  जर (pid == 0) {
    // set the break in the middle of a page.
    sbrk((10 * PGSIZE + 2048) - (uint64)sbrk(0));

    // reduce the break a bit, but not enough to
    // cause a page to be freed. this used to cause
    // a panic.
    sbrk(-10);

    निकास(0);
  }
  रुको(0);

  निकास(0);
}

// if process size was somewhat more than a page boundary, and then
// shrunk to be somewhat less than that page boundary, can the kernel
// still copyin() from addresses in the last page?
शून्य
sbrklast(अक्षर *s)
{
  uint64 शीर्ष = (uint64)sbrk(0);
  जर ((शीर्ष % PGSIZE) != 0)
    sbrk(PGSIZE - (शीर्ष % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  शीर्ष = (uint64)sbrk(0);
  अक्षर *p = (अक्षर *)(शीर्ष - 64);
  p[0] = 'x';
  p[1] = '\0';
  पूर्णांक fd = खोलो(p, O_RDWR | O_CREATE);
  लिख(fd, p, 1);
  बंद(fd);
  fd = खोलो(p, O_RDWR);
  p[0] = '\0';
  पठ(fd, p, 1);
  जर (p[0] != 'x')
    निकास(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
शून्य
sbrk8000(अक्षर *s)
{
  sbrk(0x80000004);
  अस्थिर अक्षर *शीर्ष = sbrk(0);
  *(शीर्ष - 1) = *(शीर्ष - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
शून्य
badarg(अक्षर *s)
{
  साठी (पूर्णांक i = 0; i < 50000; i++) {
    अक्षर *argv[2];
    argv[0] = (अक्षर *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  निकास(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
शून्य
lazy_alloc(अक्षर *s)
{
  अक्षर *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  जर (prev_end == (अक्षर *)SBRK_ERROR) {
    म_लिखो("sbrklazy() failed\n");
    निकास(1);
  }
  new_end = prev_end + REGION_SZ;

  साठी (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(अक्षर **)i = i;

  साठी (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    जर (*(अक्षर **)i != i) {
      म_लिखो("failed to read value from memory\n");
      निकास(1);
    }
  }

  निकास(0);
}

// Touch a page every 64 pages in region, which with lazy allocation
// causes one page to be allocated. Check that freeing the region
// frees the allocated pages.
शून्य
lazy_unmap(अक्षर *s)
{
  पूर्णांक pid;
  अक्षर *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  जर (prev_end == (अक्षर *)SBRK_ERROR) {
    म_लिखो("sbrklazy() failed\n");
    निकास(1);
  }
  new_end = prev_end + REGION_SZ;

  साठी (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(अक्षर **)i = i;

  साठी (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("error forking\n");
      निकास(1);
    } अन्यथा जर (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(अक्षर **)i = i;
      निकास(0);
    } अन्यथा {
      पूर्णांक status;
      रुको(&status);
      जर (status == 0) {
        म_लिखो("memory not unmapped\n");
        निकास(1);
      }
    }
  }

  निकास(0);
}

शून्य
lazy_copy(अक्षर *s)
{
  // copyinstr on lazy page
  {
    अक्षर *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    खोलो(p + 8192, 0);
  }

  {
    शून्य *xx = sbrk(0);
    शून्य *ret = sbrk(-(((uint64)xx) + 1));
    जर (ret != xx) {
      म_लिखो("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      निकास(1);
    }
  }

  // read() and write() to these addresses should fail.
  अचिन्हित दीर्घ bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  साठी (पूर्णांक i = 0; i < आकार(bad) / आकार(bad[0]); i++) {
    पूर्णांक fd = खोलो("README", 0);
    जर (fd < 0) {
      म_लिखो("cannot open README\n");
      निकास(1);
    }
    जर (पठ(fd, (अक्षर *)bad[i], 512) >= 0) {
      म_लिखो("read succeeded\n");
      निकास(1);
    }
    बंद(fd);
    fd = खोलो("junk", O_CREATE | O_RDWR | O_TRUNC);
    जर (fd < 0) {
      म_लिखो("cannot open junk\n");
      निकास(1);
    }
    जर (लिख(fd, (अक्षर *)bad[i], 512) >= 0) {
      म_लिखो("write succeeded\n");
      निकास(1);
    }
    बंद(fd);
  }

  निकास(0);
}

शून्य
lazy_copyinstr(अक्षर *s)
{
  अक्षर *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  जर ((uint64)p % PGSIZE != 0) {
    म_लिखो("%s: sbrk did not align\n", s);
    निकास(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  पूर्णांक fd = खोलो(&p[4095], O_RDONLY);
  जर (fd < 0) {
    म_लिखो("could not open /");
    निकास(1);
  }

  संरचना stat st;
  पूर्णांक r = ख_स्थिति(fd, &st);
  जर (r < 0) {
    म_लिखो("could not stat /");
    निकास(1);
  }

  जर (st.type != T_DIR) {
    म_लिखो("/ is not T_DIR");
    निकास(1);
  }

  बंद(fd);
}

शून्य
lazy_sbrk(अक्षर *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  अक्षर *p = sbrk(0);
  पर्यंत ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    जर (p < 0) {
      म_लिखो("sbrklazy(%d) returned %p\n", 1 << 30, p);
      निकास(1);
    }

    p = sbrklazy(0);
  }

  पूर्णांक n = TRAPFRAME - PGSIZE - (uint64)p;

  अक्षर *p1 = sbrklazy(n);
  जर (p1 < 0 || p1 != p) {
    म_लिखो("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    निकास(1);
  }

  p = sbrk(PGSIZE);
  जर (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    म_लिखो("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    निकास(1);
  }

  p[0] = 1;
  जर (p[1] != 0) {
    म_लिखो("sbrk() returned non-zero-filled memory\n");
    निकास(1);
  }

  p = sbrk(1);
  जर ((uint64)p != -1) {
    म_लिखो("sbrk(1) returned %p, expected error\n", p);
    निकास(1);
  }

  p = sbrklazy(1);
  जर ((uint64)p != -1) {
    म_लिखो("sbrklazy(1) returned %p, expected error\n", p);
    निकास(1);
  }

  निकास(0);
}

शून्य
partial_write(अक्षर *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  पूर्णांक fd = खोलो("testfile", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: cannot create testfile\n", s);
    निकास(1);
  }

  पूर्णांक cc = लिख(fd, "A", 1);
  जर (cc != 1) {
    म_लिखो("%s: could not write A\n", s);
    निकास(1);
  }

  बंद(fd);
  fd = खोलो("testfile", O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  अक्षर *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  जर ((uint64)p % PGSIZE != 0) {
    म_लिखो("%s: sbrk did not align\n", s);
    निकास(1);
  }

  p[-1] = 'X';

  cc = लिख(fd, p - 1, 2);
  जर (cc != -1) {
    म_लिखो("%s: write succeeded, should have failed\n", s);
    निकास(1);
  }

  बंद(fd);

  fd = खोलो("testfile", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  अक्षर b;
  cc = पठ(fd, &b, 1);
  जर (cc != 1) {
    म_लिखो("%s: cannot read testfile\n", s);
    निकास(1);
  }

  बंद(fd);

  जर (b != 'X') {
    म_लिखो("%s: read returned %c, expected X\n", s, b);
    निकास(1);
  }

  fd = खोलो("bigfile", O_CREATE | O_RDWR);
  साठी (पूर्णांक i = 0; i < 64; i++) {
    अक्षर buf[1024];
    स_रखो(buf, 0, आकार(buf));
    cc = लिख(fd, buf, आकार(buf));
    जर (cc != आकार(buf)) {
      म_लिखो("%s: could not write to bigfile\n", s);
      निकास(-1);
    }
  }
  बंद(fd);

  unlink("bigfile");

  fd = खोलो("testfile", O_RDONLY);
  जर (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  cc = पठ(fd, &b, 1);
  जर (cc != 1) {
    म_लिखो("%s: cannot read testfile\n", s);
    निकास(1);
  }

  बंद(fd);

  जर (b != 'X') {
    म_लिखो("%s: read returned %c, expected X\n", s, b);
    निकास(1);
  }

  unlink("testfile");
}

शून्य
unlinkcwd(अक्षर *s)
{
  जर (सूची_गढ़ो("/a") < 0) {
    म_लिखो("%s: mkdir /a failed\n", s);
    निकास(1);
  }
  जर (सूची_गढ़ो("/a/b") < 0) {
    म_लिखो("%s: mkdir /a/b failed\n", s);
    निकास(1);
  }
  जर (स_बदलो("/a/b") < 0) {
    म_लिखो("%s: chdir failed\n", s);
    निकास(1);
  }
  जर (unlink("/a/b") < 0) {
    म_लिखो("%s: unlink /a/b failed\n", s);
    निकास(1);
  }
  जर (unlink("/a") < 0) {
    म_लिखो("%s: unlink /a failed\n", s);
    निकास(1);
  }
  जर (खोलो("../", O_RDONLY) > 0) {
    म_लिखो("%s: open ../ non-existing directory\n", s);
  }
  जर (खोलो("../c", O_CREATE) > 0) {
    म_लिखो("%s: create ../c non-existing file\n", s);
  }
}

संरचना test {
  शून्य (*f)(अक्षर *);
  अक्षर *s;
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
bigdir(अक्षर *s)
{
  गणना { N = 500 };
  पूर्णांक i, fd;
  अक्षर name[10];

  unlink("bd");

  fd = खोलो("bd", O_CREATE);
  जर (fd < 0) {
    म_लिखो("%s: bigdir create failed\n", s);
    निकास(1);
  }
  बंद(fd);

  साठी (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    जर (link("bd", name) != 0) {
      म_लिखो("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      निकास(1);
    }
  }

  unlink("bd");
  साठी (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    जर (unlink(name) != 0) {
      म_लिखो("%s: bigdir unlink failed", s);
      निकास(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
शून्य
manywrites(अक्षर *s)
{
  पूर्णांक nchildren = 4;
  पूर्णांक howmany = 30; // increase to look for deadlock

  साठी (पूर्णांक ci = 0; ci < nchildren; ci++) {
    पूर्णांक pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    }

    जर (pid == 0) {
      अक्षर name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      साठी (पूर्णांक iters = 0; iters < howmany; iters++) {
        साठी (पूर्णांक i = 0; i < ci + 1; i++) {
          पूर्णांक fd = खोलो(name, O_CREATE | O_RDWR);
          जर (fd < 0) {
            म_लिखो("%s: cannot create %s\n", s, name);
            निकास(1);
          }
          पूर्णांक sz = आकार(buf);
          पूर्णांक cc = लिख(fd, buf, sz);
          जर (cc != sz) {
            म_लिखो("%s: write(%d) ret %d\n", s, sz, cc);
            निकास(1);
          }
          बंद(fd);
        }
        unlink(name);
      }

      unlink(name);
      निकास(0);
    }
  }

  साठी (पूर्णांक ci = 0; ci < nchildren; ci++) {
    पूर्णांक st = 0;
    रुको(&st);
    जर (st != 0)
      निकास(st);
  }
  निकास(0);
}

// regression test. does write() with an invalid buffer pointer cause
// a block to be allocated for a file that is then not freed when the
// file is deleted? if the kernel has this bug, it will panic: balloc:
// out of blocks. assumed_free may need to be raised to be more than
// the number of free blocks. this test takes a long time.
शून्य
badwrite(अक्षर *s)
{
  पूर्णांक assumed_free = 600;

  unlink("junk");
  साठी (पूर्णांक i = 0; i < assumed_free; i++) {
    पूर्णांक fd = खोलो("junk", O_CREATE | O_WRONLY);
    जर (fd < 0) {
      म_लिखो("open junk failed\n");
      निकास(1);
    }
    लिख(fd, (अक्षर *)0xffffffffffL, 1);
    बंद(fd);
    unlink("junk");
  }

  पूर्णांक fd = खोलो("junk", O_CREATE | O_WRONLY);
  जर (fd < 0) {
    म_लिखो("open junk failed\n");
    निकास(1);
  }
  जर (लिख(fd, "x", 1) != 1) {
    म_लिखो("write failed\n");
    निकास(1);
  }
  बंद(fd);
  unlink("junk");

  निकास(0);
}

// test the exec() code that cleans up if it runs out
// of memory. it's really a test that such a condition
// doesn't cause a panic.
शून्य
execout(अक्षर *s)
{
  साठी (पूर्णांक avail = 0; avail < 15; avail++) {
    पूर्णांक pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    } अन्यथा जर (pid == 0) {
      // allocate all of memory.
      पर्यंत (1) {
        अक्षर *a = sbrk(PGSIZE);
        जर (a == SBRK_ERROR)
          थांबा;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      साठी (पूर्णांक i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      बंद(1);
      अक्षर *args[] = {"echo", "x", 0};
      exec("echo", args);
      निकास(0);
    } अन्यथा {
      रुको((पूर्णांक *)0);
    }
  }

  निकास(0);
}

// can the kernel tolerate running out of disk space?
शून्य
diskfull(अक्षर *s)
{
  पूर्णांक fi;
  पूर्णांक done = 0;

  unlink("diskfulldir");

  साठी (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    अक्षर name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    पूर्णांक fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    जर (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      म_लिखो("%s: could not create file %s\n", s, name);
      done = 1;
      थांबा;
    }
    साठी (पूर्णांक i = 0; i < MAXFILE; i++) {
      अक्षर buf[BSIZE];
      जर (लिख(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        बंद(fd);
        थांबा;
      }
    }
    बंद(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  पूर्णांक nzz = 128;
  साठी (पूर्णांक i = 0; i < nzz; i++) {
    अक्षर name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णांक fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    जर (fd < 0)
      थांबा;
    बंद(fd);
  }

  // this mkdir() is expected to fail.
  जर (सूची_गढ़ो("diskfulldir") == 0)
    म_लिखो("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  साठी (पूर्णांक i = 0; i < nzz; i++) {
    अक्षर name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  साठी (पूर्णांक i = 0; '0' + i < 0177; i++) {
    अक्षर name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
outofinodes(अक्षर *s)
{
  पूर्णांक nzz = 32 * 32;
  साठी (पूर्णांक i = 0; i < nzz; i++) {
    अक्षर name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णांक fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    जर (fd < 0) {
      // failure is eventually expected.
      थांबा;
    }
    बंद(fd);
  }

  साठी (पूर्णांक i = 0; i < nzz; i++) {
    अक्षर name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
linkoverflow(अक्षर *s)
{
  गणना { TARGET = 32768 };
  गणना { DIRS = 64 };
  संरचना stat st;
  पूर्णांक i;

  unlink("/lof");
  पूर्णांक fd = खोलो("/lof", O_CREATE | O_RDWR);
  जर (fd < 0) {
    म_लिखो("%s: cannot create /lof\n", s);
    निकास(1);
  }
  बंद(fd);

  साठी (i = 0; i < TARGET; i++) {
    पूर्णांक d = i % DIRS;
    पूर्णांक f = i / DIRS;

    अक्षर pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    जर (f == 0 && सूची_गढ़ो(pn) < 0) {
      म_लिखो("%s: mkdir(%s) failed\n", s, pn);
      निकास(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    जर (link("/lof", pn) < 0) {
      जर (stat("/lof", &st) < 0) {
        म_लिखो("%s: stat(/lof) failed\n", s);
        निकास(1);
      }
      जर (st.nlink >= 32767) {
        // overflow check succeeded.
        थांबा;
      }
      म_लिखो("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      निकास(1);
    }

    जर (i % 100 == 0) {
      म_लिखो("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  जर (stat("/lof", &st) < 0) {
    म_लिखो("%s: stat(/lof) failed\n", s);
    निकास(1);
  }

  unlink("/lof");

  जर (st.nlink < 0) {
    म_लिखो("%s: negative link count: %d\n", s, st.nlink);
    निकास(1);
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
पूर्णांक
run(शून्य f(अक्षर *), अक्षर *s)
{
  पूर्णांक pid;
  पूर्णांक xstatus;

  म_लिखो("test %s: ", s);
  जर ((pid = विभाजन()) < 0) {
    म_लिखो("runtest: fork error\n");
    निकास(1);
  }
  जर (pid == 0) {
    f(s);
    निकास(0);
  } अन्यथा {
    रुको(&xstatus);
    जर (xstatus != 0)
      म_लिखो("FAILED\n");
    अन्यथा
      म_लिखो("OK\n");
    परत xstatus == 0;
  }
}

पूर्णांक
runtests(संरचना test *tests, अक्षर *justone, पूर्णांक continuous)
{
  पूर्णांक ntests = 0;
  साठी (संरचना test *t = tests; t->s != 0; t++) {
    जर ((justone == 0) || म_भेद(t->s, justone) == 0) {
      ntests++;
      जर (!run(t->f, t->s)) {
        जर (continuous != 2) {
          म_लिखो("SOME TESTS FAILED\n");
          परत -1;
        }
      }
    }
  }
  परत ntests;
}

// use sbrk() to count how many free physical memory pages there are.
पूर्णांक
countfree()
{
  पूर्णांक n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  पर्यंत (1) {
    अक्षर *a = sbrk(PGSIZE);
    जर (a == SBRK_ERROR) {
      थांबा;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  परत n;
}

पूर्णांक
drivetests(पूर्णांक quick, पूर्णांक continuous, अक्षर *justone)
{
  करा {
    म_लिखो("usertests starting\n");
    पूर्णांक free0 = countfree();
    पूर्णांक free1 = 0;
    पूर्णांक ntests = 0;
    पूर्णांक n;
    n = runtests(quicktests, justone, continuous);
    जर (n < 0) {
      जर (continuous != 2) {
        परत 1;
      }
    } अन्यथा {
      ntests += n;
    }
    जर (!quick) {
      जर (justone == 0)
        म_लिखो("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      जर (n < 0) {
        जर (continuous != 2) {
          परत 1;
        }
      } अन्यथा {
        ntests += n;
      }
    }
    जर ((free1 = countfree()) < free0) {
      म_लिखो("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      जर (continuous != 2) {
        परत 1;
      }
    }
    जर (justone != 0 && ntests == 0) {
      म_लिखो("NO TESTS EXECUTED\n");
      परत 1;
    }
  } पर्यंत (continuous);
  परत 0;
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक continuous = 0;
  पूर्णांक quick = 0;
  अक्षर *justone = 0;

  जर (argc == 2 && म_भेद(argv[1], "-q") == 0) {
    quick = 1;
  } अन्यथा जर (argc == 2 && म_भेद(argv[1], "-c") == 0) {
    continuous = 1;
  } अन्यथा जर (argc == 2 && म_भेद(argv[1], "-C") == 0) {
    continuous = 2;
  } अन्यथा जर (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } अन्यथा जर (argc > 1) {
    म_लिखो("Usage: usertests [-c] [-C] [-q] [testname]\n");
    निकास(1);
  }
  जर (drivetests(quick, continuous, justone)) {
    निकास(1);
  }
  म_लिखो("ALL TESTS PASSED\n");
  निकास(0);
}
