#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSZ 500

ਅੱਖਰ buf[BUFSZ];

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
  ਅੱਖਰ *s = argv[0];

  ਜੇ (mkdir("dd") != 0) {
    ਲਿਖੋ("%s: mkdir dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (chdir("dd") != 0) {
    ਲਿਖੋ("%s: chdir dd failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ (unlink("../dd") < 0) {
    ਲਿਖੋ("%s: unlink failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖੋ("wait for kill and reclaim\n");
  // sit around until killed
  ਲਈ (;;)
    pause(1000);
}
