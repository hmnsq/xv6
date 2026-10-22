#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

var_na buf[BUFSZ];

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na **argv)
{
  var_na *s = argv[0];

  ຖ້າ (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  ຖ້າ (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  ຖ້າ (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  ສຳລັບ (;;)
    pause(1000);
}
