#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

var_na buf[BUFSZ];

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na **argv)
{
  ကိန်းပြည့် fd = 0;
  var_na *s = argv[0];
  sa_nracanaa stat st;
  var_na *ff = "file0";

  အကယ်၍ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  အကယ်၍ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  အကယ်၍ (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  အကယ်၍ (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  အတွက် (;;)
    pause(1000);
}
