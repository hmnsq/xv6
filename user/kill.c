#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
メインページ(整数 argc, char **argv)
{
  整数 i;

  もし (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  ために (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
