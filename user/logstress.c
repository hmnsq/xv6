#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na **argv)
{
  ຈຳນວນເຕັມ fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  ສຳລັບ (ຈຳນວນເຕັມ i = 1; i < argc; i++) {
    ຈຳນວນເຕັມ pid1 = fork();
    ຖ້າ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ຖ້າ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ຖ້າ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      ສຳລັບ (i = 0; i < N; i++) {
        ຖ້າ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ຈຳນວນເຕັມ xstatus;
  ສຳລັບ (ຈຳນວນເຕັມ i = 1; i < argc; i++) {
    wait(&xstatus);
    ຖ້າ (xstatus != 0)
      exit(xstatus);
  }
  ສົ່ງຄືນ 0;
}
