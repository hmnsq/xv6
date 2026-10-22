#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

ವರ್ಣ buf[BUFSZ];

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
  ಪೂರ್ಣಾಙ್ಕ fd, n;
  ಗಣನಾ { N = 250, SZ = 2000 };

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < argc; i++) {
    ಪೂರ್ಣಾಙ್ಕ pid1 = fork();
    ಯದಿ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ಯದಿ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ಯದಿ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      ಪ್ರತಿ (i = 0; i < N; i++) {
        ಯದಿ ((n = ಲಿಖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ಪೂರ್ಣಾಙ್ಕ xstatus;
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < argc; i++) {
    wait(&xstatus);
    ಯದಿ (xstatus != 0)
      exit(xstatus);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
