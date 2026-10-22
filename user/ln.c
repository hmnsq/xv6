#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
主要(整数 argc, char *argv[])
{
  如果 (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  如果 (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
