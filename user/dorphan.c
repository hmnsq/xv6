#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

অক্ষর buf[BUFSZ];

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
  অক্ষর *s = argv[0];

  যদি (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  যদি (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  যদি (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  জন্য (;;)
    pause(1000);
}
