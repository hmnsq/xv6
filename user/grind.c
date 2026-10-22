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
do_rand(غیرعلامتی طویل *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  طویل hi, lo, x;

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
  واپس (x);
}

غیرعلامتی طویل rand_اگلا = 1;

صحیح
rand(خالی)
{
  واپس (do_rand(&rand_اگلا));
}

خالی
go(صحیح which_بچہ)
{
  صحیح fd = -1;
  ساکن حرف buf[999];
  حرف *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  اگر (chdir("grindir") != 0) {
    لکھو("grind: chdir grindir failed\n");
    باہر_نکلو(1);
  }
  chdir("/");

  جب (1) {
    iters++;
    اگر ((iters % 500) == 0)
      اكتب(1, which_بچہ ? "B" : "A", 1);
    صحیح what = rand() % 23;
    اگر (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ورنہ اگر (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ورنہ اگر (what == 3) {
      unlink("grindir/../a");
    } ورنہ اگر (what == 4) {
      اگر (chdir("grindir") != 0) {
        لکھو("grind: chdir grindir failed\n");
        باہر_نکلو(1);
      }
      unlink("../b");
      chdir("/");
    } ورنہ اگر (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ورنہ اگر (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ورنہ اگر (what == 7) {
      اكتب(fd, buf, حجم(buf));
    } ورنہ اگر (what == 8) {
      اقرأ(fd, buf, حجم(buf));
    } ورنہ اگر (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ورنہ اگر (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ورنہ اگر (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ورنہ اگر (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ورنہ اگر (what == 13) {
      صحیح pid = fork();
      اگر (pid == 0) {
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      wait(0);
    } ورنہ اگر (what == 14) {
      صحیح pid = fork();
      اگر (pid == 0) {
        fork();
        fork();
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      wait(0);
    } ورنہ اگر (what == 15) {
      sbrk(6011);
    } ورنہ اگر (what == 16) {
      اگر (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ورنہ اگر (what == 17) {
      صحیح pid = fork();
      اگر (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      اگر (chdir("../grindir/..") != 0) {
        لکھو("grind: chdir failed\n");
        باہر_نکلو(1);
      }
      kill(pid);
      wait(0);
    } ورنہ اگر (what == 18) {
      صحیح pid = fork();
      اگر (pid == 0) {
        kill(getpid());
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      wait(0);
    } ورنہ اگر (what == 19) {
      صحیح fds[2];
      اگر (pipe(fds) < 0) {
        لکھو("grind: pipe failed\n");
        باہر_نکلو(1);
      }
      صحیح pid = fork();
      اگر (pid == 0) {
        fork();
        fork();
        اگر (اكتب(fds[1], "x", 1) != 1)
          لکھو("grind: pipe write failed\n");
        حرف c;
        اگر (اقرأ(fds[0], &c, 1) != 1)
          لکھو("grind: pipe read failed\n");
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ورنہ اگر (what == 20) {
      صحیح pid = fork();
      اگر (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        باہر_نکلو(0);
      } ورنہ اگر (pid < 0) {
        لکھو("grind: fork failed\n");
        باہر_نکلو(1);
      }
      wait(0);
    } ورنہ اگر (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      صحیح fd1 = open("c", O_CREATE | O_RDWR);
      اگر (fd1 < 0) {
        لکھو("grind: create c failed\n");
        باہر_نکلو(1);
      }
      اگر (اكتب(fd1, "x", 1) != 1) {
        لکھو("grind: write c failed\n");
        باہر_نکلو(1);
      }
      ساخت stat st;
      اگر (fstat(fd1, &st) != 0) {
        لکھو("grind: fstat failed\n");
        باہر_نکلو(1);
      }
      اگر (st.سائز != 1) {
        لکھو("grind: fstat reports wrong size %d\n", (صحیح)st.سائز);
        باہر_نکلو(1);
      }
      اگر (st.ino > 200) {
        لکھو("grind: fstat reports crazy i-number %d\n", st.ino);
        باہر_نکلو(1);
      }
      close(fd1);
      unlink("c");
    } ورنہ اگر (what == 22) {
      // echo hi | cat
      صحیح aa[2], bb[2];
      اگر (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        باہر_نکلو(1);
      }
      اگر (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        باہر_نکلو(1);
      }
      صحیح pid1 = fork();
      اگر (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        اگر (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          باہر_نکلو(1);
        }
        close(aa[1]);
        حرف *دلائل[3] = {"echo", "hi", 0};
        exec("grindir/../echo", دلائل);
        fprintf(2, "grind: echo: not found\n");
        باہر_نکلو(2);
      } ورنہ اگر (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        باہر_نکلو(3);
      }
      صحیح pid2 = fork();
      اگر (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        اگر (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          باہر_نکلو(4);
        }
        close(aa[0]);
        close(1);
        اگر (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          باہر_نکلو(5);
        }
        close(bb[1]);
        حرف *دلائل[2] = {"cat", 0};
        exec("/cat", دلائل);
        fprintf(2, "grind: cat: not found\n");
        باہر_نکلو(6);
      } ورنہ اگر (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        باہر_نکلو(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      حرف buf[4] = {0, 0, 0, 0};
      اقرأ(bb[0], buf + 0, 1);
      اقرأ(bb[0], buf + 1, 1);
      اقرأ(bb[0], buf + 2, 1);
      close(bb[0]);
      صحیح st1, st2;
      wait(&st1);
      wait(&st2);
      اگر (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        لکھو("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        باہر_نکلو(1);
      }
    }
  }
}

خالی
iter()
{
  unlink("a");
  unlink("b");

  صحیح pid1 = fork();
  اگر (pid1 < 0) {
    لکھو("grind: fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid1 == 0) {
    rand_اگلا ^= 31;
    go(0);
    باہر_نکلو(0);
  }

  صحیح pid2 = fork();
  اگر (pid2 < 0) {
    لکھو("grind: fork failed\n");
    باہر_نکلو(1);
  }
  اگر (pid2 == 0) {
    rand_اگلا ^= 7177;
    go(1);
    باہر_نکلو(0);
  }

  صحیح st1 = -1;
  wait(&st1);
  اگر (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  صحیح st2 = -1;
  wait(&st2);

  باہر_نکلو(0);
}

صحیح
مین()
{
  جب (1) {
    صحیح pid = fork();
    اگر (pid == 0) {
      iter();
      باہر_نکلو(0);
    }
    اگر (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_اگلا += 1;
  }
}
