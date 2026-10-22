#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ціле
main(ціле argc, char *argv[])
{
  ціле i;

  якщо (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  для (i = 1; i < argc; i++) {
    якщо (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit(0);
}
