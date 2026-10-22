#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#నిర్వచించు BUFSZ 500

అక్షరం buf[BUFSZ];

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
  పూర్ణాంకం fd = 0;
  అక్షరం *s = argv[0];
  నిర్మాణం stat st;
  అక్షరం *ff = "file0";

  ఒకవేళ ((fd = open(ff, O_CREATE | O_WRONLY)) < 0) {
    రాయి("%s: open failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (unlink(ff) < 0) {
    రాయి("%s: unlink failed\n", s);
    నిష్క్రమించు(1);
  }
  ఒకవేళ (open(ff, O_RDONLY) != -1) {
    రాయి("%s: open successed\n", s);
    నిష్క్రమించు(1);
  }
  రాయి("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  కొరకు (;;)
    pause(1000);
}
