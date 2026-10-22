#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ціле
main(ціле argc, char *argv[])
{
  ціле i;

  для (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    якщо (i + 1 < argc) {
      write(1, " ", 1);
    } інакше {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
