#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

ចំនួនគត់
main(ចំនួនគត់ argc, var_na **argv)
{
  ចំនួនគត់ fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  សម្រាប់ (ចំនួនគត់ i = 1; i < argc; i++) {
    ចំនួនគត់ pid1 = fork();
    ប្រសិនបើ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ប្រសិនបើ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ប្រសិនបើ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      សម្រាប់ (i = 0; i < N; i++) {
        ប្រសិនបើ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ចំនួនគត់ xstatus;
  សម្រាប់ (ចំនួនគត់ i = 1; i < argc; i++) {
    wait(&xstatus);
    ប្រសិនបើ (xstatus != 0)
      exit(xstatus);
  }
  ត្រឡប់ 0;
}
