#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

정수
main(정수 argc, char **argv)
{
  정수 fd, n;
  enum { N = 250, SZ = 2000 };

  위해 (정수 i = 1; i < argc; i++) {
    정수 pid1 = fork();
    만약 (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    만약 (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      만약 (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      위해 (i = 0; i < N; i++) {
        만약 ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  정수 xstatus;
  위해 (정수 i = 1; i < argc; i++) {
    wait(&xstatus);
    만약 (xstatus != 0)
      exit(xstatus);
  }
  반환 0;
}
