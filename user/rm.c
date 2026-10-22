#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

цяло
main(цяло argc, char *argv[])
{
  цяло i;

  ако (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  за (i = 1; i < argc; i++) {
    ако (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
