#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#నిర్వచించు BUFSZ 500

అక్షరం buf[BUFSZ];

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
  అక్షరం *s = argv[0];

  ఒకవేళ (mkdir("dd") != 0) {
    రాయి("%s: mkdir dd failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (chdir("dd") != 0) {
    రాయి("%s: chdir dd failed\n", s);
    నిష్క్రమించు(1);
  }

  ఒకవేళ (unlink("../dd") < 0) {
    రాయి("%s: unlink failed\n", s);
    నిష్క్రమించు(1);
  }
  రాయి("wait for kill and reclaim\n");
  // sit around until killed
  కొరకు (;;)
    pause(1000);
}
