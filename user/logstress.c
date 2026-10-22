#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#تعریف_کړه BUFSZ 500

حرف buf[BUFSZ];

صحيح
اصلي(صحيح argc, حرف **argv)
{
  صحيح fd, n;
  تعداد { N = 250, SZ = 2000 };

  لكل (صحيح i = 1; i < argc; i++) {
    صحيح pid1 = fork();
    إذا (pid1 < 0) {
      چاپ_کړه("%s: fork failed\n", argv[0]);
      وتل(1);
    }
    إذا (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      إذا (fd < 0) {
        چاپ_کړه("%s: create %s failed\n", argv[0], argv[i]);
        وتل(1);
      }
      memset(buf, '0' + i, SZ);
      لكل (i = 0; i < N; i++) {
        إذا ((n = اكتب(fd, buf, SZ)) != SZ) {
          چاپ_کړه("write failed %d\n", n);
          وتل(1);
        }
      }
      وتل(0);
    }
  }
  صحيح xstatus;
  لكل (صحيح i = 1; i < argc; i++) {
    wait(&xstatus);
    إذا (xstatus != 0)
      وتل(xstatus);
  }
  أرجع 0;
}
