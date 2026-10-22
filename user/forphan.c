#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

അക്ഷരം buf[BUFSZ];

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
  പൂർണ്ണസംഖ്യ fd = 0;
  അക്ഷരം *s = argv[0];
  ഘടന stat st;
  അക്ഷരം *ff = "file0";

  എങ്കിൽ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  എങ്കിൽ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  എങ്കിൽ (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  എങ്കിൽ (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  വേണ്ടി (;;)
    pause(1000);
}
