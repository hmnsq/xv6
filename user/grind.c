//
// run random system calls in parallel forever.
//

#അന്തര്ഭാവ "kernel/param.h"
#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "kernel/syscall.h"
#അന്തര്ഭാവ "kernel/memlayout.h"
#അന്തര്ഭാവ "kernel/riscv.h"

// from FreeBSD.
പൂർണ്ണസംഖ്യ
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
  എങ്കിൽ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  തിരികെ (x);
}

unsigned long rand_next = 1;

പൂർണ്ണസംഖ്യ
rand(ശൂന്യം)
{
  തിരികെ (do_rand(&rand_next));
}

ശൂന്യം
go(പൂർണ്ണസംഖ്യ which_child)
{
  പൂർണ്ണസംഖ്യ fd = -1;
  സ്ഥിതം അക്ഷരം buf[999];
  അക്ഷരം *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  എങ്കിൽ (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ആകുമ്പോൾ (1) {
    iters++;
    എങ്കിൽ ((iters % 500) == 0)
      ലിഖ(1, which_child ? "B" : "A", 1);
    പൂർണ്ണസംഖ്യ what = rand() % 23;
    എങ്കിൽ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 3) {
      unlink("grindir/../a");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 4) {
      എങ്കിൽ (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 7) {
      ലിഖ(fd, buf, വലുപ്പം(buf));
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 8) {
      പഠ(fd, buf, വലുപ്പം(buf));
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 13) {
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 14) {
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        fork();
        fork();
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 15) {
      sbrk(6011);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 16) {
      എങ്കിൽ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 17) {
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      എങ്കിൽ (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 18) {
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        kill(getpid());
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 19) {
      പൂർണ്ണസംഖ്യ fds[2];
      എങ്കിൽ (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        fork();
        fork();
        എങ്കിൽ (ലിഖ(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        അക്ഷരം c;
        എങ്കിൽ (പഠ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 20) {
      പൂർണ്ണസംഖ്യ pid = fork();
      എങ്കിൽ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      പൂർണ്ണസംഖ്യ fd1 = open("c", O_CREATE | O_RDWR);
      എങ്കിൽ (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      എങ്കിൽ (ലിഖ(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      ഘടന stat st;
      എങ്കിൽ (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      എങ്കിൽ (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (പൂർണ്ണസംഖ്യ)st.size);
        exit(1);
      }
      എങ്കിൽ (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } അല്ലെങ്കിൽ എങ്കിൽ (what == 22) {
      // echo hi | cat
      പൂർണ്ണസംഖ്യ aa[2], bb[2];
      എങ്കിൽ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      എങ്കിൽ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      പൂർണ്ണസംഖ്യ pid1 = fork();
      എങ്കിൽ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        എങ്കിൽ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        അക്ഷരം *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      പൂർണ്ണസംഖ്യ pid2 = fork();
      എങ്കിൽ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        എങ്കിൽ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        എങ്കിൽ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        അക്ഷരം *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } അല്ലെങ്കിൽ എങ്കിൽ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      അക്ഷരം buf[4] = {0, 0, 0, 0};
      പഠ(bb[0], buf + 0, 1);
      പഠ(bb[0], buf + 1, 1);
      പഠ(bb[0], buf + 2, 1);
      close(bb[0]);
      പൂർണ്ണസംഖ്യ st1, st2;
      wait(&st1);
      wait(&st2);
      എങ്കിൽ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

ശൂന്യം
iter()
{
  unlink("a");
  unlink("b");

  പൂർണ്ണസംഖ്യ pid1 = fork();
  എങ്കിൽ (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  പൂർണ്ണസംഖ്യ pid2 = fork();
  എങ്കിൽ (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  എങ്കിൽ (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  പൂർണ്ണസംഖ്യ st1 = -1;
  wait(&st1);
  എങ്കിൽ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  പൂർണ്ണസംഖ്യ st2 = -1;
  wait(&st2);

  exit(0);
}

പൂർണ്ണസംഖ്യ
പ്രധാന()
{
  ആകുമ്പോൾ (1) {
    പൂർണ്ണസംഖ്യ pid = fork();
    എങ്കിൽ (pid == 0) {
      iter();
      exit(0);
    }
    എങ്കിൽ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
