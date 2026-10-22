#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

var_na buf[BUFSZ];

ចំនួនគត់
main(ចំនួនគត់ argc, var_na **argv)
{
  var_na *s = argv[0];

  ប្រសិនបើ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  ប្រសិនបើ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  ប្រសិនបើ (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  សម្រាប់ (;;)
    pause(1000);
}
