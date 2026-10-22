#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

ವರ್ಣ buf[BUFSZ];

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
  ಪೂರ್ಣಾಙ್ಕ fd = 0;
  ವರ್ಣ *s = argv[0];
  ಸಂರಚನಾ stat st;
  ವರ್ಣ *ff = "file0";

  ಯದಿ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ಯದಿ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  ಯದಿ (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  ಯದಿ (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  ಪ್ರತಿ (;;)
    pause(1000);
}
