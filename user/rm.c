#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ціле
main(ціле argc, char *argv[])
{
  ціле i;

  якщо (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  для (i = 1; i < argc; i++) {
    якщо (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
