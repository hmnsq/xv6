#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

حرف buf[BUFSZ];

صحيح
main(صحيح argc, حرف **argv)
{
  صحيح fd, n;
  تعداد { N = 250, SZ = 2000 };

  עבור (صحيح i = 1; i < argc; i++) {
    صحيح pid1 = fork();
    אם (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    אם (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      אם (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      עבור (i = 0; i < N; i++) {
        אם ((n = اكتب(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  صحيح xstatus;
  עבור (صحيح i = 1; i < argc; i++) {
    wait(&xstatus);
    אם (xstatus != 0)
      exit(xstatus);
  }
  החזר 0;
}
