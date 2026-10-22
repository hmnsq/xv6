//
// run random system calls in parallel forever.
//

#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

// from FreeBSD.
puur_naa_nka
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
  ᨀᨘ (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ᨀᨘᨒ (x);
}

unsigned long rand_next = 1;

puur_naa_nka
rand(void)
{
  ᨀᨘᨒ (do_rand(&rand_next));
}

void
go(puur_naa_nka which_child)
{
  puur_naa_nka fd = -1;
  static var_na buf[999];
  var_na *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ᨀᨘ (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ᨀᨛ (1) {
    iters++;
    ᨀᨘ ((iters % 500) == 0)
      write(1, which_child ? "B" : "A", 1);
    puur_naa_nka what = rand() % 23;
    ᨀᨘ (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ᨀᨘᨁ ᨀᨘ (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ᨀᨘᨁ ᨀᨘ (what == 3) {
      unlink("grindir/../a");
    } ᨀᨘᨁ ᨀᨘ (what == 4) {
      ᨀᨘ (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } ᨀᨘᨁ ᨀᨘ (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ᨀᨘᨁ ᨀᨘ (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ᨀᨘᨁ ᨀᨘ (what == 7) {
      write(fd, buf, parimaa_na(buf));
    } ᨀᨘᨁ ᨀᨘ (what == 8) {
      read(fd, buf, parimaa_na(buf));
    } ᨀᨘᨁ ᨀᨘ (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ᨀᨘᨁ ᨀᨘ (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ᨀᨘᨁ ᨀᨘ (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ᨀᨘᨁ ᨀᨘ (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ᨀᨘᨁ ᨀᨘ (what == 13) {
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 14) {
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        fork();
        fork();
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 15) {
      sbrk(6011);
    } ᨀᨘᨁ ᨀᨘ (what == 16) {
      ᨀᨘ (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ᨀᨘᨁ ᨀᨘ (what == 17) {
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      ᨀᨘ (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 18) {
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        kill(getpid());
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 19) {
      puur_naa_nka fds[2];
      ᨀᨘ (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        fork();
        fork();
        ᨀᨘ (write(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        var_na c;
        ᨀᨘ (read(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 20) {
      puur_naa_nka pid = fork();
      ᨀᨘ (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } ᨀᨘᨁ ᨀᨘ (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ᨀᨘᨁ ᨀᨘ (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      puur_naa_nka fd1 = open("c", O_CREATE | O_RDWR);
      ᨀᨘ (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      ᨀᨘ (write(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      sa_nracanaa stat st;
      ᨀᨘ (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      ᨀᨘ (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (puur_naa_nka)st.size);
        exit(1);
      }
      ᨀᨘ (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } ᨀᨘᨁ ᨀᨘ (what == 22) {
      // echo hi | cat
      puur_naa_nka aa[2], bb[2];
      ᨀᨘ (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ᨀᨘ (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      puur_naa_nka pid1 = fork();
      ᨀᨘ (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ᨀᨘ (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        var_na *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } ᨀᨘᨁ ᨀᨘ (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      puur_naa_nka pid2 = fork();
      ᨀᨘ (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ᨀᨘ (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        ᨀᨘ (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        var_na *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } ᨀᨘᨁ ᨀᨘ (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      var_na buf[4] = {0, 0, 0, 0};
      read(bb[0], buf + 0, 1);
      read(bb[0], buf + 1, 1);
      read(bb[0], buf + 2, 1);
      close(bb[0]);
      puur_naa_nka st1, st2;
      wait(&st1);
      wait(&st2);
      ᨀᨘ (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

void
iter()
{
  unlink("a");
  unlink("b");

  puur_naa_nka pid1 = fork();
  ᨀᨘ (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ᨀᨘ (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  puur_naa_nka pid2 = fork();
  ᨀᨘ (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ᨀᨘ (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  puur_naa_nka st1 = -1;
  wait(&st1);
  ᨀᨘ (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  puur_naa_nka st2 = -1;
  wait(&st2);

  exit(0);
}

puur_naa_nka
main()
{
  ᨀᨛ (1) {
    puur_naa_nka pid = fork();
    ᨀᨘ (pid == 0) {
      iter();
      exit(0);
    }
    ᨀᨘ (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
