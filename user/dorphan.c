#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

અક્ષર buf[BUFSZ];

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
  અક્ષર *s = argv[0];

  જો (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  જો (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  જો (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  માટે (;;)
    pause(1000);
}
