#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

цяло
main(цяло argc, char **argv)
{
  цяло fd, n;
  enum { N = 250, SZ = 2000 };

  за (цяло i = 1; i < argc; i++) {
    цяло pid1 = fork();
    ако (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ако (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ако (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      за (i = 0; i < N; i++) {
        ако ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  цяло xstatus;
  за (цяло i = 1; i < argc; i++) {
    wait(&xstatus);
    ако (xstatus != 0)
      exit(xstatus);
  }
  върни 0;
}
