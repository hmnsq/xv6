#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

整数
メインページ(整数 argc, char **argv)
{
  整数 fd, n;
  enum { N = 250, SZ = 2000 };

  ために (整数 i = 1; i < argc; i++) {
    整数 pid1 = fork();
    もし (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    もし (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      もし (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      ために (i = 0; i < N; i++) {
        もし ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  整数 xstatus;
  ために (整数 i = 1; i < argc; i++) {
    wait(&xstatus);
    もし (xstatus != 0)
      exit(xstatus);
  }
  戻る 0;
}
