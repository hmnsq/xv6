#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຖ້າ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  ຖ້າ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
