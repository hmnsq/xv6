#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#ವ್ಯಾಖ್ಯಾನಿಸು BUFSZ 500

ಅಕ್ಷರ buf[BUFSZ];

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
  ಪೂರ್ಣಾಂಕ fd, n;
  ಎಣಿಕೆ { N = 250, SZ = 2000 };

  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < argc; i++) {
    ಪೂರ್ಣಾಂಕ pid1 = fork();
    ಆದರೆ (pid1 < 0) {
      ಬರೆ("%s: fork failed\n", argv[0]);
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ಆದರೆ (fd < 0) {
        ಬರೆ("%s: create %s failed\n", argv[0], argv[i]);
        ನಿರ್ಗಮಿಸು(1);
      }
      memset(buf, '0' + i, SZ);
      ಗಾಗಿ (i = 0; i < N; i++) {
        ಆದರೆ ((n = ಲಿಖ(fd, buf, SZ)) != SZ) {
          ಬರೆ("write failed %d\n", n);
          ನಿರ್ಗಮಿಸು(1);
        }
      }
      ನಿರ್ಗಮಿಸು(0);
    }
  }
  ಪೂರ್ಣಾಂಕ xstatus;
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < argc; i++) {
    wait(&xstatus);
    ಆದರೆ (xstatus != 0)
      ನಿರ್ಗಮಿಸು(xstatus);
  }
  ಹಿಂತಿರುಗಿ 0;
}
