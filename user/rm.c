#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం i;

  ఒకవేళ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    నిష్క్రమించు(1);
  }

  కొరకు (i = 1; i < argc; i++) {
    ఒకవేళ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      ఆపు;
    }
  }

  నిష్క్రమించు(0);
}
