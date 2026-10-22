#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#وصف_ڪريو BUFSZ 500

حرف buf[BUFSZ];

صحيح
مکيه(صحيح argc, حرف **argv)
{
  صحيح fd, n;
  تعداد { N = 250, SZ = 2000 };

  لكل (صحيح i = 1; i < argc; i++) {
    صحيح pid1 = fork();
    إذا (pid1 < 0) {
      لکو("%s: fork failed\n", argv[0]);
      ٻاهر_نڪرو(1);
    }
    إذا (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      إذا (fd < 0) {
        لکو("%s: create %s failed\n", argv[0], argv[i]);
        ٻاهر_نڪرو(1);
      }
      memset(buf, '0' + i, SZ);
      لكل (i = 0; i < N; i++) {
        إذا ((n = اكتب(fd, buf, SZ)) != SZ) {
          لکو("write failed %d\n", n);
          ٻاهر_نڪرو(1);
        }
      }
      ٻاهر_نڪرو(0);
    }
  }
  صحيح xstatus;
  لكل (صحيح i = 1; i < argc; i++) {
    wait(&xstatus);
    إذا (xstatus != 0)
      ٻاهر_نڪرو(xstatus);
  }
  أرجع 0;
}
