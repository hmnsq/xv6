#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na **argv)
{
  ཧྲིལ i;

  གལ་ཏེ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  ཆེད་དུ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
