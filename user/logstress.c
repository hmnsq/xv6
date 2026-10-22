#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

целое
основной(целое argc, char **argv)
{
  целое fd, n;
  enum { N = 250, SZ = 2000 };

  для (целое i = 1; i < argc; i++) {
    целое pid1 = fork();
    если (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    если (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      если (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      для (i = 0; i < N; i++) {
        если ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  целое xstatus;
  для (целое i = 1; i < argc; i++) {
    wait(&xstatus);
    если (xstatus != 0)
      exit(xstatus);
  }
  вернуть 0;
}
