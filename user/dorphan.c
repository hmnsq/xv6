#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#وصف_ڪريو BUFSZ 500

حرف buf[BUFSZ];

صحيح
مکيه(صحيح argc, حرف **argv)
{
  حرف *s = argv[0];

  إذا (mkdir("dd") != 0) {
    لکو("%s: mkdir dd failed\n", s);
    ٻاهر_نڪرو(1);
  }

  إذا (chdir("dd") != 0) {
    لکو("%s: chdir dd failed\n", s);
    ٻاهر_نڪرو(1);
  }

  إذا (unlink("../dd") < 0) {
    لکو("%s: unlink failed\n", s);
    ٻاهر_نڪرو(1);
  }
  لکو("wait for kill and reclaim\n");
  // sit around until killed
  لكل (;;)
    pause(1000);
}
