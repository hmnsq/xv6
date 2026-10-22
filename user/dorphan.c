#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

ৱর্ণ buf[BUFSZ];

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
  ৱর্ণ *s = argv[0];

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
  বাবে (;;)
    pause(1000);
}
