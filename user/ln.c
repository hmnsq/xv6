#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

số_nguyên
main(số_nguyên argc, char *argv[])
{
  nếu (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  nếu (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
