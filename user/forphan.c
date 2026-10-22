#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

نویسه buf[BUFSZ];

صحیح
اصلی(صحیح argc, نویسه **argv)
{
  صحیح fd = 0;
  نویسه *s = argv[0];
  ساختار stat st;
  نویسه *ff = "file0";

  اگر ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  اگر (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  اگر (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  اگر (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  برای (;;)
    pause(1000);
}
