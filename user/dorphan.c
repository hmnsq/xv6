#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#تعریف_کرو BUFSZ 500

حرف buf[BUFSZ];

صحیح
مین(صحیح argc, حرف **argv)
{
  حرف *s = argv[0];

  اگر (mkdir("dd") != 0) {
    لکھو("%s: mkdir dd failed\n", s);
    باہر_نکلو(1);
  }

  اگر (chdir("dd") != 0) {
    لکھو("%s: chdir dd failed\n", s);
    باہر_نکلو(1);
  }

  اگر (unlink("../dd") < 0) {
    لکھو("%s: unlink failed\n", s);
    باہر_نکلو(1);
  }
  لکھو("wait for kill and reclaim\n");
  // sit around until killed
  برائے (;;)
    pause(1000);
}
