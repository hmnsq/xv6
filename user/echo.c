#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

цео
main(цео argc, char *argv[])
{
  цео i;

  за (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ако (i + 1 < argc) {
      write(1, " ", 1);
    } инако {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
