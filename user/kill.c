#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(정수 argc, char **argv)
{
  정수 i;

  만약 (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  위해 (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
