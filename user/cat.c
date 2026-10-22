#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(ကိန်းပြည့် fd)
{
  ကိန်းပြည့် n;

  နေစဉ် ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    အကယ်၍ (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  အကယ်၍ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် fd, i;

  အကယ်၍ (argc <= 1) {
    cat(0);
    exit(0);
  }

  အတွက် (i = 1; i < argc; i++) {
    အကယ်၍ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
