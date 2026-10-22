#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

số_nguyên
main(số_nguyên argc, char **argv)
{
  số_nguyên i;

  nếu (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  cho (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
