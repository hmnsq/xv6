#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

અક્ષર buf[BUFSZ];

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
  પૂર્ણાંક fd = 0;
  અક્ષર *s = argv[0];
  સંરચના stat st;
  અક્ષર *ff = "file0";

  જો ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  જો (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  જો (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  જો (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  માટે (;;)
    pause(1000);
}
