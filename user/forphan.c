#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

var_na buf[BUFSZ];

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na **argv)
{
  จำนวนเต็ม fd = 0;
  var_na *s = argv[0];
  sa_nracanaa stat st;
  var_na *ff = "file0";

  ถ้า ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ถ้า (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  ถ้า (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  ถ้า (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  สำหรับ (;;)
    pause(1000);
}
