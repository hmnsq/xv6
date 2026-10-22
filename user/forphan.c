#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

বর্ণ buf[BUFSZ];

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
  পূর্ণাঙ্ক fd = 0;
  বর্ণ *s = argv[0];
  সংরচনা stat st;
  বর্ণ *ff = "file0";

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
  প্রতি (;;)
    pause(1000);
}
