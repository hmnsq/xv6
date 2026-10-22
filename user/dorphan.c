#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

எழுத்து buf[BUFSZ];

முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
  எழுத்து *s = argv[0];

  எனில் (mkdir("dd") != 0) {
    printf("%s: mkdir dd failed\n", s);
    exit(1);
  }

  எனில் (chdir("dd") != 0) {
    printf("%s: chdir dd failed\n", s);
    exit(1);
  }

  எனில் (unlink("../dd") < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim\n");
  // sit around until killed
  க்கு (;;)
    pause(1000);
}
