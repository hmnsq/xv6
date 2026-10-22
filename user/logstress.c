#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

ཧྲིལ
main(ཧྲིལ argc, var_na **argv)
{
  ཧྲིལ fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  དོན་ལུ (ཧྲིལ i = 1; i < argc; i++) {
    ཧྲིལ pid1 = fork();
    གལ་སྲིད (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    གལ་སྲིད (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      གལ་སྲིད (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      དོན་ལུ (i = 0; i < N; i++) {
        གལ་སྲིད ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ཧྲིལ xstatus;
  དོན་ལུ (ཧྲིལ i = 1; i < argc; i++) {
    wait(&xstatus);
    གལ་སྲིད (xstatus != 0)
      exit(xstatus);
  }
  ལོག 0;
}
