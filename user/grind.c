//
// run random system calls in parallel forever.
//

#અન્તર્ભાવ "kernel/param.h"
#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "kernel/syscall.h"
#અન્તર્ભાવ "kernel/memlayout.h"
#અન્તર્ભાવ "kernel/riscv.h"

// from FreeBSD.
પૂર્ણાંક
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
  જો (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  પાછા (x);
}

unsigned long rand_next = 1;

પૂર્ણાંક
rand(શૂન્ય)
{
  પાછા (do_rand(&rand_next));
}

શૂન્ય
go(પૂર્ણાંક which_child)
{
  પૂર્ણાંક fd = -1;
  સ્થિર અક્ષર buf[999];
  અક્ષર *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  જો (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  જ્યારે (1) {
    iters++;
    જો ((iters % 500) == 0)
      લિખ(1, which_child ? "B" : "A", 1);
    પૂર્ણાંક what = rand() % 23;
    જો (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } નહીંતર જો (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } નહીંતર જો (what == 3) {
      unlink("grindir/../a");
    } નહીંતર જો (what == 4) {
      જો (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } નહીંતર જો (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } નહીંતર જો (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } નહીંતર જો (what == 7) {
      લિખ(fd, buf, કદ(buf));
    } નહીંતર જો (what == 8) {
      પઠ(fd, buf, કદ(buf));
    } નહીંતર જો (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } નહીંતર જો (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } નહીંતર જો (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } નહીંતર જો (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } નહીંતર જો (what == 13) {
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } નહીંતર જો (what == 14) {
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        fork();
        fork();
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } નહીંતર જો (what == 15) {
      sbrk(6011);
    } નહીંતર જો (what == 16) {
      જો (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } નહીંતર જો (what == 17) {
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      જો (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } નહીંતર જો (what == 18) {
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        kill(getpid());
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } નહીંતર જો (what == 19) {
      પૂર્ણાંક fds[2];
      જો (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        fork();
        fork();
        જો (લિખ(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        અક્ષર c;
        જો (પઠ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } નહીંતર જો (what == 20) {
      પૂર્ણાંક pid = fork();
      જો (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } નહીંતર જો (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } નહીંતર જો (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      પૂર્ણાંક fd1 = open("c", O_CREATE | O_RDWR);
      જો (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      જો (લિખ(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      સંરચના stat st;
      જો (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      જો (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (પૂર્ણાંક)st.size);
        exit(1);
      }
      જો (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } નહીંતર જો (what == 22) {
      // echo hi | cat
      પૂર્ણાંક aa[2], bb[2];
      જો (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      જો (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      પૂર્ણાંક pid1 = fork();
      જો (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        જો (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        અક્ષર *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } નહીંતર જો (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      પૂર્ણાંક pid2 = fork();
      જો (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        જો (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        જો (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        અક્ષર *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } નહીંતર જો (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      અક્ષર buf[4] = {0, 0, 0, 0};
      પઠ(bb[0], buf + 0, 1);
      પઠ(bb[0], buf + 1, 1);
      પઠ(bb[0], buf + 2, 1);
      close(bb[0]);
      પૂર્ણાંક st1, st2;
      wait(&st1);
      wait(&st2);
      જો (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

શૂન્ય
iter()
{
  unlink("a");
  unlink("b");

  પૂર્ણાંક pid1 = fork();
  જો (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  જો (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  પૂર્ણાંક pid2 = fork();
  જો (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  જો (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  પૂર્ણાંક st1 = -1;
  wait(&st1);
  જો (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  પૂર્ણાંક st2 = -1;
  wait(&st2);

  exit(0);
}

પૂર્ણાંક
મુખ્ય()
{
  જ્યારે (1) {
    પૂર્ણાંક pid = fork();
    જો (pid == 0) {
      iter();
      exit(0);
    }
    જો (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
