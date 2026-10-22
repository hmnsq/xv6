//
// run random system calls in parallel forever.
//

#अन्तर्भाव "kernel/param.h"
#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "kernel/syscall.h"
#अन्तर्भाव "kernel/memlayout.h"
#अन्तर्भाव "kernel/riscv.h"

// from FreeBSD.
पूर्णाङ्क
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
  सचे (x < 0)
    x += 0x7fffffff;
  /* Transform to [0, 0x7ffffffd] range. */
  x--;
  *ctx = x;
  पटिनिवत्त (x);
}

unsigned long rand_next = 1;

पूर्णाङ्क
rand(शून्य)
{
  पटिनिवत्त (do_rand(&rand_next));
}

शून्य
go(पूर्णाङ्क which_child)
{
  पूर्णाङ्क fd = -1;
  स्थिर वर्ण buf[999];
  वर्ण *break0 = sbrk(0);
  uint64 iters = 0;

  mkdir("grindir");
  सचे (chdir("grindir") != 0) {
    printf("grind: chdir grindir failed\n");
    exit(1);
  }
  chdir("/");

  याव (1) {
    iters++;
    सचे ((iters % 500) == 0)
      लिख(1, which_child ? "B" : "A", 1);
    पूर्णाङ्क what = rand() % 23;
    सचे (what == 1) {
      close(open("grindir/../a", O_CREATE | O_RDWR));
    } अञ्ञथा सचे (what == 2) {
      close(open("grindir/../grindir/../b", O_CREATE | O_RDWR));
    } अञ्ञथा सचे (what == 3) {
      unlink("grindir/../a");
    } अञ्ञथा सचे (what == 4) {
      सचे (chdir("grindir") != 0) {
        printf("grind: chdir grindir failed\n");
        exit(1);
      }
      unlink("../b");
      chdir("/");
    } अञ्ञथा सचे (what == 5) {
      close(fd);
      fd = open("/grindir/../a", O_CREATE | O_RDWR);
    } अञ्ञथा सचे (what == 6) {
      close(fd);
      fd = open("/./grindir/./../b", O_CREATE | O_RDWR);
    } अञ्ञथा सचे (what == 7) {
      लिख(fd, buf, परिमाण(buf));
    } अञ्ञथा सचे (what == 8) {
      पठ(fd, buf, परिमाण(buf));
    } अञ्ञथा सचे (what == 9) {
      mkdir("grindir/../a");
      close(open("a/../a/./a", O_CREATE | O_RDWR));
      unlink("a/a");
    } अञ्ञथा सचे (what == 10) {
      mkdir("/../b");
      close(open("grindir/../b/b", O_CREATE | O_RDWR));
      unlink("b/b");
    } अञ्ञथा सचे (what == 11) {
      unlink("b");
      link("../grindir/./../a", "../b");
    } अञ्ञथा सचे (what == 12) {
      unlink("../grindir/../a");
      link(".././b", "/grindir/../a");
    } अञ्ञथा सचे (what == 13) {
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } अञ्ञथा सचे (what == 14) {
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        fork();
        fork();
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } अञ्ञथा सचे (what == 15) {
      sbrk(6011);
    } अञ्ञथा सचे (what == 16) {
      सचे (sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } अञ्ञथा सचे (what == 17) {
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        close(open("a", O_CREATE | O_RDWR));
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      सचे (chdir("../grindir/..") != 0) {
        printf("grind: chdir failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } अञ्ञथा सचे (what == 18) {
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        kill(getpid());
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } अञ्ञथा सचे (what == 19) {
      पूर्णाङ्क fds[2];
      सचे (pipe(fds) < 0) {
        printf("grind: pipe failed\n");
        exit(1);
      }
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        fork();
        fork();
        सचे (लिख(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        वर्ण c;
        सचे (पठ(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    } अञ्ञथा सचे (what == 20) {
      पूर्णाङ्क pid = fork();
      सचे (pid == 0) {
        unlink("a");
        mkdir("a");
        chdir("a");
        unlink("../a");
        fd = open("x", O_CREATE | O_RDWR);
        unlink("x");
        exit(0);
      } अञ्ञथा सचे (pid < 0) {
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } अञ्ञथा सचे (what == 21) {
      unlink("c");
      // should always succeed. check that there are free i-nodes,
      // file descriptors, blocks.
      पूर्णाङ्क fd1 = open("c", O_CREATE | O_RDWR);
      सचे (fd1 < 0) {
        printf("grind: create c failed\n");
        exit(1);
      }
      सचे (लिख(fd1, "x", 1) != 1) {
        printf("grind: write c failed\n");
        exit(1);
      }
      संरचना stat st;
      सचे (fstat(fd1, &st) != 0) {
        printf("grind: fstat failed\n");
        exit(1);
      }
      सचे (st.size != 1) {
        printf("grind: fstat reports wrong size %d\n", (पूर्णाङ्क)st.size);
        exit(1);
      }
      सचे (st.ino > 200) {
        printf("grind: fstat reports crazy i-number %d\n", st.ino);
        exit(1);
      }
      close(fd1);
      unlink("c");
    } अञ्ञथा सचे (what == 22) {
      // echo hi | cat
      पूर्णाङ्क aa[2], bb[2];
      सचे (pipe(aa) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      सचे (pipe(bb) < 0) {
        fprintf(2, "grind: pipe failed\n");
        exit(1);
      }
      पूर्णाङ्क pid1 = fork();
      सचे (pid1 == 0) {
        close(bb[0]);
        close(bb[1]);
        close(aa[0]);
        close(1);
        सचे (dup(aa[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(1);
        }
        close(aa[1]);
        वर्ण *args[3] = {"echo", "hi", 0};
        exec("grindir/../echo", args);
        fprintf(2, "grind: echo: not found\n");
        exit(2);
      } अञ्ञथा सचे (pid1 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(3);
      }
      पूर्णाङ्क pid2 = fork();
      सचे (pid2 == 0) {
        close(aa[1]);
        close(bb[0]);
        close(0);
        सचे (dup(aa[0]) != 0) {
          fprintf(2, "grind: dup failed\n");
          exit(4);
        }
        close(aa[0]);
        close(1);
        सचे (dup(bb[1]) != 1) {
          fprintf(2, "grind: dup failed\n");
          exit(5);
        }
        close(bb[1]);
        वर्ण *args[2] = {"cat", 0};
        exec("/cat", args);
        fprintf(2, "grind: cat: not found\n");
        exit(6);
      } अञ्ञथा सचे (pid2 < 0) {
        fprintf(2, "grind: fork failed\n");
        exit(7);
      }
      close(aa[0]);
      close(aa[1]);
      close(bb[1]);
      वर्ण buf[4] = {0, 0, 0, 0};
      पठ(bb[0], buf + 0, 1);
      पठ(bb[0], buf + 1, 1);
      पठ(bb[0], buf + 2, 1);
      close(bb[0]);
      पूर्णाङ्क st1, st2;
      wait(&st1);
      wait(&st2);
      सचे (st1 != 0 || st2 != 0 || strcmp(buf, "hi\n") != 0) {
        printf("grind: exec pipeline failed %d %d \"%s\"\n", st1, st2, buf);
        exit(1);
      }
    }
  }
}

शून्य
iter()
{
  unlink("a");
  unlink("b");

  पूर्णाङ्क pid1 = fork();
  सचे (pid1 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  सचे (pid1 == 0) {
    rand_next ^= 31;
    go(0);
    exit(0);
  }

  पूर्णाङ्क pid2 = fork();
  सचे (pid2 < 0) {
    printf("grind: fork failed\n");
    exit(1);
  }
  सचे (pid2 == 0) {
    rand_next ^= 7177;
    go(1);
    exit(0);
  }

  पूर्णाङ्क st1 = -1;
  wait(&st1);
  सचे (st1 != 0) {
    kill(pid1);
    kill(pid2);
  }
  पूर्णाङ्क st2 = -1;
  wait(&st2);

  exit(0);
}

पूर्णाङ्क
main()
{
  याव (1) {
    पूर्णाङ्क pid = fork();
    सचे (pid == 0) {
      iter();
      exit(0);
    }
    सचे (pid > 0) {
      wait(0);
    }
    pause(20);
    rand_next += 1;
  }
}
