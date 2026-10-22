#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#تعریف_کرو BUFSZ 500

حرف buf[BUFSZ];

صحیح
مین(صحیح argc, حرف **argv)
{
  صحیح fd = 0;
  حرف *s = argv[0];
  ساخت stat st;
  حرف *ff = "file0";

  اگر ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    لکھو("%s: open failed\n", s);
    باہر_نکلو(1);
  }
  اگر (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    باہر_نکلو(1);
  }
  اگر (unlink(ff) < 0) {
    لکھو("%s: unlink failed\n", s);
    باہر_نکلو(1);
  }
  اگر (open(ff, O_RDONLY) != -1) {
    لکھو("%s: open successed\n", s);
    باہر_نکلو(1);
  }
  لکھو("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  برائے (;;)
    pause(1000);
}
