#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(จำนวนเต็ม fd, var_na *name)
{
  จำนวนเต็ม i, n;
  จำนวนเต็ม l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ขณะ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    สำหรับ (i = 0; i < n; i++) {
      c++;
      ถ้า (buf[i] == '\n')
        l++;
      ถ้า (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      มิฉะนั้น ถ้า (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ถ้า (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม fd, i;

  ถ้า (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  สำหรับ (i = 1; i < argc; i++) {
    ถ้า ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
