#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  නම් (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  නම් (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
