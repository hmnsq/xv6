#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(ຈຳນວນເຕັມ fd, var_na *name)
{
  ຈຳນວນເຕັມ i, n;
  ຈຳນວນເຕັມ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ໃນຂະນະທີ່ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ສຳລັບ (i = 0; i < n; i++) {
      c++;
      ຖ້າ (buf[i] == '\n')
        l++;
      ຖ້າ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ຖ້າ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ fd, i;

  ຖ້າ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  ສຳລັບ (i = 1; i < argc; i++) {
    ຖ້າ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
