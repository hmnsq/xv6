//
// run random system calls in parallel forever.
//

#அந்தர்பாவ "kernel/param.h"
#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "kernel/syscall.h"
#அந்தர்பாவ "kernel/memlayout.h"
#அந்தர்பாவ "kernel/riscv.h"

// from FreeBSD.
முழு
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
  எனில் (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  திரும்பு (x);
}

unsigned long rand_next = 1;

முழு
rand(வெறுமை)
{
  திரும்பு (do_rand(&rand_next));
}

வெறுமை
go(முழு which_child)
{
  முழு fd = -1;
  நிலையான எழுத்து buf[999];
  எழுத்து *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  எனில் (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  போது (1) {
    iters++;
    எனில் ((iters % 500) == 0)
      லிக(1, which_child ? "B" : "A", 1);
    முழு what = rand() % 23;
    எனில் (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } இல்லையெனில் எனில் (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } இல்லையெனில் எனில் (what == 3) {
      unlink("grindir/../a");
    } இல்லையெனில் எனில் (what == 4) {
      எனில் (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } இல்லையெனில் எனில் (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } இல்லையெனில் எனில் (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } இல்லையெனில் எனில் (what == 7) {
      லிக(fd, buf, அளவு(buf));
    } இல்லையெனில் எனில் (what == 8) {
      பட(fd, buf, அளவு(buf));
    } இல்லையெனில் எனில் (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } இல்லையெனில் எனில் (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } இல்லையெனில் எனில் (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } இல்லையெனில் எனில் (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } இல்லையெனில் எனில் (what == 13) {
      முழு pid = fork();
      எனில் (pid == 0) {
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } இல்லையெனில் எனில் (what == 14) {
      முழு pid = fork();
      எனில் (pid == 0) {
        fork();
        fork();
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } இல்லையெனில் எனில் (what == 15) {
      sbrk(6011);
    } இல்லையெனில் எனில் (what == 16) {
      எனில் (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } இல்லையெனில் எனில் (what == 17) {
      முழு pid = fork();
      எனில் (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      எனில் (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } இல்லையெனில் எனில் (what == 18) {
      முழு pid = fork();
      எனில் (pid == 0) {
        kill(getpid());
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } இல்லையெனில் எனில் (what == 19) {
      முழு fds[2];
      எனில் (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      முழு pid = fork();
      எனில் (pid == 0) {
        fork();
        fork();
        எனில் (லிக(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        எழுத்து c;
        எனில் (பட(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } இல்லையெனில் எனில் (what == 20) {
      முழு pid = fork();
      எனில் (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } இல்லையெனில் எனில் (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } இல்லையெனில் எனில் (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      முழு fd1 = open("c", O_CREATE | O_RDWR);
      எனில் (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      எனில் (லிக(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      கட்டமைப்பு stat st;
      எனில் (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      எனில் (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (முழு)st.size);
        exit(1);
      }
      எனில் (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } இல்லையெனில் எனில் (what == 22) {
      // echo hi | cat
      முழு aa[2], bb[2];
      எனில் (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      எனில் (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      முழு pid1 = fork();
      எனில் (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        எனில் (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        எழுத்து *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } இல்லையெனில் எனில் (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      முழு pid2 = fork();
      எனில் (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        எனில் (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        எனில் (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        எழுத்து *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } இல்லையெனில் எனில் (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      எழுத்து buf[4] = {0, 0, 0, 0};
      பட(bb[0], buf + 0, 1);
      பட(bb[0], buf + 1, 1);
      பட(bb[0], buf + 2, 1);
      close(bb[0]);
      முழு st1, st2;
      wait(&st1);
      wait(&st2);
      எனில் (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

வெறுமை
iter()
{
  unlink("a");
  unlink("b");

  முழு pid1 = fork();
  எனில் (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  எனில் (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  முழு pid2 = fork();
  எனில் (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  எனில் (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  முழு st1 = -1;
  wait(&st1);
  எனில் (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  முழு st2 = -1;
  wait(&st2);

  exit(0);
}

முழு
முதன்மை()
{
  போது (1) {
    முழு pid = fork();
    எனில் (pid == 0) {
      iter();
      exit(0);
    }
    எனில் (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
