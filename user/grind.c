//
// run random system calls in parallel forever.
//

#ଅନ୍ତର୍ଭାଵ "kernel/param.h"
#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "kernel/syscall.h"
#ଅନ୍ତର୍ଭାଵ "kernel/memlayout.h"
#ଅନ୍ତର୍ଭାଵ "kernel/riscv.h"

// from FreeBSD.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
  ଯଦି (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ଫେରନ୍ତୁ (x);
}

unsigned long rand_next = 1;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rand(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ (do_rand(&rand_next));
}

ଶୂନ୍ୟ
go(ପୂର୍ଣ୍ଣସଂଖ୍ୟା which_child)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -1;
  ସ୍ଥିର ଅକ୍ଷର buf[999];
  ଅକ୍ଷର *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  ଯଦି (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  ଯେତେବେଳେ (1) {
    iters++;
    ଯଦି ((iters % 500) == 0)
      ଲିଖ(1, which_child ? "B" : "A", 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା what = rand() % 23;
    ଯଦି (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } ଅନ୍ୟଥା ଯଦି (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } ଅନ୍ୟଥା ଯଦି (what == 3) {
      unlink("grindir/../a");
    } ଅନ୍ୟଥା ଯଦି (what == 4) {
      ଯଦି (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } ଅନ୍ୟଥା ଯଦି (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } ଅନ୍ୟଥା ଯଦି (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } ଅନ୍ୟଥା ଯଦି (what == 7) {
      ଲିଖ(fd, buf, ଆକାର(buf));
    } ଅନ୍ୟଥା ଯଦି (what == 8) {
      ପଠ(fd, buf, ଆକାର(buf));
    } ଅନ୍ୟଥା ଯଦି (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } ଅନ୍ୟଥା ଯଦି (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } ଅନ୍ୟଥା ଯଦି (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } ଅନ୍ୟଥା ଯଦି (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } ଅନ୍ୟଥା ଯଦି (what == 13) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 14) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        fork();
        fork();
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 15) {
      sbrk(6011);
    } ଅନ୍ୟଥା ଯଦି (what == 16) {
      ଯଦି (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } ଅନ୍ୟଥା ଯଦି (what == 17) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      ଯଦି (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 18) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        kill(getpid());
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 19) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[2];
      ଯଦି (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        fork();
        fork();
        ଯଦି (ଲିଖ(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        ଅକ୍ଷର c;
        ଯଦି (ପଠ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 20) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
      ଯଦି (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } ଅନ୍ୟଥା ଯଦି (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } ଅନ୍ୟଥା ଯଦି (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd1 = open("c", O_CREATE | O_RDWR);
      ଯଦି (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      ଯଦି (ଲିଖ(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      ସଂରଚନା stat st;
      ଯଦି (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      ଯଦି (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)st.size);
        exit(1);
      }
      ଯଦି (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } ଅନ୍ୟଥା ଯଦି (what == 22) {
      // echo hi | cat
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା aa[2], bb[2];
      ଯଦି (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ଯଦି (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
      ଯଦି (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        ଯଦି (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        ଅକ୍ଷର *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } ଅନ୍ୟଥା ଯଦି (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid2 = fork();
      ଯଦି (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        ଯଦି (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        ଯଦି (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        ଅକ୍ଷର *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } ଅନ୍ୟଥା ଯଦି (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      ଅକ୍ଷର buf[4] = {0, 0, 0, 0};
      ପଠ(bb[0], buf + 0, 1);
      ପଠ(bb[0], buf + 1, 1);
      ପଠ(bb[0], buf + 2, 1);
      close(bb[0]);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା st1, st2;
      wait(&st1);
      wait(&st2);
      ଯଦି (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

ଶୂନ୍ୟ
iter()
{
  unlink("a");
  unlink("b");

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
  ଯଦି (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ଯଦି (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid2 = fork();
  ଯଦି (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  ଯଦି (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା st1 = -1;
  wait(&st1);
  ଯଦି (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା st2 = -1;
  wait(&st2);

  exit(0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ()
{
  ଯେତେବେଳେ (1) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = fork();
    ଯଦି (pid == 0) {
      iter();
      exit(0);
    }
    ଯଦି (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
