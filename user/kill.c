#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
主要(整数 argc, char **argv)
{
  整数 i;

  如果 (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  为 (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
