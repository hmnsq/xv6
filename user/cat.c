#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(ຈຳນວນເຕັມ fd)
{
  ຈຳນວນເຕັມ n;

  ໃນຂະນະທີ່ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ຖ້າ (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ຖ້າ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ fd, i;

  ຖ້າ (argc <= 1) {
    cat(0);
    exit(0);
  }

  ສຳລັບ (i = 1; i < argc; i++) {
    ຖ້າ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
