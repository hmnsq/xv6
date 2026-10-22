//
// run random system calls in parallel forever.
//

#ضمن "kernel/param.h"
#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"
#ضمن "kernel/syscall.h"
#ضمن "kernel/memlayout.h"
#ضمن "kernel/riscv.h"

// from FreeBSD.
صحيح
do_rand(unsigned long *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  long hi, lo, x;

  /* Transform to [1, 0x7ffffffe] range. */
  x = (*ctx % 0x7ffffffe) + 1;
  hi = x / 127773;
  lo = x % 127773;
  x = 16807 * lo - 2836 * hi;
  ܐܢ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ܦܢܐ (x);
}

unsigned long rand_next = 1;

صحيح
rand(فراغ)
{
  ܦܢܐ (do_rand(&rand_next));
}

فراغ
go(صحيح which_child)
{
  صحيح fd = -1;
  ساكن حرف buf[999];
  حرف *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ܐܢ (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ܟܡܐ (1) {
    iters++;
    ܐܢ ((iters % 500) == 0)
      اكتب(1, which_child ? "B" : "A", 1);
    صحيح what = rand() % 23;
    ܐܢ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ܐܚܪܢܐ ܐܢ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ܐܚܪܢܐ ܐܢ (what == 3) {
      unlink("grindir/../a");
    } ܐܚܪܢܐ ܐܢ (what == 4) {
      ܐܢ (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } ܐܚܪܢܐ ܐܢ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ܐܚܪܢܐ ܐܢ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ܐܚܪܢܐ ܐܢ (what == 7) {
      اكتب(fd, buf, حجم(buf));
    } ܐܚܪܢܐ ܐܢ (what == 8) {
      اقرأ(fd, buf, حجم(buf));
    } ܐܚܪܢܐ ܐܢ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ܐܚܪܢܐ ܐܢ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ܐܚܪܢܐ ܐܢ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ܐܚܪܢܐ ܐܢ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ܐܚܪܢܐ ܐܢ (what == 13) {
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 14) {
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        fork();
        fork();
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 15) {
      sbrk(6011);
    } ܐܚܪܢܐ ܐܢ (what == 16) {
      ܐܢ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ܐܚܪܢܐ ܐܢ (what == 17) {
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      ܐܢ (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 18) {
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        kill(getpid());
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 19) {
      صحيح fds[2];
      ܐܢ (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        fork();
        fork();
        ܐܢ (اكتب(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        حرف c;
        ܐܢ (اقرأ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 20) {
      صحيح pid = fork();
      ܐܢ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } ܐܚܪܢܐ ܐܢ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ܐܚܪܢܐ ܐܢ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      صحيح fd1 = open("c", O_CREATE | O_RDWR);
      ܐܢ (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      ܐܢ (اكتب(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      بنية stat st;
      ܐܢ (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      ܐܢ (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (صحيح)st.size);
        exit(1);
      }
      ܐܢ (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } ܐܚܪܢܐ ܐܢ (what == 22) {
      // echo hi | cat
      صحيح aa[2], bb[2];
      ܐܢ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ܐܢ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      صحيح pid1 = fork();
      ܐܢ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ܐܢ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        حرف *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } ܐܚܪܢܐ ܐܢ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      صحيح pid2 = fork();
      ܐܢ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ܐܢ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        ܐܢ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        حرف *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } ܐܚܪܢܐ ܐܢ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      حرف buf[4] = {0, 0, 0, 0};
      اقرأ(bb[0], buf + 0, 1);
      اقرأ(bb[0], buf + 1, 1);
      اقرأ(bb[0], buf + 2, 1);
      close(bb[0]);
      صحيح st1, st2;
      wait(&st1);
      wait(&st2);
      ܐܢ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

فراغ
iter()
{
  unlink("a");
  unlink("b");

  صحيح pid1 = fork();
  ܐܢ (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ܐܢ (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  صحيح pid2 = fork();
  ܐܢ (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ܐܢ (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  صحيح st1 = -1;
  wait(&st1);
  ܐܢ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  صحيح st2 = -1;
  wait(&st2);

  exit(0);
}

صحيح
main()
{
  ܟܡܐ (1) {
    صحيح pid = fork();
    ܐܢ (pid == 0) {
      iter();
      exit(0);
    }
    ܐܢ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
