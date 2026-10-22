#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

цяло
main(цяло argc, char **argv)
{
  цяло i;

  ако (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  за (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
