#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

ціле
main(ціле argc, char **argv)
{
  ціле fd, n;
  enum { N = 250, SZ = 2000 };

  для (ціле i = 1; i < argc; i++) {
    ціле pid1 = fork();
    якщо (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    якщо (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      якщо (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      для (i = 0; i < N; i++) {
        якщо ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ціле xstatus;
  для (ціле i = 1; i < argc; i++) {
    wait(&xstatus);
    якщо (xstatus != 0)
      exit(xstatus);
  }
  повернути 0;
}
