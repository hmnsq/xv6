//
// run random system calls in parallel forever.
//

#అన్తర్భావ "kernel/param.h"
#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "kernel/syscall.h"
#అన్తర్భావ "kernel/memlayout.h"
#అన్తర్భావ "kernel/riscv.h"

// from FreeBSD.
పూర్ణాంకం
do_rand(గుర్తులేని దీర్ఘ *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  దీర్ఘ hi, lo, x;

  /* Transform to [1, 0x7ffffffe] range. */
  x = (*ctx % 0x7ffffffe) + 1;
  hi = x / 127773;
  lo = x % 127773;
  x = 16807 * lo - 2836 * hi;
  ఒకవేళ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  తిరిగి (x);
}

గుర్తులేని దీర్ఘ rand_తదుపరి = 1;

పూర్ణాంకం
rand(శూన్య)
{
  తిరిగి (do_rand(&rand_తదుపరి));
}

శూన్య
go(పూర్ణాంకం which_బిడ్డ)
{
  పూర్ణాంకం fd = -1;
  స్థిర అక్షరం buf[999];
  అక్షరం *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ఒకవేళ (chdir("grindir") != 0) {
    రాయి("grind: chdir grindir failed\n");
    నిష్క్రమించు(1);
  }
  chdir("/");

  అయితే (1) {
    iters++;
    ఒకవేళ ((iters % 500) == 0)
      లిఖ(1, which_బిడ్డ ? "B" : "A", 1);
    పూర్ణాంకం what = rand() % 23;
    ఒకవేళ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } లేకపోతే ఒకవేళ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } లేకపోతే ఒకవేళ (what == 3) {
      unlink("grindir/../a");
    } లేకపోతే ఒకవేళ (what == 4) {
      ఒకవేళ (chdir("grindir") != 0) {
        రాయి("grind: chdir grindir failed\n");
        నిష్క్రమించు(1);
      }
      unlink("../b");
      chdir("/");
    } లేకపోతే ఒకవేళ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } లేకపోతే ఒకవేళ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } లేకపోతే ఒకవేళ (what == 7) {
      లిఖ(fd, buf, పరిమాణం(buf));
    } లేకపోతే ఒకవేళ (what == 8) {
      పఠ(fd, buf, పరిమాణం(buf));
    } లేకపోతే ఒకవేళ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } లేకపోతే ఒకవేళ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } లేకపోతే ఒకవేళ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } లేకపోతే ఒకవేళ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } లేకపోతే ఒకవేళ (what == 13) {
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      wait(0);
    } లేకపోతే ఒకవేళ (what == 14) {
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        fork();
        fork();
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      wait(0);
    } లేకపోతే ఒకవేళ (what == 15) {
      sbrk(6011);
    } లేకపోతే ఒకవేళ (what == 16) {
      ఒకవేళ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } లేకపోతే ఒకవేళ (what == 17) {
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      ఒకవేళ (chdir("../grindir/..") != 0) {
        రాయి("grind: chdir failed\n");
        నిష్క్రమించు(1);
      }
      kill(pid);
      wait(0);
    } లేకపోతే ఒకవేళ (what == 18) {
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        kill(getpid());
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      wait(0);
    } లేకపోతే ఒకవేళ (what == 19) {
      పూర్ణాంకం fds[2];
      ఒకవేళ (pipe(fds) < 0) {
        రాయి("grind: pipe failed\n");
        నిష్క్రమించు(1);
      }
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        fork();
        fork();
        ఒకవేళ (లిఖ(fds[1], "x", 1) != 1)
          రాయి("grind: pipe write failed\n");
        అక్షరం c;
        ఒకవేళ (పఠ(fds[0], &c, 1) != 1)
          రాయి("grind: pipe read failed\n");
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } లేకపోతే ఒకవేళ (what == 20) {
      పూర్ణాంకం pid = fork();
      ఒకవేళ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        నిష్క్రమించు(0);
      } లేకపోతే ఒకవేళ (pid < 0) {
        రాయి("grind: fork failed\n");
        నిష్క్రమించు(1);
      }
      wait(0);
    } లేకపోతే ఒకవేళ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      పూర్ణాంకం fd1 = open("c", O_CREATE | O_RDWR);
      ఒకవేళ (fd1 < 0) {
        రాయి("grind: create c failed\n");
        నిష్క్రమించు(1);
      }
      ఒకవేళ (లిఖ(fd1, "x", 1) != 1) {
        రాయి("grind: write c failed\n");
        నిష్క్రమించు(1);
      }
      నిర్మాణం stat st;
      ఒకవేళ (fstat(fd1, &st) != 0) {
        రాయి("grind: fstat failed\n");
        నిష్క్రమించు(1);
      }
      ఒకవేళ (st.కొలత != 1) {
        రాయి("grind: fstat reports wrong size %d\n", (పూర్ణాంకం)st.కొలత);
        నిష్క్రమించు(1);
      }
      ఒకవేళ (st.ino > 200) {
        రాయి("grind: fstat reports crazy i-number %d\n", st.ino);
        నిష్క్రమించు(1);
      }
      close(fd1);
      unlink("c");
    } లేకపోతే ఒకవేళ (what == 22) {
      // echo hi | cat
      పూర్ణాంకం aa[2], bb[2];
      ఒకవేళ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        నిష్క్రమించు(1);
      }
      ఒకవేళ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        నిష్క్రమించు(1);
      }
      పూర్ణాంకం pid1 = fork();
      ఒకవేళ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ఒకవేళ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          నిష్క్రమించు(1);
        }
        close(aa[1]);
        అక్షరం *వాదనలు[3] = {"echo", "hi", 0};
        exec("grindir/../echo", వాదనలు);
        fprintf(2, "grind: echo: not found\n");
        నిష్క్రమించు(2);
      } లేకపోతే ఒకవేళ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        నిష్క్రమించు(3);
      }
      పూర్ణాంకం pid2 = fork();
      ఒకవేళ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ఒకవేళ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          నిష్క్రమించు(4);
        }
        close(aa[0]);
        close(1);
        ఒకవేళ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          నిష్క్రమించు(5);
        }
        close(bb[1]);
        అక్షరం *వాదనలు[2] = {"cat", 0};
        exec("/cat", వాదనలు);
        fprintf(2, "grind: cat: not found\n");
        నిష్క్రమించు(6);
      } లేకపోతే ఒకవేళ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        నిష్క్రమించు(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      అక్షరం buf[4] = {0, 0, 0, 0};
      పఠ(bb[0], buf + 0, 1);
      పఠ(bb[0], buf + 1, 1);
      పఠ(bb[0], buf + 2, 1);
      close(bb[0]);
      పూర్ణాంకం st1, st2;
      wait(&st1);
      wait(&st2);
      ఒకవేళ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        రాయి("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        నిష్క్రమించు(1);
      }
    }
  }
}

శూన్య
iter()
{
  unlink("a");
  unlink("b");

  పూర్ణాంకం pid1 = fork();
  ఒకవేళ (pid1 < 0) {
    రాయి("grind: fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid1 == 0) {
    rand_తదుపరి ^= 31;
    go(0);
    నిష్క్రమించు(0);
  }

  పూర్ణాంకం pid2 = fork();
  ఒకవేళ (pid2 < 0) {
    రాయి("grind: fork failed\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (pid2 == 0) {
    rand_తదుపరి ^= 7177;
    go(1);
    నిష్క్రమించు(0);
  }

  పూర్ణాంకం st1 = -1;
  wait(&st1);
  ఒకవేళ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  పూర్ణాంకం st2 = -1;
  wait(&st2);

  నిష్క్రమించు(0);
}

పూర్ణాంకం
ప్రధాన()
{
  అయితే (1) {
    పూర్ణాంకం pid = fork();
    ఒకవేళ (pid == 0) {
      iter();
      నిష్క్రమించు(0);
    }
    ఒకవేళ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_తదుపరి += 1;
  }
}
