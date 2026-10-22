#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

цяло
main(цяло argc, char *argv[])
{
  цяло i;

  за (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ако (i + 1 < argc) {
      write(1, " ", 1);
    } иначе {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
