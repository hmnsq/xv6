#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
  పూర్ణాంకం i;

  ఒకవేళ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    నిష్క్రమించు(1);
  }
  కొరకు (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  నిష్క్రమించు(0);
}
