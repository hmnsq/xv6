#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#ವ್ಯಾಖ್ಯಾನಿಸು BUFSZ 500

ಅಕ್ಷರ buf[BUFSZ];

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
  ಅಕ್ಷರ *s = argv[0];

  ಆದರೆ (mkdir("dd") != 0) {
    ಬರೆ("%s: mkdir dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (chdir("dd") != 0) {
    ಬರೆ("%s: chdir dd failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ (unlink("../dd") < 0) {
    ಬರೆ("%s: unlink failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಬರೆ("wait for kill and reclaim\n");
  // sit around until killed
  ಗಾಗಿ (;;)
    pause(1000);
}
