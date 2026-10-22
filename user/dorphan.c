#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#تعریف_کن BUFSZ 500

حرف buf[BUFSZ];

صحيح
اصلی(صحيح argc, حرف **argv)
{
  حرف *s = argv[0];

  إذا (mkdir("dd") != 0) {
    چاپ_کن("%s: mkdir dd failed\n", s);
    خروج(1);
  }

  إذا (chdir("dd") != 0) {
    چاپ_کن("%s: chdir dd failed\n", s);
    خروج(1);
  }

  إذا (unlink("../dd") < 0) {
    چاپ_کن("%s: unlink failed\n", s);
    خروج(1);
  }
  چاپ_کن("wait for kill and reclaim\n");
  // sit around until killed
  لكل (;;)
    pause(1000);
}
