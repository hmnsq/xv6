//
// run random system calls in parallel forever.
//

#شامل "kernel/param.h"
#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"
#شامل "kernel/syscall.h"
#شامل "kernel/memlayout.h"
#شامل "kernel/riscv.h"

// from FreeBSD.
صحیح
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
  اگر (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  بازگشت (x);
}

unsigned long rand_next = 1;

صحیح
rand(تهی)
{
  بازگشت (do_rand(&rand_next));
}

تهی
go(صحیح which_child)
{
  صحیح fd = -1;
  ایستا نویسه buf[999];
  نویسه *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  اگر (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  وقتی (1) {
    iters++;
    اگر ((iters % 500) == 0)
      اكتب(1, which_child ? "B" : "A", 1);
    صحیح what = rand() % 23;
    اگر (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } وگرنه اگر (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } وگرنه اگر (what == 3) {
      unlink("grindir/../a");
    } وگرنه اگر (what == 4) {
      اگر (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } وگرنه اگر (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } وگرنه اگر (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } وگرنه اگر (what == 7) {
      اكتب(fd, buf, اندازه(buf));
    } وگرنه اگر (what == 8) {
      اقرأ(fd, buf, اندازه(buf));
    } وگرنه اگر (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } وگرنه اگر (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } وگرنه اگر (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } وگرنه اگر (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } وگرنه اگر (what == 13) {
      صحیح pid = fork();
      اگر (pid == 0) {
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } وگرنه اگر (what == 14) {
      صحیح pid = fork();
      اگر (pid == 0) {
        fork();
        fork();
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } وگرنه اگر (what == 15) {
      sbrk(6011);
    } وگرنه اگر (what == 16) {
      اگر (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } وگرنه اگر (what == 17) {
      صحیح pid = fork();
      اگر (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      اگر (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } وگرنه اگر (what == 18) {
      صحیح pid = fork();
      اگر (pid == 0) {
        kill(getpid());
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } وگرنه اگر (what == 19) {
      صحیح fds[2];
      اگر (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      صحیح pid = fork();
      اگر (pid == 0) {
        fork();
        fork();
        اگر (اكتب(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        نویسه c;
        اگر (اقرأ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } وگرنه اگر (what == 20) {
      صحیح pid = fork();
      اگر (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } وگرنه اگر (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } وگرنه اگر (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      صحیح fd1 = open("c", O_CREATE | O_RDWR);
      اگر (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      اگر (اكتب(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      ساختار stat st;
      اگر (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      اگر (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (صحیح)st.size);
        exit(1);
      }
      اگر (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } وگرنه اگر (what == 22) {
      // echo hi | cat
      صحیح aa[2], bb[2];
      اگر (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      اگر (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      صحیح pid1 = fork();
      اگر (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        اگر (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        نویسه *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } وگرنه اگر (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      صحیح pid2 = fork();
      اگر (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        اگر (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        اگر (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        نویسه *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } وگرنه اگر (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      نویسه buf[4] = {0, 0, 0, 0};
      اقرأ(bb[0], buf + 0, 1);
      اقرأ(bb[0], buf + 1, 1);
      اقرأ(bb[0], buf + 2, 1);
      close(bb[0]);
      صحیح st1, st2;
      wait(&st1);
      wait(&st2);
      اگر (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

تهی
iter()
{
  unlink("a");
  unlink("b");

  صحیح pid1 = fork();
  اگر (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  اگر (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  صحیح pid2 = fork();
  اگر (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  اگر (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  صحیح st1 = -1;
  wait(&st1);
  اگر (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  صحیح st2 = -1;
  wait(&st2);

  exit(0);
}

صحیح
اصلی()
{
  وقتی (1) {
    صحیح pid = fork();
    اگر (pid == 0) {
      iter();
      exit(0);
    }
    اگر (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
