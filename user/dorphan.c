#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

char buf[BUFSZ];

цяло
main(цяло argc, char **argv)
{
  char *s = argv[0];

  ако (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  ако (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  ако (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  за (;;)
    pause(1000);
}
