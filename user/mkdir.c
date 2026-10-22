#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం i;

  ఒకవేళ (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    నిష్క్రమించు(1);
  }

  కొరకు (i = 1; i < argc; i++) {
    ఒకవేళ (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      ఆపు;
    }
  }

  నిష్క్రమించు(0);
}
