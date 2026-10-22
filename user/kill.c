#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ціле
main(ціле argc, char **argv)
{
  ціле i;

  якщо (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  для (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
