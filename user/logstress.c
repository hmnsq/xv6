#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

char buf[BUFSZ];

số_nguyên
main(số_nguyên argc, char **argv)
{
  số_nguyên fd, n;
  enum { N = 250, SZ = 2000 };

  cho (số_nguyên i = 1; i < argc; i++) {
    số_nguyên pid1 = fork();
    nếu (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    nếu (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      nếu (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      cho (i = 0; i < N; i++) {
        nếu ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  số_nguyên xstatus;
  cho (số_nguyên i = 1; i < argc; i++) {
    wait(&xstatus);
    nếu (xstatus != 0)
      exit(xstatus);
  }
  trả_về 0;
}
