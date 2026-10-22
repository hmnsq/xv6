#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#تعریف_کرو BUFSZ 500

حرف buf[BUFSZ];

صحيح
مکھ(صحيح argc, حرف **argv)
{
  حرف *s = argv[0];

  إذا (mkdir("dd") != 0) {
    لکھو("%s: mkdir dd failed\n", s);
    باہر_نکلو(1);
  }

  إذا (chdir("dd") != 0) {
    لکھو("%s: chdir dd failed\n", s);
    باہر_نکلو(1);
  }

  إذا (unlink("../dd") < 0) {
    لکھو("%s: unlink failed\n", s);
    باہر_نکلو(1);
  }
  لکھو("wait for kill and reclaim\n");
  // sit around until killed
  لكل (;;)
    pause(1000);
}
