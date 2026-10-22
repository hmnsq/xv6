#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  ఒకవేళ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    నిష్క్రమించు(1);
  }
  ఒకవేళ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  నిష్క్రమించు(0);
}
