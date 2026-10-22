#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na **argv)
{
  පූර්ණ_සංඛ්‍යා fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < argc; i++) {
    පූර්ණ_සංඛ්‍යා pid1 = fork();
    නම් (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    නම් (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      නම් (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      සඳහා (i = 0; i < N; i++) {
        නම් ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  පූර්ණ_සංඛ්‍යා xstatus;
  සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < argc; i++) {
    wait(&xstatus);
    නම් (xstatus != 0)
      exit(xstatus);
  }
  ආපසු 0;
}
