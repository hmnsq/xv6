#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ i;

  ສຳລັບ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ຖ້າ (i + 1 < argc) {
      write(1, " ", 1);
    } ບໍ່ດັ່ງນັ້ນ {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
