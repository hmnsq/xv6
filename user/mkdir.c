#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

цео
main(цео argc, char *argv[])
{
  цео i;

  ако (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  за (i = 1; i < argc; i++) {
    ако (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit(0);
}
