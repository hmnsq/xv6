#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

எழுத்து buf[BUFSZ];

முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
  முழு fd, n;
  எண்ணிக்கை { N = 250, SZ = 2000 };

  க்கு (முழு i = 1; i < argc; i++) {
    முழு pid1 = fork();
    எனில் (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    எனில் (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      எனில் (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      க்கு (i = 0; i < N; i++) {
        எனில் ((n = லிக(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  முழு xstatus;
  க்கு (முழு i = 1; i < argc; i++) {
    wait(&xstatus);
    எனில் (xstatus != 0)
      exit(xstatus);
  }
  திரும்பு 0;
}
