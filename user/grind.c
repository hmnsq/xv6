//
// run random system calls in parallel forever.
//

#অন্তর্ভাব "kernel/param.h"
#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "kernel/syscall.h"
#অন্তর্ভাব "kernel/memlayout.h"
#অন্তর্ভাব "kernel/riscv.h"

// from FreeBSD.
পূর্ণসংখ্যা
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
  যদি (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  ফেরত (x);
}

unsigned long rand_পরবর্তী = 1;

পূর্ণসংখ্যা
rand(শূন্য)
{
  ফেরত (do_rand(&rand_পরবর্তী));
}

শূন্য
go(পূর্ণসংখ্যা which_child)
{
  পূর্ণসংখ্যা fd = -1;
  স্থির অক্ষর buf[999];
  অক্ষর *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  যদি (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  যখন (1) {
    iters++;
    যদি ((iters % 500) == 0)
      লিখ(1, which_child ? "B" : "A", 1);
    পূর্ণসংখ্যা what = rand() % 23;
    যদি (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } অন্যথা যদি (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } অন্যথা যদি (what == 3) {
      unlink("grindir/../a");
    } অন্যথা যদি (what == 4) {
      যদি (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } অন্যথা যদি (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } অন্যথা যদি (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } অন্যথা যদি (what == 7) {
      লিখ(fd, buf, আকার(buf));
    } অন্যথা যদি (what == 8) {
      পঠ(fd, buf, আকার(buf));
    } অন্যথা যদি (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } অন্যথা যদি (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } অন্যথা যদি (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } অন্যথা যদি (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } অন্যথা যদি (what == 13) {
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } অন্যথা যদি (what == 14) {
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        fork();
        fork();
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } অন্যথা যদি (what == 15) {
      sbrk(6011);
    } অন্যথা যদি (what == 16) {
      যদি (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } অন্যথা যদি (what == 17) {
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      যদি (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } অন্যথা যদি (what == 18) {
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        kill(getpid());
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } অন্যথা যদি (what == 19) {
      পূর্ণসংখ্যা fds[2];
      যদি (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        fork();
        fork();
        যদি (লিখ(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        অক্ষর c;
        যদি (পঠ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } অন্যথা যদি (what == 20) {
      পূর্ণসংখ্যা pid = fork();
      যদি (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } অন্যথা যদি (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } অন্যথা যদি (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      পূর্ণসংখ্যা fd1 = open("c", O_CREATE | O_RDWR);
      যদি (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      যদি (লিখ(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      কাঠামো stat st;
      যদি (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      যদি (st.আকার_2 != 1) {
        printf("grind: fstat reports wrong size %d\n", (পূর্ণসংখ্যা)st.আকার_2);
        exit(1);
      }
      যদি (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } অন্যথা যদি (what == 22) {
      // echo hi | cat
      পূর্ণসংখ্যা aa[2], bb[2];
      যদি (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      যদি (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      পূর্ণসংখ্যা pid1 = fork();
      যদি (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        যদি (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        অক্ষর *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } অন্যথা যদি (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      পূর্ণসংখ্যা pid2 = fork();
      যদি (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        যদি (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        যদি (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        অক্ষর *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } অন্যথা যদি (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      অক্ষর buf[4] = {0, 0, 0, 0};
      পঠ(bb[0], buf + 0, 1);
      পঠ(bb[0], buf + 1, 1);
      পঠ(bb[0], buf + 2, 1);
      close(bb[0]);
      পূর্ণসংখ্যা st1, st2;
      wait(&st1);
      wait(&st2);
      যদি (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

শূন্য
iter()
{
  unlink("a");
  unlink("b");

  পূর্ণসংখ্যা pid1 = fork();
  যদি (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  যদি (pid1 == 0) {
    rand_পরবর্তী ^= 31;
    go(0);
    exit(0);
  }

  পূর্ণসংখ্যা pid2 = fork();
  যদি (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  যদি (pid2 == 0) {
    rand_পরবর্তী ^= 7177;
    go(1);
    exit(0);
  }

  পূর্ণসংখ্যা st1 = -1;
  wait(&st1);
  যদি (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  পূর্ণসংখ্যা st2 = -1;
  wait(&st2);

  exit(0);
}

পূর্ণসংখ্যা
প্রধান()
{
  যখন (1) {
    পূর্ণসংখ্যা pid = fork();
    যদি (pid == 0) {
      iter();
      exit(0);
    }
    যদি (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_পরবর্তী += 1;
  }
}
