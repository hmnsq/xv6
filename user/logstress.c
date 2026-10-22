#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na **argv)
{
  ကိန်းပြည့် fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  အတွက် (ကိန်းပြည့် i = 1; i < argc; i++) {
    ကိန်းပြည့် pid1 = fork();
    အကယ်၍ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    အကယ်၍ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      အကယ်၍ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      အတွက် (i = 0; i < N; i++) {
        အကယ်၍ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ကိန်းပြည့် xstatus;
  အတွက် (ကိန်းပြည့် i = 1; i < argc; i++) {
    wait(&xstatus);
    အကယ်၍ (xstatus != 0)
      exit(xstatus);
  }
  ပြန်ပေး 0;
}
