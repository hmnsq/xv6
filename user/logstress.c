#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

整数
主要(整数 argc, char **argv)
{
  整数 fd, n;
  enum { N = 250, SZ = 2000 };

  为 (整数 i = 1; i < argc; i++) {
    整数 pid1 = fork();
    如果 (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    如果 (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      如果 (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      为 (i = 0; i < N; i++) {
        如果 ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  整数 xstatus;
  为 (整数 i = 1; i < argc; i++) {
    wait(&xstatus);
    如果 (xstatus != 0)
      exit(xstatus);
  }
  返回 0;
}
