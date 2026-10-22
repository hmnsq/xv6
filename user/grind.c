//
// run random system calls in parallel forever.
//

#ಅನ್ತರ್ಭಾವ "kernel/param.h"
#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "kernel/syscall.h"
#ಅನ್ತರ್ಭಾವ "kernel/memlayout.h"
#ಅನ್ತರ್ಭಾವ "kernel/riscv.h"

// from FreeBSD.
ಪೂರ್ಣಾಙ್ಕ
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
  ಯದಿ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ಪ್ರತ್ಯಾಗಮ (x);
}

unsigned long rand_next = 1;

ಪೂರ್ಣಾಙ್ಕ
rand(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ (do_rand(&rand_next));
}

ಶೂನ್ಯ
go(ಪೂರ್ಣಾಙ್ಕ which_child)
{
  ಪೂರ್ಣಾಙ್ಕ fd = -1;
  ಸ್ಥಿರ ವರ್ಣ buf[999];
  ವರ್ಣ *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ಯದಿ (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ಯಾವತ್ (1) {
    iters++;
    ಯದಿ ((iters % 500) == 0)
      ಲಿಖ(1, which_child ? "B" : "A", 1);
    ಪೂರ್ಣಾಙ್ಕ what = rand() % 23;
    ಯದಿ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ಅನ್ಯಥಾ ಯದಿ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ಅನ್ಯಥಾ ಯದಿ (what == 3) {
      unlink("grindir/../a");
    } ಅನ್ಯಥಾ ಯದಿ (what == 4) {
      ಯದಿ (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } ಅನ್ಯಥಾ ಯದಿ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ಅನ್ಯಥಾ ಯದಿ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ಅನ್ಯಥಾ ಯದಿ (what == 7) {
      ಲಿಖ(fd, buf, ಪರಿಮಾಣ(buf));
    } ಅನ್ಯಥಾ ಯದಿ (what == 8) {
      ಪಠ(fd, buf, ಪರಿಮಾಣ(buf));
    } ಅನ್ಯಥಾ ಯದಿ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ಅನ್ಯಥಾ ಯದಿ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ಅನ್ಯಥಾ ಯದಿ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ಅನ್ಯಥಾ ಯದಿ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ಅನ್ಯಥಾ ಯದಿ (what == 13) {
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 14) {
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        fork();
        fork();
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 15) {
      sbrk(6011);
    } ಅನ್ಯಥಾ ಯದಿ (what == 16) {
      ಯದಿ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ಅನ್ಯಥಾ ಯದಿ (what == 17) {
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      ಯದಿ (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 18) {
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        kill(getpid());
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 19) {
      ಪೂರ್ಣಾಙ್ಕ fds[2];
      ಯದಿ (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        fork();
        fork();
        ಯದಿ (ಲಿಖ(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        ವರ್ಣ c;
        ಯದಿ (ಪಠ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 20) {
      ಪೂರ್ಣಾಙ್ಕ pid = fork();
      ಯದಿ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } ಅನ್ಯಥಾ ಯದಿ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ಅನ್ಯಥಾ ಯದಿ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      ಪೂರ್ಣಾಙ್ಕ fd1 = open("c", O_CREATE | O_RDWR);
      ಯದಿ (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      ಯದಿ (ಲಿಖ(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      ಸಂರಚನಾ stat st;
      ಯದಿ (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      ಯದಿ (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (ಪೂರ್ಣಾಙ್ಕ)st.size);
        exit(1);
      }
      ಯದಿ (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } ಅನ್ಯಥಾ ಯದಿ (what == 22) {
      // echo hi | cat
      ಪೂರ್ಣಾಙ್ಕ aa[2], bb[2];
      ಯದಿ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ಯದಿ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
      ಯದಿ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ಯದಿ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        ವರ್ಣ *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } ಅನ್ಯಥಾ ಯದಿ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      ಪೂರ್ಣಾಙ್ಕ pid2 = fork();
      ಯದಿ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ಯದಿ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        ಯದಿ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        ವರ್ಣ *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } ಅನ್ಯಥಾ ಯದಿ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      ವರ್ಣ buf[4] = {0, 0, 0, 0};
      ಪಠ(bb[0], buf + 0, 1);
      ಪಠ(bb[0], buf + 1, 1);
      ಪಠ(bb[0], buf + 2, 1);
      close(bb[0]);
      ಪೂರ್ಣಾಙ್ಕ st1, st2;
      wait(&st1);
      wait(&st2);
      ಯದಿ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

ಶೂನ್ಯ
iter()
{
  unlink("a");
  unlink("b");

  ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
  ಯದಿ (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ಯದಿ (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  ಪೂರ್ಣಾಙ್ಕ pid2 = fork();
  ಯದಿ (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ಯದಿ (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  ಪೂರ್ಣಾಙ್ಕ st1 = -1;
  wait(&st1);
  ಯದಿ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  ಪೂರ್ಣಾಙ್ಕ st2 = -1;
  wait(&st2);

  exit(0);
}

ಪೂರ್ಣಾಙ್ಕ
main()
{
  ಯಾವತ್ (1) {
    ಪೂರ್ಣಾಙ್ಕ pid = fork();
    ಯದಿ (pid == 0) {
      iter();
      exit(0);
    }
    ಯದಿ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
