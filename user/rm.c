#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
主要(整数 argc, char *argv[])
{
  整数 i;

  如果 (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  为 (i = 1; i < argc; i++) {
    如果 (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
