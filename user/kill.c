#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
main(ཧྲིལ argc, var_na **argv)
{
  ཧྲིལ i;

  གལ་སྲིད (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  དོན་ལུ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
