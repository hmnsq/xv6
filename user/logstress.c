#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na **argv)
{
  จำนวนเต็ม fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  สำหรับ (จำนวนเต็ม i = 1; i < argc; i++) {
    จำนวนเต็ม pid1 = fork();
    ถ้า (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ถ้า (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ถ้า (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      สำหรับ (i = 0; i < N; i++) {
        ถ้า ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  จำนวนเต็ม xstatus;
  สำหรับ (จำนวนเต็ม i = 1; i < argc; i++) {
    wait(&xstatus);
    ถ้า (xstatus != 0)
      exit(xstatus);
  }
  คืน 0;
}
