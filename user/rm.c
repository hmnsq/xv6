#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

целое
основной(целое argc, char *argv[])
{
  целое i;

  если (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  для (i = 1; i < argc; i++) {
    если (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
