#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#تعریف_کړه BUFSZ 500

حرف buf[BUFSZ];

صحيح
اصلي(صحيح argc, حرف **argv)
{
  حرف *s = argv[0];

  إذا (mkdir("dd") != 0) {
    چاپ_کړه("%s: mkdir dd failed\n", s);
    وتل(1);
  }

  إذا (chdir("dd") != 0) {
    چاپ_کړه("%s: chdir dd failed\n", s);
    وتل(1);
  }

  إذا (unlink("../dd") < 0) {
    چاپ_کړه("%s: unlink failed\n", s);
    وتل(1);
  }
  چاپ_کړه("wait for kill and reclaim\n");
  // sit around until killed
  لكل (;;)
    pause(1000);
}
