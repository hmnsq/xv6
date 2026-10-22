#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(ціле fd, char *name)
{
  ціле i, n;
  ціле l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  поки ((n = read(fd, buf, sizeof(buf))) > 0) {
    для (i = 0; i < n; i++) {
      c++;
      якщо (buf[i] == '\n')
        l++;
      якщо (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      інакше якщо (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  якщо (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ціле
main(ціле argc, char *argv[])
{
  ціле fd, i;

  якщо (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  для (i = 1; i < argc; i++) {
    якщо ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
