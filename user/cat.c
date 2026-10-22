#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(จำนวนเต็ม fd)
{
  จำนวนเต็ม n;

  ขณะ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ถ้า (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ถ้า (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม fd, i;

  ถ้า (argc <= 1) {
    cat(0);
    exit(0);
  }

  สำหรับ (i = 1; i < argc; i++) {
    ถ้า ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
