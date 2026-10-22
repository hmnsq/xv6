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
số_nguyên
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
  nếu (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  trả_về (x);
}

unsigned long rand_next = 1;

số_nguyên
rand(void)
{
  trả_về (do_rand(&rand_next));
}

void
go(số_nguyên which_child)
{
  số_nguyên fd = -1;
  static char buf[999];
  char *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  nếu (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  trong_khi (1) {
    iters++;
    nếu ((iters % 500) == 0)
      write(1, which_child ? "B" : "A", 1);
    số_nguyên what = rand() % 23;
    nếu (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } không_thì nếu (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } không_thì nếu (what == 3) {
      unlink("grindir/../a");
    } không_thì nếu (what == 4) {
      nếu (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } không_thì nếu (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } không_thì nếu (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } không_thì nếu (what == 7) {
      write(fd, buf, sizeof(buf));
    } không_thì nếu (what == 8) {
      read(fd, buf, sizeof(buf));
    } không_thì nếu (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } không_thì nếu (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } không_thì nếu (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } không_thì nếu (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } không_thì nếu (what == 13) {
      số_nguyên pid = fork();
      nếu (pid == 0) {
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } không_thì nếu (what == 14) {
      số_nguyên pid = fork();
      nếu (pid == 0) {
        fork();
        fork();
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } không_thì nếu (what == 15) {
      sbrk(6011);
    } không_thì nếu (what == 16) {
      nếu (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } không_thì nếu (what == 17) {
      số_nguyên pid = fork();
      nếu (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      nếu (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } không_thì nếu (what == 18) {
      số_nguyên pid = fork();
      nếu (pid == 0) {
        kill(getpid());
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } không_thì nếu (what == 19) {
      số_nguyên fds[2];
      nếu (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      số_nguyên pid = fork();
      nếu (pid == 0) {
        fork();
        fork();
        nếu (write(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        char c;
        nếu (read(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } không_thì nếu (what == 20) {
      số_nguyên pid = fork();
      nếu (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } không_thì nếu (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } không_thì nếu (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      số_nguyên fd1 = open("c", O_CREATE | O_RDWR);
      nếu (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      nếu (write(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      struct stat st;
      nếu (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      nếu (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (số_nguyên)st.size);
        exit(1);
      }
      nếu (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } không_thì nếu (what == 22) {
      // echo hi | cat
      số_nguyên aa[2], bb[2];
      nếu (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      nếu (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      số_nguyên pid1 = fork();
      nếu (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        nếu (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        char *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } không_thì nếu (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      số_nguyên pid2 = fork();
      nếu (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        nếu (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        nếu (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        char *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } không_thì nếu (pid2 < 0) {
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
      số_nguyên st1, st2;
      wait(&st1);
      wait(&st2);
      nếu (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
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

  số_nguyên pid1 = fork();
  nếu (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  nếu (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  số_nguyên pid2 = fork();
  nếu (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  nếu (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  số_nguyên st1 = -1;
  wait(&st1);
  nếu (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  số_nguyên st2 = -1;
  wait(&st2);

  exit(0);
}

số_nguyên
main()
{
  trong_khi (1) {
    số_nguyên pid = fork();
    nếu (pid == 0) {
      iter();
      exit(0);
    }
    nếu (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
