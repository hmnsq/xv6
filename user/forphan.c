#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

எழுத்து buf[BUFSZ];

முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
  முழு fd = 0;
  எழுத்து *s = argv[0];
  கட்டமைப்பு stat st;
  எழுத்து *ff = "file0";

  எனில் ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    printf("%s: open failed\n", s);
    exit(1);
  }
  எனில் (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    exit(1);
  }
  எனில் (unlink(ff) < 0) {
    printf("%s: unlink failed\n", s);
    exit(1);
  }
  எனில் (open(ff, O_RDONLY) != -1) {
    printf("%s: open successed\n", s);
    exit(1);
  }
  printf("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  க்கு (;;)
    pause(1000);
}
