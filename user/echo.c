#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා i;

  සඳහා (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    නම් (i + 1 < argc) {
      write(1, " ", 1);
    } නැතහොත් {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
