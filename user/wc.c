#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(ཧྲིལ fd, var_na *name)
{
  ཧྲིལ i, n;
  ཧྲིལ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  སྐབས ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ཆེད་དུ (i = 0; i < n; i++) {
      c++;
      གལ་ཏེ (buf[i] == '\n')
        l++;
      གལ་ཏེ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      དེ་མིན གལ་ཏེ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  གལ་ཏེ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ fd, i;

  གལ་ཏེ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  ཆེད་དུ (i = 1; i < argc; i++) {
    གལ་ཏེ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
