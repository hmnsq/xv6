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
do_rand(अचिह्नित दीर्घ *ctx)
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

अचिह्नित दीर्घ rand_अग्र = 1;

पूर्णाङ्क
यादृच्छिकम्(शून्य)
{
  प्रत्यागम (do_rand(&rand_अग्र));
}

शून्य
go(पूर्णाङ्क which_child)
{
  पूर्णाङ्क fd = -1;
  स्थिर वर्ण buf[999];
  वर्ण *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  यदि (chdir("grindir") != 0) {
    प्रारूप_लिख("grind: chdir grindir failed\n");
    निर्गच्छ(1);
  }
  chdir("/");

  यावत् (1) {
    iters++;
    यदि ((iters % 500) == 0)
      लिख(1, which_child ? "B" : "A", 1);
    पूर्णाङ्क what = यादृच्छिकम्() % 23;
    यदि (what == 1) {
      close(उद्घाटय("grindir/../a", O_CREATE | O_RDWR));
    } अन्यथा यदि (what == 2) {
      close(उद्घाटय("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } अन्यथा यदि (what == 3) {
      unlink("grindir/../a");
    } अन्यथा यदि (what == 4) {
      यदि (chdir("grindir") != 0) {
        प्रारूप_लिख("grind: chdir grindir failed\n");
        निर्गच्छ(1);
      }
      unlink("../b");
      chdir("/");
    } अन्यथा यदि (what == 5) {
      close(fd);
      fd = उद्घाटय("/grindir/../a", O_CREATE | O_RDWR);
    } अन्यथा यदि (what == 6) {
      close(fd);
      fd = उद्घाटय("/./grindir/./../b", O_CREATE | O_RDWR);
    } अन्यथा यदि (what == 7) {
      लिख(fd, buf, परिमाण(buf));
    } अन्यथा यदि (what == 8) {
      पठ(fd, buf, परिमाण(buf));
    } अन्यथा यदि (what == 9) {
      mkdir("grindir/../a");
      close(उद्घाटय("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } अन्यथा यदि (what == 10) {
      mkdir("/../b");
      close(उद्घाटय("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } अन्यथा यदि (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } अन्यथा यदि (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } अन्यथा यदि (what == 13) {
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 14) {
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        fork();
        fork();
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 15) {
      sbrk(6011);
    } अन्यथा यदि (what == 16) {
      यदि (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } अन्यथा यदि (what == 17) {
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        close(उद्घाटय("a", O_CREATE | O_RDWR));
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      यदि (chdir("../grindir/..") != 0) {
        प्रारूप_लिख("grind: chdir failed\n");
        निर्गच्छ(1);
      }
      kill(pid);
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 18) {
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        kill(getpid());
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 19) {
      पूर्णाङ्क fds[2];
      यदि (pipe(fds) < 0) {
        प्रारूप_लिख("grind: pipe failed\n");
        निर्गच्छ(1);
      }
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        fork();
        fork();
        यदि (लिख(fds[1], "x", 1) != 1)
          प्रारूप_लिख("grind: pipe write failed\n");
        वर्ण c;
        यदि (पठ(fds[0], &c, 1) != 1)
          प्रारूप_लिख("grind: pipe read failed\n");
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      close(fds[0]);
      close(fds[1]);
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 20) {
      पूर्णाङ्क pid = fork();
      यदि (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = उद्घाटय("x", O_CREATE | O_RDWR);
        unlink("x");
        निर्गच्छ(0);
      } अन्यथा यदि (pid < 0) {
        प्रारूप_लिख("grind: fork failed\n");
        निर्गच्छ(1);
      }
      प्रतीक्षस्व(0);
    } अन्यथा यदि (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      पूर्णाङ्क fd1 = उद्घाटय("c", O_CREATE | O_RDWR);
      यदि (fd1 < 0) {
        प्रारूप_लिख("grind: create c failed\n");
        निर्गच्छ(1);
      }
      यदि (लिख(fd1, "x", 1) != 1) {
        प्रारूप_लिख("grind: write c failed\n");
        निर्गच्छ(1);
      }
      संरचना stat st;
      यदि (fstat(fd1, &st) != 0) {
        प्रारूप_लिख("grind: fstat failed\n");
        निर्गच्छ(1);
      }
      यदि (st.आकार != 1) {
        प्रारूप_लिख("grind: fstat reports wrong size %d\n", (पूर्णाङ्क)st.आकार);
        निर्गच्छ(1);
      }
      यदि (st.ino > 200) {
        प्रारूप_लिख("grind: fstat reports crazy i-number %d\n", st.ino);
        निर्गच्छ(1);
      }
      close(fd1);
      unlink("c");
    } अन्यथा यदि (what == 22) {
      // echo hi | cat
      पूर्णाङ्क aa[2], bb[2];
      यदि (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        निर्गच्छ(1);
      }
      यदि (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        निर्गच्छ(1);
      }
      पूर्णाङ्क pid1 = fork();
      यदि (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        यदि (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          निर्गच्छ(1);
        }
        close(aa[1]);
        वर्ण *तर्काः[3] = {"echo", "hi", 0};
        exec("grindir/../echo", तर्काः);
        fprintf(2, "grind: echo: not found\n");
        निर्गच्छ(2);
      } अन्यथा यदि (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        निर्गच्छ(3);
      }
      पूर्णाङ्क pid2 = fork();
      यदि (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        यदि (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          निर्गच्छ(4);
        }
        close(aa[0]);
        close(1);
        यदि (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          निर्गच्छ(5);
        }
        close(bb[1]);
        वर्ण *तर्काः[2] = {"cat", 0};
        exec("/cat", तर्काः);
        fprintf(2, "grind: cat: not found\n");
        निर्गच्छ(6);
      } अन्यथा यदि (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        निर्गच्छ(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      वर्ण buf[4] = {0, 0, 0, 0};
      पठ(bb[0], buf + 0, 1);
      पठ(bb[0], buf + 1, 1);
      पठ(bb[0], buf + 2, 1);
      close(bb[0]);
      पूर्णाङ्क st1, st2;
      प्रतीक्षस्व(&st1);
      प्रतीक्षस्व(&st2);
      यदि (st1 != 0 || st2 != 0 || वाक्यं_तुलय(buf, "hi\n") != 0) {
        प्रारूप_लिख("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        निर्गच्छ(1);
      }
    }
  }
}

शून्य
iter()
{
  unlink("a");
  unlink("b");

  पूर्णाङ्क pid1 = fork();
  यदि (pid1 < 0) {
    प्रारूप_लिख("grind: fork failed\n");
    निर्गच्छ(1);
  }
  यदि (pid1 == 0) {
    rand_अग्र ^= 31;
    go(0);
    निर्गच्छ(0);
  }

  पूर्णाङ्क pid2 = fork();
  यदि (pid2 < 0) {
    प्रारूप_लिख("grind: fork failed\n");
    निर्गच्छ(1);
  }
  यदि (pid2 == 0) {
    rand_अग्र ^= 7177;
    go(1);
    निर्गच्छ(0);
  }

  पूर्णाङ्क st1 = -1;
  प्रतीक्षस्व(&st1);
  यदि (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  पूर्णाङ्क st2 = -1;
  प्रतीक्षस्व(&st2);

  निर्गच्छ(0);
}

पूर्णाङ्क
मुख्यम्()
{
  यावत् (1) {
    पूर्णाङ्क pid = fork();
    यदि (pid == 0) {
      iter();
      निर्गच्छ(0);
    }
    यदि (pid > 0) {
      प्रतीक्षस्व(0);
    }
    pause(20);
    rand_अग्र += 1;
  }
}
