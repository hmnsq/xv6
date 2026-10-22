#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#تعریف_کرو BUFSZ 500

حرف buf[BUFSZ];

صحيح
مکھ(صحيح argc, حرف **argv)
{
  صحيح fd = 0;
  حرف *s = argv[0];
  بنية stat st;
  حرف *ff = "file0";

  إذا ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    لکھو("%s: open failed\n", s);
    باہر_نکلو(1);
  }
  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    باہر_نکلو(1);
  }
  إذا (unlink(ff) < 0) {
    لکھو("%s: unlink failed\n", s);
    باہر_نکلو(1);
  }
  إذا (open(ff, O_RDONLY) != -1) {
    لکھو("%s: open successed\n", s);
    باہر_نکلو(1);
  }
  لکھو("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  لكل (;;)
    pause(1000);
}
