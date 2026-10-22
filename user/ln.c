#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  གལ་ཏེ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  གལ་ཏེ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
