#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

ଅକ୍ଷର buf[BUFSZ];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = 0;
  ଅକ୍ଷର *s = argv[0];
  ସଂରଚନା stat st;
  ଅକ୍ଷର *ff = "file0";

  ଯଦି ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  ଯଦି (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  ଯଦି (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  ଯଦି (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  ପାଇଁ (;;)
    pause(1000);
}
