#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSZ 500

ਅੱਖਰ buf[BUFSZ];

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
  ਪੂਰਨ fd = 0;
  ਅੱਖਰ *s = argv[0];
  ਬਣਤਰ stat st;
  ਅੱਖਰ *ff = "file0";

  ਜੇ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    ਲਿਖੋ("%s: open failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (unlink(ff) < 0) {
    ਲਿਖੋ("%s: unlink failed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (open(ff, O_RDONLY) != -1) {
    ਲਿਖੋ("%s: open successed\n", s);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖੋ("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  ਲਈ (;;)
    pause(1000);
}
