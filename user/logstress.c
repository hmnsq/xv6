#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#تعریف_کرو BUFSZ 500

حرف buf[BUFSZ];

صحیح
مین(صحیح argc, حرف **argv)
{
  صحیح fd, n;
  شمار { N = 250, SZ = 2000 };

  برائے (صحیح i = 1; i < argc; i++) {
    صحیح pid1 = fork();
    اگر (pid1 < 0) {
      لکھو("%s: fork failed\n", argv[0]);
      باہر_نکلو(1);
    }
    اگر (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      اگر (fd < 0) {
        لکھو("%s: create %s failed\n", argv[0], argv[i]);
        باہر_نکلو(1);
      }
      memset(buf, '0' + i, SZ);
      برائے (i = 0; i < N; i++) {
        اگر ((n = اكتب(fd, buf, SZ)) != SZ) {
          لکھو("write failed %d\n", n);
          باہر_نکلو(1);
        }
      }
      باہر_نکلو(0);
    }
  }
  صحیح xstatus;
  برائے (صحیح i = 1; i < argc; i++) {
    wait(&xstatus);
    اگر (xstatus != 0)
      باہر_نکلو(xstatus);
  }
  واپس 0;
}
