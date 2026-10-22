#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ i;

  ຖ້າ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  ສຳລັບ (i = 1; i < argc; i++) {
    ຖ້າ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
