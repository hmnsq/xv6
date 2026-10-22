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

वर्ण buf[BUFSZ];

//
// Section with tests that run fairly quickly.  Use -q if you want to
// run just those.  Without -q usertests also runs the ones that take a
// fair amount of time.
//

// what if you pass ridiculous pointers to system calls
// that read user memory with copyin?
शून्य
copyin(वर्ण *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  प्रति (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = खोलो("copyin1", O_CREATE | O_WRONLY);
    यदि (fd < 0) {
      म_लिखो("open(copyin1) failed\n");
      निकास(1);
    }
    पूर्णाङ्क n = लिख(fd, (शून्य *)addr, 8192);
    यदि (n >= 0) {
      म_लिखो("write(fd, %p, 8192) returned %d, not -1\n", (शून्य *)addr, n);
      निकास(1);
    }
    बंद(fd);
    unlink("copyin1");

    n = लिख(1, (वर्ण *)addr, 8192);
    यदि (n > 0) {
      म_लिखो("write(1, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      निकास(1);
    }

    पूर्णाङ्क fds[2];
    यदि (pipe(fds) < 0) {
      म_लिखो("pipe() failed\n");
      निकास(1);
    }
    n = लिख(fds[1], (वर्ण *)addr, 8192);
    यदि (n > 0) {
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
copyout(वर्ण *s)
{
  uint64 addrs[] = {0LL,          0x80000000LL, 0x3fffffe000,
                    0x3ffffff000, 0x4000000000, 0xffffffffffffffff};

  प्रति (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = खोलो("README", 0);
    यदि (fd < 0) {
      म_लिखो("open(README) failed\n");
      निकास(1);
    }
    पूर्णाङ्क n = पठ(fd, (शून्य *)addr, 8192);
    यदि (n > 0) {
      म_लिखो("read(fd, %p, 8192) returned %d, not -1 or 0\n", (शून्य *)addr, n);
      निकास(1);
    }
    बंद(fd);

    पूर्णाङ्क fds[2];
    यदि (pipe(fds) < 0) {
      म_लिखो("pipe() failed\n");
      निकास(1);
    }
    n = लिख(fds[1], "x", 1);
    यदि (n != 1) {
      म_लिखो("pipe write failed\n");
      निकास(1);
    }
    n = पठ(fds[0], (शून्य *)addr, 8192);
    यदि (n > 0) {
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
copyinstr1(वर्ण *s)
{
  uint64 addrs[] = {0x80000000LL, 0x3fffffe000, 0x3ffffff000, 0x4000000000,
                    0xffffffffffffffff};

  प्रति (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    uint64 addr = addrs[ai];

    पूर्णाङ्क fd = खोलो((वर्ण *)addr, O_CREATE | O_WRONLY);
    यदि (fd >= 0) {
      म_लिखो("open(%p) returned %d, not -1\n", (शून्य *)addr, fd);
      निकास(1);
    }
  }
}

// what if a string system call argument is exactly the size
// of the kernel buffer it is copied into, so that the null
// would fall just beyond the end of the kernel buffer?
शून्य
copyinstr2(वर्ण *s)
{
  वर्ण b[MAXPATH + 1];

  प्रति (पूर्णाङ्क i = 0; i < MAXPATH; i++)
    b[i] = 'x';
  b[MAXPATH] = '\0';

  पूर्णाङ्क ret = unlink(b);
  यदि (ret != -1) {
    म_लिखो("unlink(%s) returned %d, not -1\n", b, ret);
    निकास(1);
  }

  पूर्णाङ्क fd = खोलो(b, O_CREATE | O_WRONLY);
  यदि (fd != -1) {
    म_लिखो("open(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  ret = link(b, b);
  यदि (ret != -1) {
    म_लिखो("link(%s, %s) returned %d, not -1\n", b, b, ret);
    निकास(1);
  }

  वर्ण *args[] = {"xx", 0};
  ret = exec(b, args);
  यदि (ret != -1) {
    म_लिखो("exec(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  पूर्णाङ्क pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  यदि (pid == 0) {
    स्थिर वर्ण big[PGSIZE + 1];
    प्रति (पूर्णाङ्क i = 0; i < PGSIZE; i++)
      big[i] = 'x';
    big[PGSIZE] = '\0';
    वर्ण *args2[] = {big, big, big, 0};
    ret = exec("echo", args2);
    यदि (ret != -1) {
      म_लिखो("exec(echo, BIG) returned %d, not -1\n", fd);
      निकास(1);
    }
    निकास(747); // OK
  }

  पूर्णाङ्क st = 0;
  रुको(&st);
  यदि (st != 747) {
    म_लिखो("exec(echo, BIG) succeeded, should have failed\n");
    निकास(1);
  }
}

// what if a string argument crosses over the end of last user page?
शून्य
copyinstr3(वर्ण *s)
{
  sbrk(8192);
  uint64 शीर्ष = (uint64)sbrk(0);
  यदि ((शीर्ष % PGSIZE) != 0) {
    sbrk(PGSIZE - (शीर्ष % PGSIZE));
  }
  शीर्ष = (uint64)sbrk(0);
  यदि (शीर्ष % PGSIZE) {
    म_लिखो("oops\n");
    निकास(1);
  }

  वर्ण *b = (वर्ण *)(शीर्ष - 1);
  *b = 'x';

  पूर्णाङ्क ret = unlink(b);
  यदि (ret != -1) {
    म_लिखो("unlink(%s) returned %d, not -1\n", b, ret);
    निकास(1);
  }

  पूर्णाङ्क fd = खोलो(b, O_CREATE | O_WRONLY);
  यदि (fd != -1) {
    म_लिखो("open(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }

  ret = link(b, b);
  यदि (ret != -1) {
    म_लिखो("link(%s, %s) returned %d, not -1\n", b, b, ret);
    निकास(1);
  }

  वर्ण *args[] = {"xx", 0};
  ret = exec(b, args);
  यदि (ret != -1) {
    म_लिखो("exec(%s) returned %d, not -1\n", b, fd);
    निकास(1);
  }
}

// See if the kernel refuses to read/write user memory that the
// application doesn't have anymore, because it returned it.
शून्य
rwsbrk(वर्ण *s)
{
  पूर्णाङ्क fd, n;

  uint64 a = (uint64)sbrk(8192);

  यदि (a == (uint64)SBRK_ERROR) {
    म_लिखो("sbrk(rwsbrk) failed\n");
    निकास(1);
  }

  यदि (sbrk(-8192) == SBRK_ERROR) {
    म_लिखो("sbrk(rwsbrk) shrink failed\n");
    निकास(1);
  }

  fd = खोलो("rwsbrk", O_CREATE | O_WRONLY);
  यदि (fd < 0) {
    म_लिखो("open(rwsbrk) failed\n");
    निकास(1);
  }
  n = लिख(fd, (शून्य *)(a + PGSIZE), 1024);
  यदि (n >= 0) {
    म_लिखो("write(fd, %p, 1024) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    निकास(1);
  }
  बंद(fd);
  unlink("rwsbrk");

  fd = खोलो("README", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("open(README) failed\n");
    निकास(1);
  }
  n = पठ(fd, (शून्य *)(a + PGSIZE), 10);
  यदि (n >= 0) {
    म_लिखो("read(fd, %p, 10) returned %d, not -1\n", (शून्य *)a + PGSIZE, n);
    निकास(1);
  }
  बंद(fd);

  निकास(0);
}

// test O_TRUNC.
शून्य
truncate1(वर्ण *s)
{
  वर्ण buf[32];

  unlink("truncfile");
  पूर्णाङ्क fd1 = खोलो("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
  लिख(fd1, "abcd", 4);
  बंद(fd1);

  पूर्णाङ्क fd2 = खोलो("truncfile", O_RDONLY);
  पूर्णाङ्क n = पठ(fd2, buf, परिमाण(buf));
  यदि (n != 4) {
    म_लिखो("%s: read %d bytes, wanted 4\n", s, n);
    निकास(1);
  }

  fd1 = खोलो("truncfile", O_WRONLY | O_TRUNC);

  पूर्णाङ्क fd3 = खोलो("truncfile", O_RDONLY);
  n = पठ(fd3, buf, परिमाण(buf));
  यदि (n != 0) {
    म_लिखो("aaa fd3=%d\n", fd3);
    म_लिखो("%s: read %d bytes, wanted 0\n", s, n);
    निकास(1);
  }

  n = पठ(fd2, buf, परिमाण(buf));
  यदि (n != 0) {
    म_लिखो("bbb fd2=%d\n", fd2);
    म_लिखो("%s: read %d bytes, wanted 0\n", s, n);
    निकास(1);
  }

  लिख(fd1, "abcdef", 6);

  n = पठ(fd3, buf, परिमाण(buf));
  यदि (n != 6) {
    म_लिखो("%s: read %d bytes, wanted 6\n", s, n);
    निकास(1);
  }

  n = पठ(fd2, buf, परिमाण(buf));
  यदि (n != 2) {
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
truncate2(वर्ण *s)
{
  unlink("truncfile");

  पूर्णाङ्क fd1 = खोलो("truncfile", O_CREATE | O_TRUNC | O_WRONLY);
  लिख(fd1, "abcd", 4);

  पूर्णाङ्क fd2 = खोलो("truncfile", O_TRUNC | O_WRONLY);

  पूर्णाङ्क n = लिख(fd1, "x", 1);
  यदि (n != -1) {
    म_लिखो("%s: write returned %d, expected -1\n", s, n);
    निकास(1);
  }

  unlink("truncfile");
  बंद(fd1);
  बंद(fd2);
}

शून्य
truncate3(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  बंद(खोलो("truncfile", O_CREATE | O_TRUNC | O_WRONLY));

  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }

  यदि (pid == 0) {
    प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
      वर्ण buf[32];
      पूर्णाङ्क fd = खोलो("truncfile", O_WRONLY);
      यदि (fd < 0) {
        म_लिखो("%s: open failed\n", s);
        निकास(1);
      }
      पूर्णाङ्क n = लिख(fd, "1234567890", 10);
      यदि (n != 10) {
        म_लिखो("%s: write got %d, expected 10\n", s, n);
        निकास(1);
      }
      बंद(fd);
      fd = खोलो("truncfile", O_RDONLY);
      पठ(fd, buf, परिमाण(buf));
      बंद(fd);
    }
    निकास(0);
  }

  प्रति (पूर्णाङ्क i = 0; i < 150; i++) {
    पूर्णाङ्क fd = खोलो("truncfile", O_CREATE | O_WRONLY | O_TRUNC);
    यदि (fd < 0) {
      म_लिखो("%s: open failed\n", s);
      निकास(1);
    }
    पूर्णाङ्क n = लिख(fd, "xxx", 3);
    यदि (n != 3) {
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
iputtest(वर्ण *s)
{
  यदि (सूची_गढ़ो("iputdir") < 0) {
    म_लिखो("%s: mkdir failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("iputdir") < 0) {
    म_लिखो("%s: chdir iputdir failed\n", s);
    निकास(1);
  }
  यदि (unlink("../iputdir") < 0) {
    म_लिखो("%s: unlink ../iputdir failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("/") < 0) {
    म_लिखो("%s: chdir / failed\n", s);
    निकास(1);
  }
}

// does exit() call iput(p->cwd) in a transaction?
शून्य
exitiputtest(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid == 0) {
    यदि (सूची_गढ़ो("iputdir") < 0) {
      म_लिखो("%s: mkdir failed\n", s);
      निकास(1);
    }
    यदि (स_बदलो("iputdir") < 0) {
      म_लिखो("%s: child chdir failed\n", s);
      निकास(1);
    }
    यदि (unlink("../iputdir") < 0) {
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
openiputtest(वर्ण *s)
{
  पूर्णाङ्क pid, xstatus;

  यदि (सूची_गढ़ो("oidir") < 0) {
    म_लिखो("%s: mkdir oidir failed\n", s);
    निकास(1);
  }
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid == 0) {
    पूर्णाङ्क fd = खोलो("oidir", O_RDWR);
    यदि (fd >= 0) {
      म_लिखो("%s: open directory for write succeeded\n", s);
      निकास(1);
    }
    निकास(0);
  }
  छोड़ो(1);
  यदि (unlink("oidir") != 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  रुको(&xstatus);
  निकास(xstatus);
}

// simple file system tests

शून्य
opentest(वर्ण *s)
{
  पूर्णाङ्क fd;

  fd = खोलो("echo", 0);
  यदि (fd < 0) {
    म_लिखो("%s: open echo failed!\n", s);
    निकास(1);
  }
  बंद(fd);
  fd = खोलो("doesnotexist", 0);
  यदि (fd >= 0) {
    म_लिखो("%s: open doesnotexist succeeded!\n", s);
    निकास(1);
  }
}

शून्य
writetest(वर्ण *s)
{
  पूर्णाङ्क fd;
  पूर्णाङ्क i;
  गणना { N = 100, SZ = 10 };

  fd = खोलो("small", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: error: creat small failed!\n", s);
    निकास(1);
  }
  प्रति (i = 0; i < N; i++) {
    यदि (लिख(fd, "aaaaaaaaaa", SZ) != SZ) {
      म_लिखो("%s: error: write aa %d new file failed\n", s, i);
      निकास(1);
    }
    यदि (लिख(fd, "bbbbbbbbbb", SZ) != SZ) {
      म_लिखो("%s: error: write bb %d new file failed\n", s, i);
      निकास(1);
    }
  }
  बंद(fd);
  fd = खोलो("small", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: error: open small failed!\n", s);
    निकास(1);
  }
  i = पठ(fd, buf, N * SZ * 2);
  यदि (i != N * SZ * 2) {
    म_लिखो("%s: read failed\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (unlink("small") < 0) {
    म_लिखो("%s: unlink small failed\n", s);
    निकास(1);
  }
}

शून्य
writebig(वर्ण *s)
{
  पूर्णाङ्क i, fd, n;

  fd = खोलो("big", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: error: creat big failed!\n", s);
    निकास(1);
  }

  प्रति (i = 0; i < MAXFILE; i++) {
    ((पूर्णाङ्क *)buf)[0] = i;
    यदि (लिख(fd, buf, BSIZE) != BSIZE) {
      म_लिखो("%s: error: write big file failed i=%d\n", s, i);
      निकास(1);
    }
  }

  बंद(fd);

  fd = खोलो("big", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: error: open big failed!\n", s);
    निकास(1);
  }

  n = 0;
  प्रति (;;) {
    i = पठ(fd, buf, BSIZE);
    यदि (i == 0) {
      यदि (n != MAXFILE) {
        म_लिखो("%s: read only %d blocks from big", s, n);
        निकास(1);
      }
      भङ्ग;
    } अन्यथा यदि (i != BSIZE) {
      म_लिखो("%s: read failed %d\n", s, i);
      निकास(1);
    }
    यदि (((पूर्णाङ्क *)buf)[0] != n) {
      म_लिखो("%s: read content of block %d is %d\n", s, n, ((पूर्णाङ्क *)buf)[0]);
      निकास(1);
    }
    n++;
  }
  बंद(fd);
  यदि (unlink("big") < 0) {
    म_लिखो("%s: unlink big failed\n", s);
    निकास(1);
  }
}

// many creates, followed by unlink test
शून्य
createtest(वर्ण *s)
{
  पूर्णाङ्क i, fd;
  गणना { N = 52 };

  वर्ण name[3];
  name[0] = 'a';
  name[2] = '\0';
  प्रति (i = 0; i < N; i++) {
    name[1] = '0' + i;
    fd = खोलो(name, O_CREATE | O_RDWR);
    बंद(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  प्रति (i = 0; i < N; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

शून्य
dirtest(वर्ण *s)
{
  यदि (सूची_गढ़ो("dir0") < 0) {
    म_लिखो("%s: mkdir failed\n", s);
    निकास(1);
  }

  यदि (स_बदलो("dir0") < 0) {
    म_लिखो("%s: chdir dir0 failed\n", s);
    निकास(1);
  }

  यदि (स_बदलो("..") < 0) {
    म_लिखो("%s: chdir .. failed\n", s);
    निकास(1);
  }

  यदि (unlink("dir0") < 0) {
    म_लिखो("%s: unlink dir0 failed\n", s);
    निकास(1);
  }
}

शून्य
exectest(वर्ण *s)
{
  पूर्णाङ्क fd, xstatus, pid;
  वर्ण *echoargv[] = {"echo", "OK", 0};
  वर्ण buf[3];

  unlink("echo-ok");
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid == 0) {
    पूर्णाङ्क errfd = dup(1);
    यदि (errfd < 0) {
      म_लिखो("%s: dup failed\n", s);
      निकास(1);
    }
    बंद(1);
    fd = खोलो("echo-ok", O_CREATE | O_WRONLY);
    यदि (fd < 0) {
      ख_लिखो(errfd, "%s: create failed\n", s);
      निकास(1);
    }
    यदि (fd != 1) {
      ख_लिखो(errfd, "%s: wrong fd\n", s);
      निकास(1);
    }
    यदि (exec("echo", echoargv) < 0) {
      ख_लिखो(errfd, "%s: exec echo failed\n", s);
      निकास(1);
    }
    // won't get to here
  }
  यदि (रुको(&xstatus) != pid) {
    म_लिखो("%s: wait failed!\n", s);
  }
  यदि (xstatus != 0) {
    म_लिखो("%s: nonzero wait status %d\n", s, xstatus);
    निकास(1);
  }

  fd = खोलो("echo-ok", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  यदि (पठ(fd, buf, 2) != 2) {
    म_लिखो("%s: read failed\n", s);
    निकास(1);
  }
  unlink("echo-ok");
  यदि (buf[0] == 'O' && buf[1] == 'K')
    निकास(0);
  अन्यथा {
    म_लिखो("%s: wrong output\n", s);
    निकास(1);
  }
}

// simple fork and pipe read/write

शून्य
pipe1(वर्ण *s)
{
  पूर्णाङ्क fds[2], pid, xstatus;
  पूर्णाङ्क seq, i, n, cc, total;
  गणना { N = 5, SZ = 1033 };

  यदि (pipe(fds) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
  pid = विभाजन();
  seq = 0;
  यदि (pid == 0) {
    बंद(fds[0]);
    प्रति (n = 0; n < N; n++) {
      प्रति (i = 0; i < SZ; i++)
        buf[i] = seq++;
      यदि (लिख(fds[1], buf, SZ) != SZ) {
        म_लिखो("%s: pipe1 oops 1\n", s);
        निकास(1);
      }
    }
    निकास(0);
  } अन्यथा यदि (pid > 0) {
    बंद(fds[1]);
    total = 0;
    cc = 1;
    यावत् ((n = पठ(fds[0], buf, cc)) > 0) {
      प्रति (i = 0; i < n; i++) {
        यदि ((buf[i] & 0xff) != (seq++ & 0xff)) {
          म_लिखो("%s: pipe1 oops 2\n", s);
          प्रत्यागम;
        }
      }
      total += n;
      cc = cc * 2;
      यदि (cc > परिमाण(buf))
        cc = परिमाण(buf);
    }
    यदि (total != N * SZ) {
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
killstatus(वर्ण *s)
{
  पूर्णाङ्क xst;

  प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
    पूर्णाङ्क pid1 = विभाजन();
    यदि (pid1 < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid1 == 0) {
      यावत् (1) {
        getpid();
      }
      निकास(0);
    }
    छोड़ो(1);
    समाप्त(pid1);
    रुको(&xst);
    यदि (xst != -1) {
      म_लिखो("%s: status should be -1\n", s);
      निकास(1);
    }
  }
  निकास(0);
}

// test that kill on unused process 0 doesn't set the killed flag
शून्य
killzero(वर्ण *s)
{
  पूर्णाङ्क pid, xst;

  समाप्त(0);

  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid == 0) {
    निकास(7);
  }
  यदि (रुको(&xst) != pid) {
    म_लिखो("%s: wait wrong pid\n", s);
    निकास(1);
  }
  यदि (xst != 7) {
    म_लिखो("%s: child exited with status %d, expected 7\n", s, xst);
    निकास(1);
  }
  निकास(0);
}

// meant to be run w/ at most two CPUs
शून्य
preempt(वर्ण *s)
{
  पूर्णाङ्क pid1, pid2, pid3;
  पूर्णाङ्क pfds[2];

  pid1 = विभाजन();
  यदि (pid1 < 0) {
    म_लिखो("%s: fork failed", s);
    निकास(1);
  }
  यदि (pid1 == 0)
    प्रति (;;)
      ;

  pid2 = विभाजन();
  यदि (pid2 < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid2 == 0)
    प्रति (;;)
      ;

  pipe(pfds);
  pid3 = विभाजन();
  यदि (pid3 < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid3 == 0) {
    बंद(pfds[0]);
    यदि (लिख(pfds[1], "x", 1) != 1)
      म_लिखो("%s: preempt write error", s);
    बंद(pfds[1]);
    प्रति (;;)
      ;
  }

  बंद(pfds[1]);
  यदि (पठ(pfds[0], buf, परिमाण(buf)) != 1) {
    म_लिखो("%s: preempt read error", s);
    प्रत्यागम;
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
exitwait(वर्ण *s)
{
  पूर्णाङ्क i, pid;

  प्रति (i = 0; i < 100; i++) {
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid) {
      पूर्णाङ्क xstate;
      यदि (रुको(&xstate) != pid) {
        म_लिखो("%s: wait wrong pid\n", s);
        निकास(1);
      }
      यदि (i != xstate) {
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
reparent(वर्ण *s)
{
  पूर्णाङ्क master_pid = getpid();
  प्रति (पूर्णाङ्क i = 0; i < 200; i++) {
    पूर्णाङ्क pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid) {
      यदि (रुको(0) != pid) {
        म_लिखो("%s: wait wrong pid\n", s);
        निकास(1);
      }
    } अन्यथा {
      पूर्णाङ्क pid2 = विभाजन();
      यदि (pid2 < 0) {
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
twochildren(वर्ण *s)
{
  प्रति (पूर्णाङ्क i = 0; i < 1000; i++) {
    पूर्णाङ्क pid1 = विभाजन();
    यदि (pid1 < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid1 == 0) {
      निकास(0);
    } अन्यथा {
      पूर्णाङ्क pid2 = विभाजन();
      यदि (pid2 < 0) {
        म_लिखो("%s: fork failed\n", s);
        निकास(1);
      }
      यदि (pid2 == 0) {
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
forkfork(वर्ण *s)
{
  गणना { N = 2 };

  प्रति (पूर्णाङ्क i = 0; i < N; i++) {
    पूर्णाङ्क pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed", s);
      निकास(1);
    }
    यदि (pid == 0) {
      प्रति (पूर्णाङ्क j = 0; j < 200; j++) {
        पूर्णाङ्क pid1 = विभाजन();
        यदि (pid1 < 0) {
          निकास(1);
        }
        यदि (pid1 == 0) {
          निकास(0);
        }
        रुको(0);
      }
      निकास(0);
    }
  }

  पूर्णाङ्क xstatus;
  प्रति (पूर्णाङ्क i = 0; i < N; i++) {
    रुको(&xstatus);
    यदि (xstatus != 0) {
      म_लिखो("%s: fork in child failed", s);
      निकास(1);
    }
  }
}

शून्य
forkforkfork(वर्ण *s)
{
  unlink("stopforking");

  पूर्णाङ्क pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed", s);
    निकास(1);
  }
  यदि (pid == 0) {
    यावत् (1) {
      पूर्णाङ्क fd = खोलो("stopforking", 0);
      यदि (fd >= 0) {
        निकास(0);
      }
      यदि (विभाजन() < 0) {
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
reparent2(वर्ण *s)
{
  प्रति (पूर्णाङ्क i = 0; i < 800; i++) {
    पूर्णाङ्क pid1 = विभाजन();
    यदि (pid1 < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    }
    यदि (pid1 == 0) {
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
mem(वर्ण *s)
{
  शून्य *m1, *m2;
  पूर्णाङ्क pid;

  यदि ((pid = विभाजन()) == 0) {
    m1 = 0;
    यावत् ((m2 = दो_स्मृति(10001)) != 0) {
      *(वर्ण **)m2 = m1;
      m1 = m2;
    }
    यावत् (m1) {
      m2 = *(वर्ण **)m1;
      मुक्त(m1);
      m1 = m2;
    }
    m1 = दो_स्मृति(1024 * 20);
    यदि (m1 == 0) {
      म_लिखो("%s: couldn't allocate mem?!!\n", s);
      निकास(1);
    }
    मुक्त(m1);
    निकास(0);
  } अन्यथा {
    पूर्णाङ्क xstatus;
    रुको(&xstatus);
    यदि (xstatus == -1) {
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
sharedfd(वर्ण *s)
{
  पूर्णाङ्क fd, pid, i, n, nc, np;
  गणना { N = 1000, SZ = 10 };
  वर्ण buf[SZ];

  unlink("sharedfd");
  fd = खोलो("sharedfd", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: cannot open sharedfd for writing", s);
    निकास(1);
  }
  pid = विभाजन();
  स_रखो(buf, pid == 0 ? 'c' : 'p', परिमाण(buf));
  प्रति (i = 0; i < N; i++) {
    यदि (लिख(fd, buf, परिमाण(buf)) != परिमाण(buf)) {
      म_लिखो("%s: write sharedfd failed\n", s);
      निकास(1);
    }
  }
  यदि (pid == 0) {
    निकास(0);
  } अन्यथा {
    पूर्णाङ्क xstatus;
    रुको(&xstatus);
    यदि (xstatus != 0)
      निकास(xstatus);
  }

  बंद(fd);
  fd = खोलो("sharedfd", 0);
  यदि (fd < 0) {
    म_लिखो("%s: cannot open sharedfd for reading\n", s);
    निकास(1);
  }
  nc = np = 0;
  यावत् ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    प्रति (i = 0; i < परिमाण(buf); i++) {
      यदि (buf[i] == 'c')
        nc++;
      यदि (buf[i] == 'p')
        np++;
    }
  }
  बंद(fd);
  unlink("sharedfd");
  यदि (nc == N * SZ && np == N * SZ) {
    निकास(0);
  } अन्यथा {
    म_लिखो("%s: nc/np test fails\n", s);
    निकास(1);
  }
}

// four processes write different files at the same
// time, to test block allocation.
शून्य
fourfiles(वर्ण *s)
{
  पूर्णाङ्क fd, pid, i, j, n, total, pi;
  वर्ण *names[] = {"f0", "f1", "f2", "f3"};
  वर्ण *fname;
  गणना { N = 12, NCHILD = 4, SZ = 500 };

  प्रति (pi = 0; pi < NCHILD; pi++) {
    fname = names[pi];
    unlink(fname);

    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }

    यदि (pid == 0) {
      fd = खोलो(fname, O_CREATE | O_RDWR);
      यदि (fd < 0) {
        म_लिखो("%s: create failed\n", s);
        निकास(1);
      }

      स_रखो(buf, '0' + pi, SZ);
      प्रति (i = 0; i < N; i++) {
        यदि ((n = लिख(fd, buf, SZ)) != SZ) {
          म_लिखो("write failed %d\n", n);
          निकास(1);
        }
      }
      निकास(0);
    }
  }

  पूर्णाङ्क xstatus;
  प्रति (pi = 0; pi < NCHILD; pi++) {
    रुको(&xstatus);
    यदि (xstatus != 0)
      निकास(xstatus);
  }

  प्रति (i = 0; i < NCHILD; i++) {
    fname = names[i];
    fd = खोलो(fname, 0);
    total = 0;
    यावत् ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
      प्रति (j = 0; j < n; j++) {
        यदि (buf[j] != '0' + i) {
          म_लिखो("%s: wrong char\n", s);
          निकास(1);
        }
      }
      total += n;
    }
    बंद(fd);
    यदि (total != N * SZ) {
      म_लिखो("wrong length %d\n", total);
      निकास(1);
    }
    unlink(fname);
  }
}

// four processes create and delete different files in same directory
शून्य
createdelete(वर्ण *s)
{
  गणना { N = 20, NCHILD = 4 };
  पूर्णाङ्क pid, i, fd, pi;
  वर्ण name[32];

  प्रति (pi = 0; pi < NCHILD; pi++) {
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }

    यदि (pid == 0) {
      name[0] = 'p' + pi;
      name[2] = '\0';
      प्रति (i = 0; i < N; i++) {
        name[1] = '0' + i;
        fd = खोलो(name, O_CREATE | O_RDWR);
        यदि (fd < 0) {
          म_लिखो("%s: create failed\n", s);
          निकास(1);
        }
        बंद(fd);
        यदि (i > 0 && (i % 2) == 0) {
          name[1] = '0' + (i / 2);
          यदि (unlink(name) < 0) {
            म_लिखो("%s: unlink failed\n", s);
            निकास(1);
          }
        }
      }
      निकास(0);
    }
  }

  पूर्णाङ्क xstatus;
  प्रति (pi = 0; pi < NCHILD; pi++) {
    रुको(&xstatus);
    यदि (xstatus != 0)
      निकास(1);
  }

  name[0] = name[1] = name[2] = 0;
  प्रति (i = 0; i < N; i++) {
    प्रति (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = खोलो(name, 0);
      यदि ((i == 0 || i >= N / 2) && fd < 0) {
        म_लिखो("%s: oops createdelete %s didn't exist\n", s, name);
        निकास(1);
      } अन्यथा यदि ((i >= 1 && i < N / 2) && fd >= 0) {
        म_लिखो("%s: oops createdelete %s did exist\n", s, name);
        निकास(1);
      }
      यदि (fd >= 0)
        बंद(fd);
    }
  }

  प्रति (i = 0; i < N; i++) {
    प्रति (pi = 0; pi < NCHILD; pi++) {
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      unlink(name);
    }
  }
}

// can I unlink a file and still read it?
शून्य
unlinkread(वर्ण *s)
{
  गणना { SZ = 5 };
  पूर्णाङ्क fd, fd1;

  fd = खोलो("unlinkread", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: create unlinkread failed\n", s);
    निकास(1);
  }
  लिख(fd, "hello", SZ);
  बंद(fd);

  fd = खोलो("unlinkread", O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: open unlinkread failed\n", s);
    निकास(1);
  }
  यदि (unlink("unlinkread") != 0) {
    म_लिखो("%s: unlink unlinkread failed\n", s);
    निकास(1);
  }

  fd1 = खोलो("unlinkread", O_CREATE | O_RDWR);
  लिख(fd1, "yyy", 3);
  बंद(fd1);

  यदि (पठ(fd, buf, परिमाण(buf)) != SZ) {
    म_लिखो("%s: unlinkread read failed", s);
    निकास(1);
  }
  यदि (buf[0] != 'h') {
    म_लिखो("%s: unlinkread wrong data\n", s);
    निकास(1);
  }
  यदि (लिख(fd, buf, 10) != 10) {
    म_लिखो("%s: unlinkread write failed\n", s);
    निकास(1);
  }
  बंद(fd);
  unlink("unlinkread");
}

शून्य
linktest(वर्ण *s)
{
  गणना { SZ = 5 };
  पूर्णाङ्क fd;

  unlink("lf1");
  unlink("lf2");

  fd = खोलो("lf1", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: create lf1 failed\n", s);
    निकास(1);
  }
  यदि (लिख(fd, "hello", SZ) != SZ) {
    म_लिखो("%s: write lf1 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (link("lf1", "lf2") < 0) {
    म_लिखो("%s: link lf1 lf2 failed\n", s);
    निकास(1);
  }
  unlink("lf1");

  यदि (खोलो("lf1", 0) >= 0) {
    म_लिखो("%s: unlinked lf1 but it is still there!\n", s);
    निकास(1);
  }

  fd = खोलो("lf2", 0);
  यदि (fd < 0) {
    म_लिखो("%s: open lf2 failed\n", s);
    निकास(1);
  }
  यदि (पठ(fd, buf, परिमाण(buf)) != SZ) {
    म_लिखो("%s: read lf2 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (link("lf2", "lf2") >= 0) {
    म_लिखो("%s: link lf2 lf2 succeeded! oops\n", s);
    निकास(1);
  }

  unlink("lf2");
  यदि (link("lf2", "lf1") >= 0) {
    म_लिखो("%s: link non-existent succeeded! oops\n", s);
    निकास(1);
  }

  यदि (link(".", "lf1") >= 0) {
    म_लिखो("%s: link . lf1 succeeded! oops\n", s);
    निकास(1);
  }
}

// test concurrent create/link/unlink of the same file
शून्य
concreate(वर्ण *s)
{
  गणना { N = 40 };
  वर्ण file[3];
  पूर्णाङ्क i, pid, n, fd;
  वर्ण fa[N];
  संरचना {
    ushort inum;
    वर्ण name[DIRSIZ];
  } de;

  file[0] = 'C';
  file[2] = '\0';
  प्रति (i = 0; i < N; i++) {
    file[1] = '0' + i;
    unlink(file);
    pid = विभाजन();
    यदि (pid && (i % 3) == 1) {
      link("C0", file);
    } अन्यथा यदि (pid == 0 && (i % 5) == 1) {
      link("C0", file);
    } अन्यथा {
      fd = खोलो(file, O_CREATE | O_RDWR);
      यदि (fd < 0) {
        म_लिखो("concreate create %s failed\n", file);
        निकास(1);
      }
      बंद(fd);
    }
    यदि (pid == 0) {
      निकास(0);
    } अन्यथा {
      पूर्णाङ्क xstatus;
      रुको(&xstatus);
      यदि (xstatus != 0)
        निकास(1);
    }
  }

  स_रखो(fa, 0, परिमाण(fa));
  fd = खोलो(".", 0);
  n = 0;
  यावत् (पठ(fd, &de, परिमाण(de)) > 0) {
    यदि (de.inum == 0)
      अनुवर्त;
    यदि (de.name[0] == 'C' && de.name[2] == '\0') {
      i = de.name[1] - '0';
      यदि (i < 0 || i >= परिमाण(fa)) {
        म_लिखो("%s: concreate weird file %s\n", s, de.name);
        निकास(1);
      }
      यदि (fa[i]) {
        म_लिखो("%s: concreate duplicate file %s\n", s, de.name);
        निकास(1);
      }
      fa[i] = 1;
      n++;
    }
  }
  बंद(fd);

  यदि (n != N) {
    म_लिखो("%s: concreate not enough files in directory listing\n", s);
    निकास(1);
  }

  प्रति (i = 0; i < N; i++) {
    file[1] = '0' + i;
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (((i % 3) == 0 && pid == 0) || ((i % 3) == 1 && pid != 0)) {
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
    यदि (pid == 0)
      निकास(0);
    अन्यथा
      रुको(0);
  }
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
शून्य
linkunlink(वर्ण *s)
{
  पूर्णाङ्क pid, i;

  unlink("x");
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }

  अचिन्हित पूर्णाङ्क x = (pid ? 1 : 97);
  प्रति (i = 0; i < 100; i++) {
    x = x * 1103515245 + 12345;
    यदि ((x % 3) == 0) {
      बंद(खोलो("x", O_RDWR | O_CREATE));
    } अन्यथा यदि ((x % 3) == 1) {
      link("cat", "x");
    } अन्यथा {
      unlink("x");
    }
  }

  यदि (pid)
    रुको(0);
  अन्यथा
    निकास(0);
}

शून्य
subdir(वर्ण *s)
{
  पूर्णाङ्क fd, cc;

  unlink("ff");
  यदि (सूची_गढ़ो("dd") != 0) {
    म_लिखो("%s: mkdir dd failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/ff", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: create dd/ff failed\n", s);
    निकास(1);
  }
  लिख(fd, "ff", 2);
  बंद(fd);

  यदि (unlink("dd") >= 0) {
    म_लिखो("%s: unlink dd (non-empty dir) succeeded!\n", s);
    निकास(1);
  }

  यदि (सूची_गढ़ो("/dd/dd") != 0) {
    म_लिखो("%s: subdir mkdir dd/dd failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/dd/ff", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: create dd/dd/ff failed\n", s);
    निकास(1);
  }
  लिख(fd, "FF", 2);
  बंद(fd);

  fd = खोलो("dd/dd/../ff", 0);
  यदि (fd < 0) {
    म_लिखो("%s: open dd/dd/../ff failed\n", s);
    निकास(1);
  }
  cc = पठ(fd, buf, परिमाण(buf));
  यदि (cc != 2 || buf[0] != 'f') {
    म_लिखो("%s: dd/dd/../ff wrong content\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (link("dd/dd/ff", "dd/dd/ffff") != 0) {
    म_लिखो("%s: link dd/dd/ff dd/dd/ffff failed\n", s);
    निकास(1);
  }

  यदि (unlink("dd/dd/ff") != 0) {
    म_लिखो("%s: unlink dd/dd/ff failed\n", s);
    निकास(1);
  }
  यदि (खोलो("dd/dd/ff", O_RDONLY) >= 0) {
    म_लिखो("%s: open (unlinked) dd/dd/ff succeeded\n", s);
    निकास(1);
  }

  यदि (स_बदलो("dd") != 0) {
    म_लिखो("%s: chdir dd failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("dd/../../dd") != 0) {
    म_लिखो("%s: chdir dd/../../dd failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("dd/../../../dd") != 0) {
    म_लिखो("%s: chdir dd/../../../dd failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("./..") != 0) {
    म_लिखो("%s: chdir ./.. failed\n", s);
    निकास(1);
  }

  fd = खोलो("dd/dd/ffff", 0);
  यदि (fd < 0) {
    म_लिखो("%s: open dd/dd/ffff failed\n", s);
    निकास(1);
  }
  यदि (पठ(fd, buf, परिमाण(buf)) != 2) {
    म_लिखो("%s: read dd/dd/ffff wrong len\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (खोलो("dd/dd/ff", O_RDONLY) >= 0) {
    म_लिखो("%s: open (unlinked) dd/dd/ff succeeded!\n", s);
    निकास(1);
  }

  यदि (खोलो("dd/ff/ff", O_CREATE | O_RDWR) >= 0) {
    म_लिखो("%s: create dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (खोलो("dd/xx/ff", O_CREATE | O_RDWR) >= 0) {
    म_लिखो("%s: create dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (खोलो("dd", O_CREATE) >= 0) {
    म_लिखो("%s: create dd succeeded!\n", s);
    निकास(1);
  }
  यदि (खोलो("dd", O_RDWR) >= 0) {
    म_लिखो("%s: open dd rdwr succeeded!\n", s);
    निकास(1);
  }
  यदि (खोलो("dd", O_WRONLY) >= 0) {
    म_लिखो("%s: open dd wronly succeeded!\n", s);
    निकास(1);
  }
  यदि (link("dd/ff/ff", "dd/dd/xx") == 0) {
    म_लिखो("%s: link dd/ff/ff dd/dd/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (link("dd/xx/ff", "dd/dd/xx") == 0) {
    म_लिखो("%s: link dd/xx/ff dd/dd/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (link("dd/ff", "dd/dd/ffff") == 0) {
    म_लिखो("%s: link dd/ff dd/dd/ffff succeeded!\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("dd/ff/ff") == 0) {
    म_लिखो("%s: mkdir dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("dd/xx/ff") == 0) {
    म_लिखो("%s: mkdir dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("dd/dd/ffff") == 0) {
    म_लिखो("%s: mkdir dd/dd/ffff succeeded!\n", s);
    निकास(1);
  }
  यदि (unlink("dd/xx/ff") == 0) {
    म_लिखो("%s: unlink dd/xx/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (unlink("dd/ff/ff") == 0) {
    म_लिखो("%s: unlink dd/ff/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (स_बदलो("dd/ff") == 0) {
    म_लिखो("%s: chdir dd/ff succeeded!\n", s);
    निकास(1);
  }
  यदि (स_बदलो("dd/xx") == 0) {
    म_लिखो("%s: chdir dd/xx succeeded!\n", s);
    निकास(1);
  }

  यदि (unlink("dd/dd/ffff") != 0) {
    म_लिखो("%s: unlink dd/dd/ff failed\n", s);
    निकास(1);
  }
  यदि (unlink("dd/ff") != 0) {
    म_लिखो("%s: unlink dd/ff failed\n", s);
    निकास(1);
  }
  यदि (unlink("dd") == 0) {
    म_लिखो("%s: unlink non-empty dd succeeded!\n", s);
    निकास(1);
  }
  यदि (unlink("dd/dd") < 0) {
    म_लिखो("%s: unlink dd/dd failed\n", s);
    निकास(1);
  }
  यदि (unlink("dd") < 0) {
    म_लिखो("%s: unlink dd failed\n", s);
    निकास(1);
  }
}

// test writes that are larger than the log.
शून्य
bigwrite(वर्ण *s)
{
  पूर्णाङ्क fd, sz;

  unlink("bigwrite");
  प्रति (sz = 499; sz < (MAXOPBLOCKS + 2) * BSIZE; sz += 471) {
    fd = खोलो("bigwrite", O_CREATE | O_RDWR);
    यदि (fd < 0) {
      म_लिखो("%s: cannot create bigwrite\n", s);
      निकास(1);
    }
    पूर्णाङ्क i;
    प्रति (i = 0; i < 2; i++) {
      पूर्णाङ्क cc = लिख(fd, buf, sz);
      यदि (cc != sz) {
        म_लिखो("%s: write(%d) ret %d\n", s, sz, cc);
        निकास(1);
      }
    }
    बंद(fd);
    unlink("bigwrite");
  }
}

शून्य
bigfile(वर्ण *s)
{
  गणना { N = 20, SZ = 600 };
  पूर्णाङ्क fd, i, total, cc;

  unlink("bigfile.dat");
  fd = खोलो("bigfile.dat", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: cannot create bigfile", s);
    निकास(1);
  }
  प्रति (i = 0; i < N; i++) {
    स_रखो(buf, i, SZ);
    यदि (लिख(fd, buf, SZ) != SZ) {
      म_लिखो("%s: write bigfile failed\n", s);
      निकास(1);
    }
  }
  बंद(fd);

  fd = खोलो("bigfile.dat", 0);
  यदि (fd < 0) {
    म_लिखो("%s: cannot open bigfile\n", s);
    निकास(1);
  }
  total = 0;
  प्रति (i = 0;; i++) {
    cc = पठ(fd, buf, SZ / 2);
    यदि (cc < 0) {
      म_लिखो("%s: read bigfile failed\n", s);
      निकास(1);
    }
    यदि (cc == 0)
      भङ्ग;
    यदि (cc != SZ / 2) {
      म_लिखो("%s: short read bigfile\n", s);
      निकास(1);
    }
    यदि (buf[0] != i / 2 || buf[SZ / 2 - 1] != i / 2) {
      म_लिखो("%s: read bigfile wrong data\n", s);
      निकास(1);
    }
    total += cc;
  }
  बंद(fd);
  यदि (total != N * SZ) {
    म_लिखो("%s: read bigfile wrong total\n", s);
    निकास(1);
  }
  unlink("bigfile.dat");
}

शून्य
fourteen(वर्ण *s)
{
  पूर्णाङ्क fd;

  // DIRSIZ is 14.

  यदि (सूची_गढ़ो("12345678901234") != 0) {
    म_लिखो("%s: mkdir 12345678901234 failed\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("12345678901234/123456789012345") != 0) {
    म_लिखो("%s: mkdir 12345678901234/123456789012345 failed\n", s);
    निकास(1);
  }
  fd = खोलो("123456789012345/123456789012345/123456789012345", O_CREATE);
  यदि (fd < 0) {
    म_लिखो(
      "%s: create 123456789012345/123456789012345/123456789012345 failed\n", s);
    निकास(1);
  }
  बंद(fd);
  fd = खोलो("12345678901234/12345678901234/12345678901234", 0);
  यदि (fd < 0) {
    म_लिखो("%s: open 12345678901234/12345678901234/12345678901234 failed\n", s);
    निकास(1);
  }
  बंद(fd);

  यदि (सूची_गढ़ो("12345678901234/12345678901234") == 0) {
    म_लिखो("%s: mkdir 12345678901234/12345678901234 succeeded!\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("123456789012345/12345678901234") == 0) {
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
rmdot(वर्ण *s)
{
  यदि (सूची_गढ़ो("dots") != 0) {
    म_लिखो("%s: mkdir dots failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("dots") != 0) {
    म_लिखो("%s: chdir dots failed\n", s);
    निकास(1);
  }
  यदि (unlink(".") == 0) {
    म_लिखो("%s: rm . worked!\n", s);
    निकास(1);
  }
  यदि (unlink("..") == 0) {
    म_लिखो("%s: rm .. worked!\n", s);
    निकास(1);
  }
  यदि (स_बदलो("/") != 0) {
    म_लिखो("%s: chdir / failed\n", s);
    निकास(1);
  }
  यदि (unlink("dots/.") == 0) {
    म_लिखो("%s: unlink dots/. worked!\n", s);
    निकास(1);
  }
  यदि (unlink("dots/..") == 0) {
    म_लिखो("%s: unlink dots/.. worked!\n", s);
    निकास(1);
  }
  यदि (unlink("dots") != 0) {
    म_लिखो("%s: unlink dots failed!\n", s);
    निकास(1);
  }
}

शून्य
dirfile(वर्ण *s)
{
  पूर्णाङ्क fd;

  fd = खोलो("dirfile", O_CREATE);
  यदि (fd < 0) {
    म_लिखो("%s: create dirfile failed\n", s);
    निकास(1);
  }
  बंद(fd);
  यदि (स_बदलो("dirfile") == 0) {
    म_लिखो("%s: chdir dirfile succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो("dirfile/xx", 0);
  यदि (fd >= 0) {
    म_लिखो("%s: create dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो("dirfile/xx", O_CREATE);
  यदि (fd >= 0) {
    म_लिखो("%s: create dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("dirfile/xx") == 0) {
    म_लिखो("%s: mkdir dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (unlink("dirfile/xx") == 0) {
    म_लिखो("%s: unlink dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (link("README", "dirfile/xx") == 0) {
    म_लिखो("%s: link to dirfile/xx succeeded!\n", s);
    निकास(1);
  }
  यदि (unlink("dirfile") != 0) {
    म_लिखो("%s: unlink dirfile failed!\n", s);
    निकास(1);
  }

  fd = खोलो(".", O_RDWR);
  यदि (fd >= 0) {
    म_लिखो("%s: open . for writing succeeded!\n", s);
    निकास(1);
  }
  fd = खोलो(".", 0);
  यदि (लिख(fd, "x", 1) > 0) {
    म_लिखो("%s: write . succeeded!\n", s);
    निकास(1);
  }
  बंद(fd);
}

// test that iput() is called at the end of _namei().
// also tests empty file names.
शून्य
iref(वर्ण *s)
{
  पूर्णाङ्क i, fd;

  प्रति (i = 0; i < NINODE + 1; i++) {
    यदि (सूची_गढ़ो("irefd") != 0) {
      म_लिखो("%s: mkdir irefd failed\n", s);
      निकास(1);
    }
    यदि (स_बदलो("irefd") != 0) {
      म_लिखो("%s: chdir irefd failed\n", s);
      निकास(1);
    }

    सूची_गढ़ो("");
    link("README", "");
    fd = खोलो("", O_CREATE);
    यदि (fd >= 0)
      बंद(fd);
    fd = खोलो("xx", O_CREATE);
    यदि (fd >= 0)
      बंद(fd);
    unlink("xx");
  }

  // clean up
  प्रति (i = 0; i < NINODE + 1; i++) {
    स_बदलो("..");
    unlink("irefd");
  }

  स_बदलो("/");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
शून्य
forktest(वर्ण *s)
{
  गणना { N = 1000 };
  पूर्णाङ्क n, pid;

  प्रति (n = 0; n < N; n++) {
    pid = विभाजन();
    यदि (pid < 0)
      भङ्ग;
    यदि (pid == 0)
      निकास(0);
  }

  यदि (n == 0) {
    म_लिखो("%s: no fork at all!\n", s);
    निकास(1);
  }

  यदि (n == N) {
    म_लिखो("%s: fork claimed to work 1000 times!\n", s);
    निकास(1);
  }

  प्रति (; n > 0; n--) {
    यदि (रुको(0) < 0) {
      म_लिखो("%s: wait stopped early\n", s);
      निकास(1);
    }
  }

  यदि (रुको(0) != -1) {
    म_लिखो("%s: wait got too many\n", s);
    निकास(1);
  }
}

शून्य
sbrkbasic(वर्ण *s)
{
  गणना { TOOMUCH = 1024 * 1024 * 1024 };
  पूर्णाङ्क i, pid, xstatus;
  वर्ण *c, *a, *b;

  // does sbrk() return the expected failure value?
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("fork failed in sbrkbasic\n");
    निकास(1);
  }
  यदि (pid == 0) {
    a = sbrk(TOOMUCH);
    यदि (a == (वर्ण *)SBRK_ERROR) {
      // it's OK if this fails.
      निकास(0);
    }

    प्रति (b = a; b < a + TOOMUCH; b += PGSIZE) {
      *b = 99;
    }

    // we should not get here! either sbrk(TOOMUCH)
    // should have failed, or (with lazy allocation)
    // a pagefault should have killed this process.
    निकास(1);
  }

  रुको(&xstatus);
  यदि (xstatus == 1) {
    म_लिखो("%s: too much memory allocated!\n", s);
    निकास(1);
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  प्रति (i = 0; i < 5000; i++) {
    b = sbrk(1);
    यदि (b != a) {
      म_लिखो("%s: sbrk test failed %d %p %p\n", s, i, a, b);
      निकास(1);
    }
    *b = 1;
    a = b + 1;
  }
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: sbrk test fork failed\n", s);
    निकास(1);
  }
  c = sbrk(1);
  c = sbrk(1);
  यदि (c != a + 1) {
    म_लिखो("%s: sbrk test failed post-fork\n", s);
    निकास(1);
  }
  यदि (pid == 0)
    निकास(0);
  रुको(&xstatus);
  निकास(xstatus);
}

शून्य
sbrkmuch(वर्ण *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  वर्ण *c, *oldbrk, *a, *lastaddr, *p;
  uint64 amt;

  oldbrk = sbrk(0);

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  यदि (p != a) {
    म_लिखो("%s: sbrk test failed to grow big address space; enough phys mem?\n",
           s);
    निकास(1);
  }

  lastaddr = (वर्ण *)(BIG - 1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  यदि (c == (वर्ण *)SBRK_ERROR) {
    म_लिखो("%s: sbrk could not deallocate\n", s);
    निकास(1);
  }
  c = sbrk(0);
  यदि (c != a - PGSIZE) {
    म_लिखो("%s: sbrk deallocation produced wrong address, a %p c %p\n", s, a,
           c);
    निकास(1);
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  यदि (c != a || sbrk(0) != a + PGSIZE) {
    म_लिखो("%s: sbrk re-allocation failed, a %p c %p\n", s, a, c);
    निकास(1);
  }
  यदि (*lastaddr == 99) {
    // should be zero
    म_लिखो("%s: sbrk de-allocation didn't really deallocate\n", s);
    निकास(1);
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  यदि (c != a) {
    म_लिखो("%s: sbrk downsize failed, a %p c %p\n", s, a, c);
    निकास(1);
  }
}

// can we read the kernel's memory?
शून्य
kernmem(वर्ण *s)
{
  वर्ण *a;
  पूर्णाङ्क pid;

  प्रति (a = (वर्ण *)(KERNBASE); a < (वर्ण *)(KERNBASE + 2000000); a += 50000) {
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid == 0) {
      म_लिखो("%s: oops could read %p = %x\n", s, a, *a);
      निकास(1);
    }
    पूर्णाङ्क xstatus;
    रुको(&xstatus);
    यदि (xstatus != -1) // did kernel kill child?
      निकास(1);
  }
}

// user code should not be able to write to addresses above MAXVA.
शून्य
MAXVAplus(वर्ण *s)
{
  चञ्चल uint64 a = MAXVA;
  प्रति (; a != 0; a <<= 1) {
    पूर्णाङ्क pid;
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    यदि (pid == 0) {
      *(वर्ण *)a = 99;
      म_लिखो("%s: oops wrote %p\n", s, (शून्य *)a);
      निकास(1);
    }
    पूर्णाङ्क xstatus;
    रुको(&xstatus);
    यदि (xstatus != -1) // did kernel kill child?
      निकास(1);
  }
}

// if we run the system out of memory, does it clean up the last
// failed allocation?
शून्य
sbrkfail(वर्ण *s)
{
  गणना { BIG = 100 * 1024 * 1024 };
  पूर्णाङ्क i, xstatus;
  पूर्णाङ्क fds[2];
  वर्ण scratch;
  वर्ण *c, *a;
  पूर्णाङ्क pids[10];
  पूर्णाङ्क pid;
  पूर्णाङ्क failed;

  failed = 0;
  यदि (pipe(fds) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
  प्रति (i = 0; i < परिमाण(pids) / परिमाण(pids[0]); i++) {
    यदि ((pids[i] = विभाजन()) == 0) {
      // allocate a lot of memory
      यदि (sbrk(BIG - (uint64)sbrk(0)) == (वर्ण *)SBRK_ERROR)
        लिख(fds[1], "0", 1);
      अन्यथा
        लिख(fds[1], "1", 1);
      // sit around until killed
      प्रति (;;)
        छोड़ो(1000);
    }
    यदि (pids[i] != -1) {
      पठ(fds[0], &scratch, 1);
      यदि (scratch == '0')
        failed = 1;
    }
  }
  यदि (!failed) {
    म_लिखो("%s: no allocation failed; allocate more?\n", s);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  प्रति (i = 0; i < परिमाण(pids) / परिमाण(pids[0]); i++) {
    यदि (pids[i] == -1)
      अनुवर्त;
    समाप्त(pids[i]);
    रुको(0);
  }
  यदि (c == (वर्ण *)SBRK_ERROR) {
    म_लिखो("%s: failed sbrk leaked memory\n", s);
    निकास(1);
  }

  // test running fork with the above allocated page
  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  यदि (pid == 0) {
    // allocate a lot of memory. this should produce an error
    a = sbrk(10 * BIG);
    यदि (a == (वर्ण *)SBRK_ERROR) {
      निकास(0);
    }
    म_लिखो("%s: allocate a lot of memory succeeded %d\n", s, 10 * BIG);
    निकास(1);
  }
  रुको(&xstatus);
  यदि (xstatus != 0)
    निकास(1);
}

// test reads/writes from/to allocated memory
शून्य
sbrkarg(वर्ण *s)
{
  वर्ण *a;
  पूर्णाङ्क fd, n;

  a = sbrk(PGSIZE);
  fd = खोलो("sbrk", O_CREATE | O_WRONLY);
  unlink("sbrk");
  यदि (fd < 0) {
    म_लिखो("%s: open sbrk failed\n", s);
    निकास(1);
  }
  यदि ((n = लिख(fd, a, PGSIZE)) < 0) {
    म_लिखो("%s: write sbrk failed\n", s);
    निकास(1);
  }
  बंद(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  यदि (pipe((पूर्णाङ्क *)a) != 0) {
    म_लिखो("%s: pipe() failed\n", s);
    निकास(1);
  }
}

शून्य
validatetest(वर्ण *s)
{
  पूर्णाङ्क hi;
  uint64 p;

  hi = 1100 * 1024;
  प्रति (p = 0; p <= (uint)hi; p += PGSIZE) {
    // try to crash the kernel by passing in a bad string pointer
    यदि (link("nosuchfile", (वर्ण *)p) != -1) {
      म_लिखो("%s: link should not succeed\n", s);
      निकास(1);
    }
  }
}

// does uninitialized data start out zero?
वर्ण uninit[10000];
शून्य
bsstest(वर्ण *s)
{
  पूर्णाङ्क i;

  प्रति (i = 0; i < परिमाण(uninit); i++) {
    यदि (uninit[i] != '\0') {
      म_लिखो("%s: bss test failed\n", s);
      निकास(1);
    }
  }
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
शून्य
bigargtest(वर्ण *s)
{
  पूर्णाङ्क pid, fd, xstatus;

  unlink("bigarg-ok");
  pid = विभाजन();
  यदि (pid == 0) {
    स्थिर वर्ण *args[MAXARG];
    पूर्णाङ्क i;
    वर्ण big[400];
    स_रखो(big, ' ', परिमाण(big));
    big[परिमाण(big) - 1] = '\0';
    प्रति (i = 0; i < MAXARG - 1; i++)
      args[i] = big;
    args[MAXARG - 1] = 0;
    // this exec() should fail (and return) because the
    // arguments are too large.
    exec("echo", args);
    fd = खोलो("bigarg-ok", O_CREATE);
    बंद(fd);
    निकास(0);
  } अन्यथा यदि (pid < 0) {
    म_लिखो("%s: bigargtest: fork failed\n", s);
    निकास(1);
  }

  रुको(&xstatus);
  यदि (xstatus != 0)
    निकास(xstatus);
  fd = खोलो("bigarg-ok", 0);
  यदि (fd < 0) {
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
  पूर्णाङ्क nfiles;
  पूर्णाङ्क fsblocks = 0;

  म_लिखो("fsfull test\n");

  प्रति (nfiles = 0;; nfiles++) {
    वर्ण name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    म_लिखो("writing %s\n", name);
    पूर्णाङ्क fd = खोलो(name, O_CREATE | O_RDWR);
    यदि (fd < 0) {
      म_लिखो("open %s failed\n", name);
      भङ्ग;
    }
    पूर्णाङ्क total = 0;
    यावत् (1) {
      पूर्णाङ्क cc = लिख(fd, buf, BSIZE);
      यदि (cc < BSIZE)
        भङ्ग;
      total += cc;
      fsblocks++;
    }
    म_लिखो("wrote %d bytes\n", total);
    बंद(fd);
    यदि (total == 0)
      भङ्ग;
  }

  यावत् (nfiles >= 0) {
    वर्ण name[64];
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
argptest(वर्ण *s)
{
  पूर्णाङ्क fd;
  fd = खोलो("init", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  पठ(fd, sbrk(0) - 1, -1);
  बंद(fd);
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
शून्य
stacktest(वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;

  pid = विभाजन();
  यदि (pid == 0) {
    वर्ण *sp = (वर्ण *)r_sp();
    sp -= USERSTACK * PGSIZE;
    // the *sp should cause a trap.
    म_लिखो("%s: stacktest: read below stack %d\n", s, *sp);
    निकास(1);
  } अन्यथा यदि (pid < 0) {
    म_लिखो("%s: fork failed\n", s);
    निकास(1);
  }
  रुको(&xstatus);
  यदि (xstatus == -1) // kernel killed child?
    निकास(0);
  अन्यथा
    निकास(xstatus);
}

// check that writes to a few forbidden addresses
// cause a fault, e.g. process's text and TRAMPOLINE.
शून्य
nowrite(वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;
  uint64 addrs[] = {0,
                    0x80000000LL,
                    0x3fffffe000,
                    0x3ffffff000,
                    0x4000000000,
                    0xffffffffffffffff};

  प्रति (पूर्णाङ्क ai = 0; ai < परिमाण(addrs) / परिमाण(addrs[0]); ai++) {
    pid = विभाजन();
    यदि (pid == 0) {
      चञ्चल पूर्णाङ्क *addr = (पूर्णाङ्क *)addrs[ai];
      *addr = 10;
      म_लिखो("%s: write to %p did not fail!\n", s, addr);
      निकास(0);
    } अन्यथा यदि (pid < 0) {
      म_लिखो("%s: fork failed\n", s);
      निकास(1);
    }
    रुको(&xstatus);
    यदि (xstatus == 0) {
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
pgbug(वर्ण *s)
{
  वर्ण *argv[1];
  argv[0] = 0;
  exec(big, argv);
  pipe(big);

  निकास(0);
}

// regression test. does the kernel panic if a process sbrk()s its
// size to be less than a page, or zero, or reduces the break by an
// amount too small to cause a page to be freed?
शून्य
sbrkbugs(वर्ण *s)
{
  पूर्णाङ्क pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  यदि (pid == 0) {
    पूर्णाङ्क sz = (uint64)sbrk(0);
    // free all user memory; there used to be a bug that
    // would not adjust p->sz correctly in this case,
    // causing exit() to panic.
    sbrk(-sz);
    // user page fault here.
    निकास(0);
  }
  रुको(0);

  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  यदि (pid == 0) {
    पूर्णाङ्क sz = (uint64)sbrk(0);
    // set the break to somewhere in the very first
    // page; there used to be a bug that would incorrectly
    // free the first page.
    sbrk(-(sz - 3500));
    निकास(0);
  }
  रुको(0);

  pid = विभाजन();
  यदि (pid < 0) {
    म_लिखो("fork failed\n");
    निकास(1);
  }
  यदि (pid == 0) {
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
sbrklast(वर्ण *s)
{
  uint64 शीर्ष = (uint64)sbrk(0);
  यदि ((शीर्ष % PGSIZE) != 0)
    sbrk(PGSIZE - (शीर्ष % PGSIZE));
  sbrk(PGSIZE);
  sbrk(10);
  sbrk(-20);
  शीर्ष = (uint64)sbrk(0);
  वर्ण *p = (वर्ण *)(शीर्ष - 64);
  p[0] = 'x';
  p[1] = '\0';
  पूर्णाङ्क fd = खोलो(p, O_RDWR | O_CREATE);
  लिख(fd, p, 1);
  बंद(fd);
  fd = खोलो(p, O_RDWR);
  p[0] = '\0';
  पठ(fd, p, 1);
  यदि (p[0] != 'x')
    निकास(1);
}

// does sbrk handle signed int32 wrap-around with
// negative arguments?
शून्य
sbrk8000(वर्ण *s)
{
  sbrk(0x80000004);
  चञ्चल वर्ण *शीर्ष = sbrk(0);
  *(शीर्ष - 1) = *(शीर्ष - 1) + 1;
}

// regression test. test whether exec() leaks memory if one of the
// arguments is invalid. the test passes if the kernel doesn't panic.
शून्य
badarg(वर्ण *s)
{
  प्रति (पूर्णाङ्क i = 0; i < 50000; i++) {
    वर्ण *argv[2];
    argv[0] = (वर्ण *)0xffffffff;
    argv[1] = 0;
    exec("echo", argv);
  }

  निकास(0);
}

#define REGION_SZ (1024 * 1024 * 1024)

// Touch a page every 64 pages, which with lazy allocation
// causes one page to be allocated.
शून्य
lazy_alloc(वर्ण *s)
{
  वर्ण *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  यदि (prev_end == (वर्ण *)SBRK_ERROR) {
    म_लिखो("sbrklazy() failed\n");
    निकास(1);
  }
  new_end = prev_end + REGION_SZ;

  प्रति (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE)
    *(वर्ण **)i = i;

  प्रति (i = prev_end + PGSIZE; i < new_end; i += 64 * PGSIZE) {
    यदि (*(वर्ण **)i != i) {
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
lazy_unmap(वर्ण *s)
{
  पूर्णाङ्क pid;
  वर्ण *i, *prev_end, *new_end;

  prev_end = sbrklazy(REGION_SZ);
  यदि (prev_end == (वर्ण *)SBRK_ERROR) {
    म_लिखो("sbrklazy() failed\n");
    निकास(1);
  }
  new_end = prev_end + REGION_SZ;

  प्रति (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE)
    *(वर्ण **)i = i;

  प्रति (i = prev_end + PGSIZE; i < new_end; i += PGSIZE * PGSIZE) {
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("error forking\n");
      निकास(1);
    } अन्यथा यदि (pid == 0) {
      sbrklazy(-1L * REGION_SZ);
      *(वर्ण **)i = i;
      निकास(0);
    } अन्यथा {
      पूर्णाङ्क status;
      रुको(&status);
      यदि (status == 0) {
        म_लिखो("memory not unmapped\n");
        निकास(1);
      }
    }
  }

  निकास(0);
}

शून्य
lazy_copy(वर्ण *s)
{
  // copyinstr on lazy page
  {
    वर्ण *p = sbrk(0);
    sbrklazy(4 * PGSIZE);
    खोलो(p + 8192, 0);
  }

  {
    शून्य *xx = sbrk(0);
    शून्य *ret = sbrk(-(((uint64)xx) + 1));
    यदि (ret != xx) {
      म_लिखो("sbrk(sbrk(0)+1) returned %p, not old sz\n", ret);
      निकास(1);
    }
  }

  // read() and write() to these addresses should fail.
  अचिन्हित दीर्घ bad[] = {
    0x3fffffc000, 0x3fffffd000, 0x3fffffe000,
    0x3ffffff000, 0x4000000000, 0x8000000000,
  };
  प्रति (पूर्णाङ्क i = 0; i < परिमाण(bad) / परिमाण(bad[0]); i++) {
    पूर्णाङ्क fd = खोलो("README", 0);
    यदि (fd < 0) {
      म_लिखो("cannot open README\n");
      निकास(1);
    }
    यदि (पठ(fd, (वर्ण *)bad[i], 512) >= 0) {
      म_लिखो("read succeeded\n");
      निकास(1);
    }
    बंद(fd);
    fd = खोलो("junk", O_CREATE | O_RDWR | O_TRUNC);
    यदि (fd < 0) {
      म_लिखो("cannot open junk\n");
      निकास(1);
    }
    यदि (लिख(fd, (वर्ण *)bad[i], 512) >= 0) {
      म_लिखो("write succeeded\n");
      निकास(1);
    }
    बंद(fd);
  }

  निकास(0);
}

शून्य
lazy_copyinstr(वर्ण *s)
{
  वर्ण *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  यदि ((uint64)p % PGSIZE != 0) {
    म_लिखो("%s: sbrk did not align\n", s);
    निकास(1);
  }

  sbrklazy(2 * PGSIZE);
  p[4095] = '/';
  पूर्णाङ्क fd = खोलो(&p[4095], O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("could not open /");
    निकास(1);
  }

  संरचना stat st;
  पूर्णाङ्क r = ख_स्थिति(fd, &st);
  यदि (r < 0) {
    म_लिखो("could not stat /");
    निकास(1);
  }

  यदि (st.type != T_DIR) {
    म_लिखो("/ is not T_DIR");
    निकास(1);
  }

  बंद(fd);
}

शून्य
lazy_sbrk(वर्ण *s)
{
  // sbrk() takes just int, so take 2^30-sized steps towards MAXVA
  वर्ण *p = sbrk(0);
  यावत् ((uint64)p < MAXVA - (1 << 30)) {
    p = sbrklazy(1 << 30);
    यदि (p < 0) {
      म_लिखो("sbrklazy(%d) returned %p\n", 1 << 30, p);
      निकास(1);
    }

    p = sbrklazy(0);
  }

  पूर्णाङ्क n = TRAPFRAME - PGSIZE - (uint64)p;

  वर्ण *p1 = sbrklazy(n);
  यदि (p1 < 0 || p1 != p) {
    म_लिखो("sbrklazy(%d) returned %p, not expected %p\n", n, p1, p);
    निकास(1);
  }

  p = sbrk(PGSIZE);
  यदि (p < 0 || (uint64)p != TRAPFRAME - PGSIZE) {
    म_लिखो("sbrk(%d) returned %p, not expected TRAPFRAME-PGSIZE\n", PGSIZE, p);
    निकास(1);
  }

  p[0] = 1;
  यदि (p[1] != 0) {
    म_लिखो("sbrk() returned non-zero-filled memory\n");
    निकास(1);
  }

  p = sbrk(1);
  यदि ((uint64)p != -1) {
    म_लिखो("sbrk(1) returned %p, expected error\n", p);
    निकास(1);
  }

  p = sbrklazy(1);
  यदि ((uint64)p != -1) {
    म_लिखो("sbrklazy(1) returned %p, expected error\n", p);
    निकास(1);
  }

  निकास(0);
}

शून्य
partial_write(वर्ण *s)
{
  // Create testfile containing "A".
  // write() 2 bytes that span page boundary: first is "X", second is unmapped.
  // Potential problem: write errors, forgets to log the updated first byte.
  // read() from file returns "X".
  // Flush buffer cache with some large writes.
  // read() from file should still return "X" (but might return "A" due to bug).

  unlink("testfile");
  पूर्णाङ्क fd = खोलो("testfile", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: cannot create testfile\n", s);
    निकास(1);
  }

  पूर्णाङ्क cc = लिख(fd, "A", 1);
  यदि (cc != 1) {
    म_लिखो("%s: could not write A\n", s);
    निकास(1);
  }

  बंद(fd);
  fd = खोलो("testfile", O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  वर्ण *p = sbrk(0);
  sbrk(PGSIZE - ((uint64)p % PGSIZE));

  p = sbrk(0);
  यदि ((uint64)p % PGSIZE != 0) {
    म_लिखो("%s: sbrk did not align\n", s);
    निकास(1);
  }

  p[-1] = 'X';

  cc = लिख(fd, p - 1, 2);
  यदि (cc != -1) {
    म_लिखो("%s: write succeeded, should have failed\n", s);
    निकास(1);
  }

  बंद(fd);

  fd = खोलो("testfile", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  वर्ण b;
  cc = पठ(fd, &b, 1);
  यदि (cc != 1) {
    म_लिखो("%s: cannot read testfile\n", s);
    निकास(1);
  }

  बंद(fd);

  यदि (b != 'X') {
    म_लिखो("%s: read returned %c, expected X\n", s, b);
    निकास(1);
  }

  fd = खोलो("bigfile", O_CREATE | O_RDWR);
  प्रति (पूर्णाङ्क i = 0; i < 64; i++) {
    वर्ण buf[1024];
    स_रखो(buf, 0, परिमाण(buf));
    cc = लिख(fd, buf, परिमाण(buf));
    यदि (cc != परिमाण(buf)) {
      म_लिखो("%s: could not write to bigfile\n", s);
      निकास(-1);
    }
  }
  बंद(fd);

  unlink("bigfile");

  fd = खोलो("testfile", O_RDONLY);
  यदि (fd < 0) {
    म_लिखो("%s: cannot re-open testfile\n", s);
    निकास(1);
  }

  cc = पठ(fd, &b, 1);
  यदि (cc != 1) {
    म_लिखो("%s: cannot read testfile\n", s);
    निकास(1);
  }

  बंद(fd);

  यदि (b != 'X') {
    म_लिखो("%s: read returned %c, expected X\n", s, b);
    निकास(1);
  }

  unlink("testfile");
}

शून्य
unlinkcwd(वर्ण *s)
{
  यदि (सूची_गढ़ो("/a") < 0) {
    म_लिखो("%s: mkdir /a failed\n", s);
    निकास(1);
  }
  यदि (सूची_गढ़ो("/a/b") < 0) {
    म_लिखो("%s: mkdir /a/b failed\n", s);
    निकास(1);
  }
  यदि (स_बदलो("/a/b") < 0) {
    म_लिखो("%s: chdir failed\n", s);
    निकास(1);
  }
  यदि (unlink("/a/b") < 0) {
    म_लिखो("%s: unlink /a/b failed\n", s);
    निकास(1);
  }
  यदि (unlink("/a") < 0) {
    म_लिखो("%s: unlink /a failed\n", s);
    निकास(1);
  }
  यदि (खोलो("../", O_RDONLY) > 0) {
    म_लिखो("%s: open ../ non-existing directory\n", s);
  }
  यदि (खोलो("../c", O_CREATE) > 0) {
    म_लिखो("%s: create ../c non-existing file\n", s);
  }
}

संरचना test {
  शून्य (*f)(वर्ण *);
  वर्ण *s;
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
bigdir(वर्ण *s)
{
  गणना { N = 500 };
  पूर्णाङ्क i, fd;
  वर्ण name[10];

  unlink("bd");

  fd = खोलो("bd", O_CREATE);
  यदि (fd < 0) {
    म_लिखो("%s: bigdir create failed\n", s);
    निकास(1);
  }
  बंद(fd);

  प्रति (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    यदि (link("bd", name) != 0) {
      म_लिखो("%s: bigdir i=%d link(bd, %s) failed\n", s, i, name);
      निकास(1);
    }
  }

  unlink("bd");
  प्रति (i = 0; i < N; i++) {
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    यदि (unlink(name) != 0) {
      म_लिखो("%s: bigdir unlink failed", s);
      निकास(1);
    }
  }
}

// concurrent writes to try to provoke deadlock in the virtio disk
// driver.
शून्य
manywrites(वर्ण *s)
{
  पूर्णाङ्क nchildren = 4;
  पूर्णाङ्क howmany = 30; // increase to look for deadlock

  प्रति (पूर्णाङ्क ci = 0; ci < nchildren; ci++) {
    पूर्णाङ्क pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    }

    यदि (pid == 0) {
      वर्ण name[3];
      name[0] = 'b';
      name[1] = 'a' + ci;
      name[2] = '\0';
      unlink(name);

      प्रति (पूर्णाङ्क iters = 0; iters < howmany; iters++) {
        प्रति (पूर्णाङ्क i = 0; i < ci + 1; i++) {
          पूर्णाङ्क fd = खोलो(name, O_CREATE | O_RDWR);
          यदि (fd < 0) {
            म_लिखो("%s: cannot create %s\n", s, name);
            निकास(1);
          }
          पूर्णाङ्क sz = परिमाण(buf);
          पूर्णाङ्क cc = लिख(fd, buf, sz);
          यदि (cc != sz) {
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

  प्रति (पूर्णाङ्क ci = 0; ci < nchildren; ci++) {
    पूर्णाङ्क st = 0;
    रुको(&st);
    यदि (st != 0)
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
badwrite(वर्ण *s)
{
  पूर्णाङ्क assumed_free = 600;

  unlink("junk");
  प्रति (पूर्णाङ्क i = 0; i < assumed_free; i++) {
    पूर्णाङ्क fd = खोलो("junk", O_CREATE | O_WRONLY);
    यदि (fd < 0) {
      म_लिखो("open junk failed\n");
      निकास(1);
    }
    लिख(fd, (वर्ण *)0xffffffffffL, 1);
    बंद(fd);
    unlink("junk");
  }

  पूर्णाङ्क fd = खोलो("junk", O_CREATE | O_WRONLY);
  यदि (fd < 0) {
    म_लिखो("open junk failed\n");
    निकास(1);
  }
  यदि (लिख(fd, "x", 1) != 1) {
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
execout(वर्ण *s)
{
  प्रति (पूर्णाङ्क avail = 0; avail < 15; avail++) {
    पूर्णाङ्क pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("fork failed\n");
      निकास(1);
    } अन्यथा यदि (pid == 0) {
      // allocate all of memory.
      यावत् (1) {
        वर्ण *a = sbrk(PGSIZE);
        यदि (a == SBRK_ERROR)
          भङ्ग;
        *(a + PGSIZE - 1) = 1;
      }

      // free a few pages, in order to let exec() make some
      // progress.
      प्रति (पूर्णाङ्क i = 0; i < avail; i++)
        sbrk(-PGSIZE);

      बंद(1);
      वर्ण *args[] = {"echo", "x", 0};
      exec("echo", args);
      निकास(0);
    } अन्यथा {
      रुको((पूर्णाङ्क *)0);
    }
  }

  निकास(0);
}

// can the kernel tolerate running out of disk space?
शून्य
diskfull(वर्ण *s)
{
  पूर्णाङ्क fi;
  पूर्णाङ्क done = 0;

  unlink("diskfulldir");

  प्रति (fi = 0; done == 0 && '0' + fi < 0177; fi++) {
    वर्ण name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + fi;
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    यदि (fd < 0) {
      // oops, ran out of inodes before running out of blocks.
      म_लिखो("%s: could not create file %s\n", s, name);
      done = 1;
      भङ्ग;
    }
    प्रति (पूर्णाङ्क i = 0; i < MAXFILE; i++) {
      वर्ण buf[BSIZE];
      यदि (लिख(fd, buf, BSIZE) != BSIZE) {
        done = 1;
        बंद(fd);
        भङ्ग;
      }
    }
    बंद(fd);
  }

  // now that there are no free blocks, test that dirlink()
  // merely fails (doesn't panic) if it can't extend
  // directory content. one of these file creations
  // is expected to fail.
  पूर्णाङ्क nzz = 128;
  प्रति (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    यदि (fd < 0)
      भङ्ग;
    बंद(fd);
  }

  // this mkdir() is expected to fail.
  यदि (सूची_गढ़ो("diskfulldir") == 0)
    म_लिखो("%s: mkdir(diskfulldir) unexpectedly succeeded!\n", s);

  unlink("diskfulldir");

  प्रति (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }

  प्रति (पूर्णाङ्क i = 0; '0' + i < 0177; i++) {
    वर्ण name[32];
    name[0] = 'b';
    name[1] = 'i';
    name[2] = 'g';
    name[3] = '0' + i;
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
outofinodes(वर्ण *s)
{
  पूर्णाङ्क nzz = 32 * 32;
  प्रति (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
    पूर्णाङ्क fd = खोलो(name, O_CREATE | O_RDWR | O_TRUNC);
    यदि (fd < 0) {
      // failure is eventually expected.
      भङ्ग;
    }
    बंद(fd);
  }

  प्रति (पूर्णाङ्क i = 0; i < nzz; i++) {
    वर्ण name[32];
    name[0] = 'z';
    name[1] = 'z';
    name[2] = '0' + (i / 32);
    name[3] = '0' + (i % 32);
    name[4] = '\0';
    unlink(name);
  }
}

शून्य
linkoverflow(वर्ण *s)
{
  गणना { TARGET = 32768 };
  गणना { DIRS = 64 };
  संरचना stat st;
  पूर्णाङ्क i;

  unlink("/lof");
  पूर्णाङ्क fd = खोलो("/lof", O_CREATE | O_RDWR);
  यदि (fd < 0) {
    म_लिखो("%s: cannot create /lof\n", s);
    निकास(1);
  }
  बंद(fd);

  प्रति (i = 0; i < TARGET; i++) {
    पूर्णाङ्क d = i % DIRS;
    पूर्णाङ्क f = i / DIRS;

    वर्ण pn[16];
    pn[0] = '/';
    pn[1] = 'd';
    pn[2] = '_';
    pn[3] = 'a' + (d / 16);
    pn[4] = 'a' + (d % 16);
    pn[5] = '\0';
    यदि (f == 0 && सूची_गढ़ो(pn) < 0) {
      म_लिखो("%s: mkdir(%s) failed\n", s, pn);
      निकास(1);
    }

    pn[5] = '/';
    pn[6] = 'l';
    pn[7] = 'a' + (f / 256);
    pn[8] = 'a' + ((f / 16) % 16);
    pn[9] = 'a' + (f % 16);
    pn[10] = '\0';

    यदि (link("/lof", pn) < 0) {
      यदि (stat("/lof", &st) < 0) {
        म_लिखो("%s: stat(/lof) failed\n", s);
        निकास(1);
      }
      यदि (st.nlink >= 32767) {
        // overflow check succeeded.
        भङ्ग;
      }
      म_लिखो("%s: link failed after %d links (nlink=%d)\n", s, i, st.nlink);
      निकास(1);
    }

    यदि (i % 100 == 0) {
      म_लिखो("%s: i=%d, pn=%s\n", s, i, pn);
    }
  }

  यदि (stat("/lof", &st) < 0) {
    म_लिखो("%s: stat(/lof) failed\n", s);
    निकास(1);
  }

  unlink("/lof");

  यदि (st.nlink < 0) {
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
पूर्णाङ्क
run(शून्य f(वर्ण *), वर्ण *s)
{
  पूर्णाङ्क pid;
  पूर्णाङ्क xstatus;

  म_लिखो("test %s: ", s);
  यदि ((pid = विभाजन()) < 0) {
    म_लिखो("runtest: fork error\n");
    निकास(1);
  }
  यदि (pid == 0) {
    f(s);
    निकास(0);
  } अन्यथा {
    रुको(&xstatus);
    यदि (xstatus != 0)
      म_लिखो("FAILED\n");
    अन्यथा
      म_लिखो("OK\n");
    प्रत्यागम xstatus == 0;
  }
}

पूर्णाङ्क
runtests(संरचना test *tests, वर्ण *justone, पूर्णाङ्क continuous)
{
  पूर्णाङ्क ntests = 0;
  प्रति (संरचना test *t = tests; t->s != 0; t++) {
    यदि ((justone == 0) || म_भेद(t->s, justone) == 0) {
      ntests++;
      यदि (!run(t->f, t->s)) {
        यदि (continuous != 2) {
          म_लिखो("SOME TESTS FAILED\n");
          प्रत्यागम -1;
        }
      }
    }
  }
  प्रत्यागम ntests;
}

// use sbrk() to count how many free physical memory pages there are.
पूर्णाङ्क
countfree()
{
  पूर्णाङ्क n = 0;
  uint64 sz0 = (uint64)sbrk(0);
  यावत् (1) {
    वर्ण *a = sbrk(PGSIZE);
    यदि (a == SBRK_ERROR) {
      भङ्ग;
    }
    n += 1;
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  प्रत्यागम n;
}

पूर्णाङ्क
drivetests(पूर्णाङ्क quick, पूर्णाङ्क continuous, वर्ण *justone)
{
  कुरु {
    म_लिखो("usertests starting\n");
    पूर्णाङ्क free0 = countfree();
    पूर्णाङ्क free1 = 0;
    पूर्णाङ्क ntests = 0;
    पूर्णाङ्क n;
    n = runtests(quicktests, justone, continuous);
    यदि (n < 0) {
      यदि (continuous != 2) {
        प्रत्यागम 1;
      }
    } अन्यथा {
      ntests += n;
    }
    यदि (!quick) {
      यदि (justone == 0)
        म_लिखो("usertests slow tests starting\n");
      n = runtests(slowtests, justone, continuous);
      यदि (n < 0) {
        यदि (continuous != 2) {
          प्रत्यागम 1;
        }
      } अन्यथा {
        ntests += n;
      }
    }
    यदि ((free1 = countfree()) < free0) {
      म_लिखो("FAILED -- lost some free pages %d (out of %d)\n", free1, free0);
      यदि (continuous != 2) {
        प्रत्यागम 1;
      }
    }
    यदि (justone != 0 && ntests == 0) {
      म_लिखो("NO TESTS EXECUTED\n");
      प्रत्यागम 1;
    }
  } यावत् (continuous);
  प्रत्यागम 0;
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क continuous = 0;
  पूर्णाङ्क quick = 0;
  वर्ण *justone = 0;

  यदि (argc == 2 && म_भेद(argv[1], "-q") == 0) {
    quick = 1;
  } अन्यथा यदि (argc == 2 && म_भेद(argv[1], "-c") == 0) {
    continuous = 1;
  } अन्यथा यदि (argc == 2 && म_भेद(argv[1], "-C") == 0) {
    continuous = 2;
  } अन्यथा यदि (argc == 2 && argv[1][0] != '-') {
    justone = argv[1];
  } अन्यथा यदि (argc > 1) {
    म_लिखो("Usage: usertests [-c] [-C] [-q] [testname]\n");
    निकास(1);
  }
  यदि (drivetests(quick, continuous, justone)) {
    निकास(1);
  }
  म_लिखो("ALL TESTS PASSED\n");
  निकास(0);
}
