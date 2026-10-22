#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා i;

  නම් (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  සඳහා (i = 1; i < argc; i++) {
    නම් (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
