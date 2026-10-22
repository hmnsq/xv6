//
// run random system calls in parallel forever.
//

#समावेश "kernel/param.h"
#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"
#समावेश "kernel/fs.h"
#समावेश "kernel/fcntl.h"
#समावेश "kernel/syscall.h"
#समावेश "kernel/memlayout.h"
#समावेश "kernel/riscv.h"

// from FreeBSD.
पूर्णांक
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
  अगर (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  वापस (x);
}

अचिन्हित दीर्घ rand_अग्र = 1;

पूर्णांक
अक्रम(व्योम)
{
  वापस (do_rand(&rand_अग्र));
}

व्योम
go(पूर्णांक which_child)
{
  पूर्णांक fd = -1;
  अटल अक्षर buf[999];
  अक्षर *break0 = sbrk(0);
  uint64 iters = 0;

  सूची_गढ़ो("grindir");
  अगर (स_बदलो("grindir") != 0) {
    म_लिखो("grind: chdir grindir failed\n");
    निकास(1);
  }
  स_बदलो("/");

  जबतक (1) {
    iters++;
    अगर ((iters % 500) == 0)
      ग_लिखो(1, which_child ? "B" : "A", 1);
    पूर्णांक what = अक्रम() % 23;
    अगर (what == 1) {
      बंद(खोलो("grindir/../a", O_CREATE | O_RDWR));
    } अन्यथा अगर (what == 2) {
      बंद(खोलो("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } अन्यथा अगर (what == 3) {
      unlink("grindir/../a");
    } अन्यथा अगर (what == 4) {
      अगर (स_बदलो("grindir") != 0) {
        म_लिखो("grind: chdir grindir failed\n");
        निकास(1);
      }
      unlink("../b");
      स_बदलो("/");
    } अन्यथा अगर (what == 5) {
      बंद(fd);
      fd = खोलो("/grindir/../a", O_CREATE | O_RDWR);
    } अन्यथा अगर (what == 6) {
      बंद(fd);
      fd = खोलो("/./grindir/./../b", O_CREATE | O_RDWR);
    } अन्यथा अगर (what == 7) {
      ग_लिखो(fd, buf, माप(buf));
    } अन्यथा अगर (what == 8) {
      पढ़ो(fd, buf, माप(buf));
    } अन्यथा अगर (what == 9) {
      सूची_गढ़ो("grindir/../a");
      बंद(खोलो("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } अन्यथा अगर (what == 10) {
      सूची_गढ़ो("/../b");
      बंद(खोलो("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } अन्यथा अगर (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } अन्यथा अगर (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } अन्यथा अगर (what == 13) {
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा अगर (what == 14) {
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        विभाजन();
        विभाजन();
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा अगर (what == 15) {
      sbrk(6011);
    } अन्यथा अगर (what == 16) {
      अगर (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } अन्यथा अगर (what == 17) {
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        बंद(खोलो("a", O_CREATE | O_RDWR));
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      अगर (स_बदलो("../grindir/..") != 0) {
        म_लिखो("grind: chdir failed\n");
        निकास(1);
      }
      समाप्त(pid);
      रुको(0);
    } अन्यथा अगर (what == 18) {
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        समाप्त(getpid());
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा अगर (what == 19) {
      पूर्णांक fds[2];
      अगर (pipe(fds) < 0) {
        म_लिखो("grind: pipe failed\n");
        निकास(1);
      }
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        विभाजन();
        विभाजन();
        अगर (ग_लिखो(fds[1], "x", 1) != 1)
          म_लिखो("grind: pipe write failed\n");
        अक्षर c;
        अगर (पढ़ो(fds[0], &c, 1) != 1)
          म_लिखो("grind: pipe read failed\n");
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      बंद(fds[0]);
      बंद(fds[1]);
      रुको(0);
    } अन्यथा अगर (what == 20) {
      पूर्णांक pid = विभाजन();
      अगर (pid == 0) {
        unlink("a");
        सूची_गढ़ो("a");
        स_बदलो("a");
        unlink("../a");
        fd = खोलो("x", O_CREATE | O_RDWR);
        unlink("x");
        निकास(0);
      } अन्यथा अगर (pid < 0) {
        म_लिखो("grind: fork failed\n");
        निकास(1);
      }
      रुको(0);
    } अन्यथा अगर (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      पूर्णांक fd1 = खोलो("c", O_CREATE | O_RDWR);
      अगर (fd1 < 0) {
        म_लिखो("grind: create c failed\n");
        निकास(1);
      }
      अगर (ग_लिखो(fd1, "x", 1) != 1) {
        म_लिखो("grind: write c failed\n");
        निकास(1);
      }
      काष्ठा stat st;
      अगर (ख_स्थिति(fd1, &st) != 0) {
        म_लिखो("grind: fstat failed\n");
        निकास(1);
      }
      अगर (st.आकार != 1) {
        म_लिखो("grind: fstat reports wrong size %d\n", (पूर्णांक)st.आकार);
        निकास(1);
      }
      अगर (st.ino > 200) {
        म_लिखो("grind: fstat reports crazy i-number %d\n", st.ino);
        निकास(1);
      }
      बंद(fd1);
      unlink("c");
    } अन्यथा अगर (what == 22) {
      // echo hi | cat
      पूर्णांक aa[2], bb[2];
      अगर (pipe(aa) < 0) {
        ख_लिखो(2, "grind: pipe failed\n");
        निकास(1);
      }
      अगर (pipe(bb) < 0) {
        ख_लिखो(2, "grind: pipe failed\n");
        निकास(1);
      }
      पूर्णांक pid1 = विभाजन();
      अगर (pid1 == 0) {
        बंद(bb[0]);
        बंद(bb[1]);
        बंद(aa[0]);
        बंद(1);
        अगर (dup(aa[1]) != 1) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(1);
        }
        बंद(aa[1]);
        अक्षर *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        ख_लिखो(2, "grind: echo: not found\n");
        निकास(2);
      } अन्यथा अगर (pid1 < 0) {
        ख_लिखो(2, "grind: fork failed\n");
        निकास(3);
      }
      पूर्णांक pid2 = विभाजन();
      अगर (pid2 == 0) {
        बंद(aa[1]);
        बंद(bb[0]);
        बंद(0);
        अगर (dup(aa[0]) != 0) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(4);
        }
        बंद(aa[0]);
        बंद(1);
        अगर (dup(bb[1]) != 1) {
          ख_लिखो(2, "grind: dup failed\n");
          निकास(5);
        }
        बंद(bb[1]);
        अक्षर *args[2] = {"cat", 0};
        exec("/cat", args);
        ख_लिखो(2, "grind: cat: not found\n");
        निकास(6);
      } अन्यथा अगर (pid2 < 0) {
        ख_लिखो(2, "grind: fork failed\n");
        निकास(7);
      }
      बंद(aa[0]);
      बंद(aa[1]);
      बंद(bb[1]);
      अक्षर buf[4] = {0, 0, 0, 0};
      पढ़ो(bb[0], buf + 0, 1);
      पढ़ो(bb[0], buf + 1, 1);
      पढ़ो(bb[0], buf + 2, 1);
      बंद(bb[0]);
      पूर्णांक st1, st2;
      रुको(&st1);
      रुको(&st2);
      अगर (st1 != 0 || st2 != 0 || म_भेद(buf, "hi\n") != 0) {
        म_लिखो("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        निकास(1);
      }
    }
  }
}

व्योम
iter()
{
  unlink("a");
  unlink("b");

  पूर्णांक pid1 = विभाजन();
  अगर (pid1 < 0) {
    म_लिखो("grind: fork failed\n");
    निकास(1);
  }
  अगर (pid1 == 0) {
    rand_अग्र ^= 31;
    go(0);
    निकास(0);
  }

  पूर्णांक pid2 = विभाजन();
  अगर (pid2 < 0) {
    म_लिखो("grind: fork failed\n");
    निकास(1);
  }
  अगर (pid2 == 0) {
    rand_अग्र ^= 7177;
    go(1);
    निकास(0);
  }

  पूर्णांक st1 = -1;
  रुको(&st1);
  अगर (st1 != 0) {
    समाप्त(pid1);
    समाप्त(pid2);
  }
  पूर्णांक st2 = -1;
  रुको(&st2);

  निकास(0);
}

पूर्णांक
मुख्य()
{
  जबतक (1) {
    पूर्णांक pid = विभाजन();
    अगर (pid == 0) {
      iter();
      निकास(0);
    }
    अगर (pid > 0) {
      रुको(0);
    }
    छोड़ो(20);
    rand_अग्र += 1;
  }
}
