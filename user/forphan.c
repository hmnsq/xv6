#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क fd = 0;
  वर्ण *s = argv[0];
  संरचना stat st;
  वर्ण *ff = "file0";

  सचे ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  सचे (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  सचे (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  सचे (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  अत्थाय (;;)
    pause(1000);
}
