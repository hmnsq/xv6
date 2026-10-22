#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

char buf[BUFSZ];

số_nguyên
main(số_nguyên argc, char **argv)
{
  số_nguyên fd = 0;
  char *s = argv[0];
  struct stat st;
  char *ff = "file0";

  nếu ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  nếu (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  nếu (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  nếu (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  cho (;;)
    pause(1000);
}
