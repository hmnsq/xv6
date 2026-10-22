#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSZ 500

ਅੱਖਰ buf[BUFSZ];

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
  ਪੂਰਨ fd, n;
  ਗਿਣਤੀ { N = 250, SZ = 2000 };

  ਲਈ (ਪੂਰਨ i = 1; i < argc; i++) {
    ਪੂਰਨ pid1 = fork();
    ਜੇ (pid1 < 0) {
      ਲਿਖੋ("%s: fork failed\n", argv[0]);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ਜੇ (fd < 0) {
        ਲਿਖੋ("%s: create %s failed\n", argv[0], argv[i]);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      }
      memset(buf, '0' + i, SZ);
      ਲਈ (i = 0; i < N; i++) {
        ਜੇ ((n = ਲਿਖ(fd, buf, SZ)) != SZ) {
          ਲਿਖੋ("write failed %d\n", n);
          ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
      }
      ਬਾਹਰ_ਨਿਕਲੋ(0);
    }
  }
  ਪੂਰਨ xstatus;
  ਲਈ (ਪੂਰਨ i = 1; i < argc; i++) {
    wait(&xstatus);
    ਜੇ (xstatus != 0)
      ਬਾਹਰ_ਨਿਕਲੋ(xstatus);
  }
  ਵਾਪਸ 0;
}
