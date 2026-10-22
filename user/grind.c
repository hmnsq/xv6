//
// run random system calls in parallel forever.
//

#अन्तर्भाव "kernel/param.h"
#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "kernel/syscall.h"
#अन्तर्भाव "kernel/memlayout.h"
#अन्तर्भाव "kernel/riscv.h"

// from FreeBSD.
पूर्णाङ्क
do_rand(अचिन्हित दीर्घ *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  दीर्घ hi, lo, x;

  /* Transform to [1, 0x7ffffffe] range. */
  x = (*ctx % 0x7ffffffe) + 1;
  hi = x / 127773;
  lo = x % 127773;
  x = 16807 * lo - 2836 * hi;
  यदि (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  प्रत्यागम (x);
}

अचिन्हित दीर्घ rand_next = 1;

पूर्णाङ्क
अक्रम(शून्य)
{
  प्रत्यागम (do_rand(&rand_next));
}

शून्य
go(पूर्णाङ्क which_child)
{
  पूर्णाङ्क fd = -1;
  स्थिर वर्ण buf[999];
  वर्ण *break0 = sbrk(0);
  uint64 iters = 0;

  सूची_गढ़ो("grindir");
  यदि (स_बदलो("grindir") != 0) {
    म_लिखो("grind: chdir grindir failed\n");
    निकास(1);
  }
  स_बदलो("/");

  यावत् (1) {
    iters++;
    यदि ((iters % 500) == 0)
      लिख(1, which_child ? "B" : "A", 1);
    पूर्णाङ्क what = अक्रम() % 23;
    यदि (what == 1) {
      बंद(खोलो("grindir/../a", O_CREATE | O_RDWR));
    } अन्यथा यदि (what == 2) {
      बंद(खोलो("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } अन्यथा यदि (what == 3) {
      unlink("grindir/../a");
    } अन्यथा यदि (what == 4) {
      यदि (स_बदलो("grindir") != 0) {
        म_लिखो("grind: chdir grindir failed\n");
        निकास(1);
      }
      unlink("../b");
      स_बदलो("/");
    } अन्यथा यदि (what == 5) {
      बंद(fd);
      fd = खोलो("/grindir/../a", O_CREATE | O_RDWR);
    } अन्यथा यदि (what == 6) {
      बंद(fd);
      fd = खोलो("/./grindir/./../b", O_CREATE | O_RDWR);
    } अन्यथा यदि (what == 7) {
      लिख(fd, buf, परिमाण(buf));
    } अन्यथा यदि (what == 8) {
      पठ(fd, buf, परिमाण(buf));
    } अन्यथा यदि (what == 9) {
      सूची_गढ़ो("grindir/../a");
      बंद(खोलो("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } अन्यथा यदि (what == 10) {
      सूची_गढ़ो("/../b");
      बंद(खोलो("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } अन्यथा यदि (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } अन्यथा यदि (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } अन्यथा यदि (what == 13) {
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा यदि (what == 14) {
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        विभाजन();
        विभाजन();
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा यदि (what == 15) {
      sbrk(6011);
    } अन्यथा यदि (what == 16) {
      यदि (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } अन्यथा यदि (what == 17) {
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        बंद(खोलो("a", O_CREATE | O_RDWR));
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      यदि (स_बदलो("../grindir/..") != 0) {
        म_लिखो("grind: chdir failed\n");
        निकास(1);
      }
      समाप्त(pid);
      रुको(0);
    } अन्यथा यदि (what == 18) {
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        समाप्त(getpid());
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा यदि (what == 19) {
      पूर्णाङ्क fds[2];
      यदि (pipe(fds) < 0) {
        म_लिखो("grind: pipe failed\n");
        निकास(1);
      }
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        विभाजन();
        विभाजन();
        यदि (लिख(fds[1], "x", 1) != 1)
          म_लिखो("grind: pipe write failed\n");
        वर्ण c;
        यदि (पठ(fds[0], &c, 1) != 1)
          म_लिखो("grind: pipe read failed\n");
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      बंद(fds[0]);
      बंद(fds[1]);
      रुको(0);
    } अन्यथा यदि (what == 20) {
      पूर्णाङ्क pid = विभाजन();
      यदि (pid == 0) {
        unlink("a");
        सूची_गढ़ो("a");
        स_बदलो("a");
        unlink("../a");
        fd = खोलो("x", O_CREATE | O_RDWR);
        unlink("x");
        निकास(0);
      } अन्यथा यदि (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा यदि (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      पूर्णाङ्क fd1 = खोलो("c", O_CREATE | O_RDWR);
      यदि (fd1 < 0) {
        म_लिखो("grind: create c failed\n");
        निकास(1);
      }
      यदि (लिख(fd1, "x", 1) != 1) {
        म_लिखो("grind: write c failed\n");
        निकास(1);
      }
      संरचना stat st;
      यदि (ख_स्थिति(fd1, &st) != 0) {
        म_लिखो("grind: fstat failed\n");
        निकास(1);
      }
      यदि (st.size != 1) {
        म_लिखो("grind: fstat reports wrong size %d\n", (पूर्णाङ्क)st.size);
        निकास(1);
      }
      यदि (st.ino > 200) {
        म_लिखो("grind: fstat reports crazy i-number %d\n", st.ino);
        निकास(1);
      }
      बंद(fd1);
      unlink("c");
    } अन्यथा यदि (what == 22) {
      // echo hi | cat
      पूर्णाङ्क aa[2], bb[2];
      यदि (pipe(aa) < 0) {
        ख_लिखो(2, "grind: pipe failed\n");
        निकास(1);
      }
      यदि (pipe(bb) < 0) {
        ख_लिखो(2, "grind: pipe failed\n");
        निकास(1);
      }
      पूर्णाङ्क pid1 = विभाजन();
      यदि (pid1 == 0) {
        बंद(bb[0]);
        बंद(bb[1]);
        बंद(aa[0]);
        बंद(1);
        यदि (dup(aa[1]) != 1) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(1);
        }
        बंद(aa[1]);
        वर्ण *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        ख_लिखो(2, "grind: echo: not found\n");
        निकास(2);
      } अन्यथा यदि (pid1 < 0) {
        ख_लिखो(2, "grind: fork failed\n");
        निकास(3);
      }
      पूर्णाङ्क pid2 = विभाजन();
      यदि (pid2 == 0) {
        बंद(aa[1]);
        बंद(bb[0]);
        बंद(0);
        यदि (dup(aa[0]) != 0) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(4);
        }
        बंद(aa[0]);
        बंद(1);
        यदि (dup(bb[1]) != 1) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(5);
        }
        बंद(bb[1]);
        वर्ण *args[2] = {"cat", 0};
        exec("/cat", args);
        ख_लिखो(2, "grind: cat: not found\n");
        निकास(6);
      } अन्यथा यदि (pid2 < 0) {
        ख_लिखो(2, "grind: fork failed\n");
        निकास(7);
      }
      बंद(aa[0]);
      बंद(aa[1]);
      बंद(bb[1]);
      वर्ण buf[4] = {0, 0, 0, 0};
      पठ(bb[0], buf + 0, 1);
      पठ(bb[0], buf + 1, 1);
      पठ(bb[0], buf + 2, 1);
      बंद(bb[0]);
      पूर्णाङ्क st1, st2;
      रुको(&st1);
      रुको(&st2);
      यदि (st1 != 0 || st2 != 0 || म_भेद(buf, "hi\n") != 0) {
        म_लिखो("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        निकास(1);
      }
    }
  }
}

शून्य
iter()
{
  unlink("a");
  unlink("b");

  पूर्णाङ्क pid1 = विभाजन();
  यदि (pid1 < 0) {
    म_लिखो("grind: fork failed\n");
    निकास(1);
  }
  यदि (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    निकास(0);
  }

  पूर्णाङ्क pid2 = विभाजन();
  यदि (pid2 < 0) {
    म_लिखो("grind: fork failed\n");
    निकास(1);
  }
  यदि (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    निकास(0);
  }

  पूर्णाङ्क st1 = -1;
  रुको(&st1);
  यदि (st1 != 0) {
    समाप्त(pid1);
    समाप्त(pid2);
  }
  पूर्णाङ्क st2 = -1;
  रुको(&st2);

  निकास(0);
}

पूर्णाङ्क
main()
{
  यावत् (1) {
    पूर्णाङ्क pid = विभाजन();
    यदि (pid == 0) {
      iter();
      निकास(0);
    }
    यदि (pid > 0) {
      रुको(0);
    }
    छोड़ो(20);
    rand_next += 1;
  }
}
