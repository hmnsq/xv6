#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

char buf[BUFSZ];

整数
主要(整数 argc, char **argv)
{
  整数 fd = 0;
  char *s = argv[0];
  struct stat st;
  char *ff = "file0";

  如果 ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  如果 (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  如果 (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  如果 (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  为 (;;)
    pause(1000);
}
