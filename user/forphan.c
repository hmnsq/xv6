#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

ৱর্ণ buf[BUFSZ];

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
  পূর্ণাঙ্ক fd = 0;
  ৱর্ণ *s = argv[0];
  সংরচনা stat st;
  ৱর্ণ *ff = "file0";

  যদি ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  যদি (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  যদি (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  যদি (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  বাবে (;;)
    pause(1000);
}
