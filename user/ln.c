#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(정수 argc, char *argv[])
{
  만약 (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  만약 (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
