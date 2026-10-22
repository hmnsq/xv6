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
ಪೂರ್ಣಾಂಕ
do_rand(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  ದೀರ್ಘ hi, lo, x;

  /* Transform to [1, 0x7ffffffe] range. */
  x = (*ctx % 0x7ffffffe) + 1;
  hi = x / 127773;
  lo = x % 127773;
  x = 16807 * lo - 2836 * hi;
  ಆದರೆ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ಹಿಂತಿರುಗಿ (x);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rand_ಮುಂದಿನ = 1;

ಪೂರ್ಣಾಂಕ
rand(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ (do_rand(&rand_ಮುಂದಿನ));
}

ಶೂನ್ಯ
go(ಪೂರ್ಣಾಂಕ which_ಮಗು)
{
  ಪೂರ್ಣಾಂಕ fd = -1;
  ಸ್ಥಿರ ಅಕ್ಷರ buf[999];
  ಅಕ್ಷರ *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ಆದರೆ (chdir("grindir") != 0) {
    ಬರೆ("grind: chdir grindir failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  chdir("/");

  ಆಗ (1) {
    iters++;
    ಆದರೆ ((iters % 500) == 0)
      ಲಿಖ(1, which_ಮಗು ? "B" : "A", 1);
    ಪೂರ್ಣಾಂಕ what = rand() % 23;
    ಆದರೆ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 3) {
      unlink("grindir/../a");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 4) {
      ಆದರೆ (chdir("grindir") != 0) {
        ಬರೆ("grind: chdir grindir failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      unlink("../b");
      chdir("/");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 7) {
      ಲಿಖ(fd, buf, ಗಾತ್ರ(buf));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 8) {
      ಪಠ(fd, buf, ಗಾತ್ರ(buf));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 13) {
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 14) {
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        fork();
        fork();
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 15) {
      sbrk(6011);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 16) {
      ಆದರೆ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 17) {
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (chdir("../grindir/..") != 0) {
        ಬರೆ("grind: chdir failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      kill(pid);
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 18) {
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        kill(getpid());
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 19) {
      ಪೂರ್ಣಾಂಕ fds[2];
      ಆದರೆ (pipe(fds) < 0) {
        ಬರೆ("grind: pipe failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        fork();
        fork();
        ಆದರೆ (ಲಿಖ(fds[1], "x", 1) != 1)
          ಬರೆ("grind: pipe write failed\n");
        ಅಕ್ಷರ c;
        ಆದರೆ (ಪಠ(fds[0], &c, 1) != 1)
          ಬರೆ("grind: pipe read failed\n");
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 20) {
      ಪೂರ್ಣಾಂಕ pid = fork();
      ಆದರೆ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        ನಿರ್ಗಮಿಸು(0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid < 0) {
        ಬರೆ("grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      wait(0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      ಪೂರ್ಣಾಂಕ fd1 = open("c", O_CREATE | O_RDWR);
      ಆದರೆ (fd1 < 0) {
        ಬರೆ("grind: create c failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (ಲಿಖ(fd1, "x", 1) != 1) {
        ಬರೆ("grind: write c failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ರಚನೆ stat st;
      ಆದರೆ (fstat(fd1, &st) != 0) {
        ಬರೆ("grind: fstat failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (st.ಗಾತ್ರ_2 != 1) {
        ಬರೆ("grind: fstat reports wrong size %d\n", (ಪೂರ್ಣಾಂಕ)st.ಗಾತ್ರ_2);
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (st.ino > 200) {
        ಬರೆ("grind: fstat reports crazy i-number %d\n", st.ino);
        ನಿರ್ಗಮಿಸು(1);
      }
      close(fd1);
      unlink("c");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == 22) {
      // echo hi | cat
      ಪೂರ್ಣಾಂಕ aa[2], bb[2];
      ಆದರೆ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಆದರೆ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        ನಿರ್ಗಮಿಸು(1);
      }
      ಪೂರ್ಣಾಂಕ pid1 = fork();
      ಆದರೆ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ಆದರೆ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          ನಿರ್ಗಮಿಸು(1);
        }
        close(aa[1]);
        ಅಕ್ಷರ *ವಾದಗಳು[3] = {"echo", "hi", 0};
        exec("grindir/../echo", ವಾದಗಳು);
        fprintf(2, "grind: echo: not found\n");
        ನಿರ್ಗಮಿಸು(2);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(3);
      }
      ಪೂರ್ಣಾಂಕ pid2 = fork();
      ಆದರೆ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ಆದರೆ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          ನಿರ್ಗಮಿಸು(4);
        }
        close(aa[0]);
        close(1);
        ಆದರೆ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          ನಿರ್ಗಮಿಸು(5);
        }
        close(bb[1]);
        ಅಕ್ಷರ *ವಾದಗಳು[2] = {"cat", 0};
        exec("/cat", ವಾದಗಳು);
        fprintf(2, "grind: cat: not found\n");
        ನಿರ್ಗಮಿಸು(6);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        ನಿರ್ಗಮಿಸು(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      ಅಕ್ಷರ buf[4] = {0, 0, 0, 0};
      ಪಠ(bb[0], buf + 0, 1);
      ಪಠ(bb[0], buf + 1, 1);
      ಪಠ(bb[0], buf + 2, 1);
      close(bb[0]);
      ಪೂರ್ಣಾಂಕ st1, st2;
      wait(&st1);
      wait(&st2);
      ಆದರೆ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        ಬರೆ("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        ನಿರ್ಗಮಿಸು(1);
      }
    }
  }
}

ಶೂನ್ಯ
iter()
{
  unlink("a");
  unlink("b");

  ಪೂರ್ಣಾಂಕ pid1 = fork();
  ಆದರೆ (pid1 < 0) {
    ಬರೆ("grind: fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid1 == 0) {
    rand_ಮುಂದಿನ ^= 31;
    go(0);
    ನಿರ್ಗಮಿಸು(0);
  }

  ಪೂರ್ಣಾಂಕ pid2 = fork();
  ಆದರೆ (pid2 < 0) {
    ಬರೆ("grind: fork failed\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (pid2 == 0) {
    rand_ಮುಂದಿನ ^= 7177;
    go(1);
    ನಿರ್ಗಮಿಸು(0);
  }

  ಪೂರ್ಣಾಂಕ st1 = -1;
  wait(&st1);
  ಆದರೆ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  ಪೂರ್ಣಾಂಕ st2 = -1;
  wait(&st2);

  ನಿರ್ಗಮಿಸು(0);
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ()
{
  ಆಗ (1) {
    ಪೂರ್ಣಾಂಕ pid = fork();
    ಆದರೆ (pid == 0) {
      iter();
      ನಿರ್ಗಮಿಸು(0);
    }
    ಆದರೆ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_ಮುಂದಿನ += 1;
  }
}
