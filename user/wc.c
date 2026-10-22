#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(ကိန်းပြည့် fd, var_na *name)
{
  ကိန်းပြည့် i, n;
  ကိန်းပြည့် l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  နေစဉ် ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    အတွက် (i = 0; i < n; i++) {
      c++;
      အကယ်၍ (buf[i] == '\n')
        l++;
      အကယ်၍ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      မဟုတ်ပါက အကယ်၍ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  အကယ်၍ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် fd, i;

  အကယ်၍ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  အတွက် (i = 1; i < argc; i++) {
    အကယ်၍ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
