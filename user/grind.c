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
整数
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
  もし (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  戻る (x);
}

unsigned long rand_next = 1;

整数
rand(void)
{
  戻る (do_rand(&rand_next));
}

void
go(整数 which_child)
{
  整数 fd = -1;
  static char buf[999];
  char *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  もし (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ながら (1) {
    iters++;
    もし ((iters % 500) == 0)
      write(1, which_child ? "B" : "A", 1);
    整数 what = rand() % 23;
    もし (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } 否则 もし (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } 否则 もし (what == 3) {
      unlink("grindir/../a");
    } 否则 もし (what == 4) {
      もし (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } 否则 もし (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } 否则 もし (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } 否则 もし (what == 7) {
      write(fd, buf, sizeof(buf));
    } 否则 もし (what == 8) {
      read(fd, buf, sizeof(buf));
    } 否则 もし (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } 否则 もし (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } 否则 もし (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } 否则 もし (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } 否则 もし (what == 13) {
      整数 pid = fork();
      もし (pid == 0) {
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } 否则 もし (what == 14) {
      整数 pid = fork();
      もし (pid == 0) {
        fork();
        fork();
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } 否则 もし (what == 15) {
      sbrk(6011);
    } 否则 もし (what == 16) {
      もし (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } 否则 もし (what == 17) {
      整数 pid = fork();
      もし (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      もし (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } 否则 もし (what == 18) {
      整数 pid = fork();
      もし (pid == 0) {
        kill(getpid());
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } 否则 もし (what == 19) {
      整数 fds[2];
      もし (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      整数 pid = fork();
      もし (pid == 0) {
        fork();
        fork();
        もし (write(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        char c;
        もし (read(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } 否则 もし (what == 20) {
      整数 pid = fork();
      もし (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } 否则 もし (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } 否则 もし (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      整数 fd1 = open("c", O_CREATE | O_RDWR);
      もし (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      もし (write(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      struct stat st;
      もし (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      もし (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (整数)st.size);
        exit(1);
      }
      もし (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } 否则 もし (what == 22) {
      // echo hi | cat
      整数 aa[2], bb[2];
      もし (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      もし (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      整数 pid1 = fork();
      もし (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        もし (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        char *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } 否则 もし (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      整数 pid2 = fork();
      もし (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        もし (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        もし (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        char *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } 否则 もし (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      char buf[4] = {0, 0, 0, 0};
      read(bb[0], buf + 0, 1);
      read(bb[0], buf + 1, 1);
      read(bb[0], buf + 2, 1);
      close(bb[0]);
      整数 st1, st2;
      wait(&st1);
      wait(&st2);
      もし (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
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

  整数 pid1 = fork();
  もし (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  もし (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  整数 pid2 = fork();
  もし (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  もし (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  整数 st1 = -1;
  wait(&st1);
  もし (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  整数 st2 = -1;
  wait(&st2);

  exit(0);
}

整数
メインページ()
{
  ながら (1) {
    整数 pid = fork();
    もし (pid == 0) {
      iter();
      exit(0);
    }
    もし (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
