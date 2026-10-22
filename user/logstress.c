#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

var_na buf[BUFSZ];

puur_naa_nka
main(puur_naa_nka argc, var_na **argv)
{
  puur_naa_nka fd, n;
  ga_nanaa { N = 250, SZ = 2000 };

  for (puur_naa_nka i = 1; i < argc; i++) {
    puur_naa_nka pid1 = fork();
    ᱡᱩᱫᱤ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ᱡᱩᱫᱤ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ᱡᱩᱫᱤ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      for (i = 0; i < N; i++) {
        ᱡᱩᱫᱤ ((n = write(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  puur_naa_nka xstatus;
  for (puur_naa_nka i = 1; i < argc; i++) {
    wait(&xstatus);
    ᱡᱩᱫᱤ (xstatus != 0)
      exit(xstatus);
  }
  ᱨᱩᱣᱟᱹᱲ 0;
}
