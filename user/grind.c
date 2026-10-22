//
// run random system calls in parallel forever.
//

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/syscall.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/riscv.h"

// from FreeBSD.
ਪੂਰਨ
do_rand(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ctx)
{
  /*
   * Compute x = (7^5 * x) mod (2^31 - 1)
   * without overflowing 31 bits:
   *      (2^31 - 1) = 127773 * (7^5) + 2836
   * From "Random number generators: good ones are hard to find",
   * Park and Miller, Communications of the ACM, vol. 31, no. 10,
   * October 1988, p. 1195.
   */
  ਲੰਮਾ hi, lo, x;

  /* Transform to [1, 0x7ffffffe] range. */
  x = (*ctx % 0x7ffffffe) + 1;
  hi = x / 127773;
  lo = x % 127773;
  x = 16807 * lo - 2836 * hi;
  ਜੇ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ਵਾਪਸ (x);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rand_ਅਗਲਾ = 1;

ਪੂਰਨ
rand(ਖਾਲੀ)
{
  ਵਾਪਸ (do_rand(&rand_ਅਗਲਾ));
}

ਖਾਲੀ
go(ਪੂਰਨ which_ਬੱਚਾ)
{
  ਪੂਰਨ fd = -1;
  ਸਥਿਰ ਅੱਖਰ buf[999];
  ਅੱਖਰ *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ਜੇ (chdir("grindir") != 0) {
    ਲਿਖੋ("grind: chdir grindir failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  chdir("/");

  ਜਦੋਂ (1) {
    iters++;
    ਜੇ ((iters % 500) == 0)
      ਲਿਖ(1, which_ਬੱਚਾ ? "B" : "A", 1);
    ਪੂਰਨ what = rand() % 23;
    ਜੇ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ਨਹੀਂਤਾਂ ਜੇ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ਨਹੀਂਤਾਂ ਜੇ (what == 3) {
      unlink("grindir/../a");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 4) {
      ਜੇ (chdir("grindir") != 0) {
        ਲਿਖੋ("grind: chdir grindir failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      unlink("../b");
      chdir("/");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 7) {
      ਲਿਖ(fd, buf, ਆਕਾਰ(buf));
    } ਨਹੀਂਤਾਂ ਜੇ (what == 8) {
      ਪਠ(fd, buf, ਆਕਾਰ(buf));
    } ਨਹੀਂਤਾਂ ਜੇ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 13) {
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 14) {
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        fork();
        fork();
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 15) {
      sbrk(6011);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 16) {
      ਜੇ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ਨਹੀਂਤਾਂ ਜੇ (what == 17) {
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (chdir("../grindir/..") != 0) {
        ਲਿਖੋ("grind: chdir failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      kill(pid);
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 18) {
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        kill(getpid());
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 19) {
      ਪੂਰਨ fds[2];
      ਜੇ (pipe(fds) < 0) {
        ਲਿਖੋ("grind: pipe failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        fork();
        fork();
        ਜੇ (ਲਿਖ(fds[1], "x", 1) != 1)
          ਲਿਖੋ("grind: pipe write failed\n");
        ਅੱਖਰ c;
        ਜੇ (ਪਠ(fds[0], &c, 1) != 1)
          ਲਿਖੋ("grind: pipe read failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 20) {
      ਪੂਰਨ pid = fork();
      ਜੇ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        ਬਾਹਰ_ਨਿਕਲੋ(0);
      } ਨਹੀਂਤਾਂ ਜੇ (pid < 0) {
        ਲਿਖੋ("grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      wait(0);
    } ਨਹੀਂਤਾਂ ਜੇ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      ਪੂਰਨ fd1 = open("c", O_CREATE | O_RDWR);
      ਜੇ (fd1 < 0) {
        ਲਿਖੋ("grind: create c failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (ਲਿਖ(fd1, "x", 1) != 1) {
        ਲਿਖੋ("grind: write c failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਬਣਤਰ stat st;
      ਜੇ (fstat(fd1, &st) != 0) {
        ਲਿਖੋ("grind: fstat failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (st.ਮਾਪ != 1) {
        ਲਿਖੋ("grind: fstat reports wrong size %d\n", (ਪੂਰਨ)st.ਮਾਪ);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (st.ino > 200) {
        ਲਿਖੋ("grind: fstat reports crazy i-number %d\n", st.ino);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      close(fd1);
      unlink("c");
    } ਨਹੀਂਤਾਂ ਜੇ (what == 22) {
      // echo hi | cat
      ਪੂਰਨ aa[2], bb[2];
      ਜੇ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਜੇ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      ਪੂਰਨ pid1 = fork();
      ਜੇ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ਜੇ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
        close(aa[1]);
        ਅੱਖਰ *ਦਲੀਲਾਂ[3] = {"echo", "hi", 0};
        exec("grindir/../echo", ਦਲੀਲਾਂ);
        fprintf(2, "grind: echo: not found\n");
        ਬਾਹਰ_ਨਿਕਲੋ(2);
      } ਨਹੀਂਤਾਂ ਜੇ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(3);
      }
      ਪੂਰਨ pid2 = fork();
      ਜੇ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ਜੇ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          ਬਾਹਰ_ਨਿਕਲੋ(4);
        }
        close(aa[0]);
        close(1);
        ਜੇ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          ਬਾਹਰ_ਨਿਕਲੋ(5);
        }
        close(bb[1]);
        ਅੱਖਰ *ਦਲੀਲਾਂ[2] = {"cat", 0};
        exec("/cat", ਦਲੀਲਾਂ);
        fprintf(2, "grind: cat: not found\n");
        ਬਾਹਰ_ਨਿਕਲੋ(6);
      } ਨਹੀਂਤਾਂ ਜੇ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        ਬਾਹਰ_ਨਿਕਲੋ(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      ਅੱਖਰ buf[4] = {0, 0, 0, 0};
      ਪਠ(bb[0], buf + 0, 1);
      ਪਠ(bb[0], buf + 1, 1);
      ਪਠ(bb[0], buf + 2, 1);
      close(bb[0]);
      ਪੂਰਨ st1, st2;
      wait(&st1);
      wait(&st2);
      ਜੇ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        ਲਿਖੋ("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
    }
  }
}

ਖਾਲੀ
iter()
{
  unlink("a");
  unlink("b");

  ਪੂਰਨ pid1 = fork();
  ਜੇ (pid1 < 0) {
    ਲਿਖੋ("grind: fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid1 == 0) {
    rand_ਅਗਲਾ ^= 31;
    go(0);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਪੂਰਨ pid2 = fork();
  ਜੇ (pid2 < 0) {
    ਲਿਖੋ("grind: fork failed\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (pid2 == 0) {
    rand_ਅਗਲਾ ^= 7177;
    go(1);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਪੂਰਨ st1 = -1;
  wait(&st1);
  ਜੇ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  ਪੂਰਨ st2 = -1;
  wait(&st2);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਪੂਰਨ
ਮੁੱਖ()
{
  ਜਦੋਂ (1) {
    ਪੂਰਨ pid = fork();
    ਜੇ (pid == 0) {
      iter();
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
    ਜੇ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_ਅਗਲਾ += 1;
  }
}
