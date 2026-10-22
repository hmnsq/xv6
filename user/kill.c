#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na **argv)
{
  ຈຳນວນເຕັມ i;

  ຖ້າ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  ສຳລັບ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
