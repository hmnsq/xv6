#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

целое
основной(целое argc, char *argv[])
{
  целое i;

  для (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    если (i + 1 < argc) {
      write(1, " ", 1);
    } иначе {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
