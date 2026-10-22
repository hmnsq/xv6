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
  当 ((n = read(fd, buf, sizeof(buf))) > 0) {
    为 (i = 0; i < n; i++) {
      c++;
      如果 (buf[i] == '\n')
        l++;
      如果 (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      否则 如果 (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  如果 (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

整数
主要(整数 argc, char *argv[])
{
  整数 fd, i;

  如果 (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  为 (i = 1; i < argc; i++) {
    如果 ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
