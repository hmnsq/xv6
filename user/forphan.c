#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#ವ್ಯಾಖ್ಯಾನಿಸು BUFSZ 500

ಅಕ್ಷರ buf[BUFSZ];

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
  ಪೂರ್ಣಾಂಕ fd = 0;
  ಅಕ್ಷರ *s = argv[0];
  ರಚನೆ stat st;
  ಅಕ್ಷರ *ff = "file0";

  ಆದರೆ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    ಬರೆ("%s: open failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (unlink(ff) < 0) {
    ಬರೆ("%s: unlink failed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (open(ff, O_RDONLY) != -1) {
    ಬರೆ("%s: open successed\n", s);
    ನಿರ್ಗಮಿಸು(1);
  }
  ಬರೆ("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  ಗಾಗಿ (;;)
    pause(1000);
}
