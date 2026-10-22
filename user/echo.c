#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
メインページ(整数 argc, char *argv[])
{
  整数 i;

  ために (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    もし (i + 1 < argc) {
      write(1, " ", 1);
    } 否则 {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
