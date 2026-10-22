#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(整数 fd, char *name)
{
  整数 i, n;
  整数 l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ながら ((n = read(fd, buf, sizeof(buf))) > 0) {
    ために (i = 0; i < n; i++) {
      c++;
      もし (buf[i] == '\n')
        l++;
      もし (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      否则 もし (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  もし (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

整数
メインページ(整数 argc, char *argv[])
{
  整数 fd, i;

  もし (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  ために (i = 1; i < argc; i++) {
    もし ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
