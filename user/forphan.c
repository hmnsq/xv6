#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

حرف buf[BUFSZ];

صحيح
الرئيسية(صحيح argc, حرف **argv)
{
  صحيح fd = 0;
  حرف *s = argv[0];
  بنية stat st;
  حرف *ff = "file0";

  إذا ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  إذا (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  إذا (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  لكل (;;)
    pause(1000);
}
