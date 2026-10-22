#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

نویسه buf[BUFSZ];

صحیح
اصلی(صحیح argc, نویسه **argv)
{
  صحیح fd, n;
  شمارش { N = 250, SZ = 2000 };

  برای (صحیح i = 1; i < argc; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    اگر (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      اگر (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      برای (i = 0; i < N; i++) {
        اگر ((n = اكتب(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  صحیح xstatus;
  برای (صحیح i = 1; i < argc; i++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      exit(xstatus);
  }
  بازگشت 0;
}
