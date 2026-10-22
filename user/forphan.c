#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#وصف_ڪريو BUFSZ 500

حرف buf[BUFSZ];

صحيح
مکيه(صحيح argc, حرف **argv)
{
  صحيح fd = 0;
  حرف *s = argv[0];
  بنية stat st;
  حرف *ff = "file0";

  إذا ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    لکو("%s: open failed\n", s);
    ٻاهر_نڪرو(1);
  }
  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    ٻاهر_نڪرو(1);
  }
  إذا (unlink(ff) < 0) {
    لکو("%s: unlink failed\n", s);
    ٻاهر_نڪرو(1);
  }
  إذا (open(ff, O_RDONLY) != -1) {
    لکو("%s: open successed\n", s);
    ٻاهر_نڪرو(1);
  }
  لکو("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  لكل (;;)
    pause(1000);
}
