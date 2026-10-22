#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(целое fd, char *name)
{
  целое i, n;
  целое l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  пока ((n = read(fd, buf, sizeof(buf))) > 0) {
    для (i = 0; i < n; i++) {
      c++;
      если (buf[i] == '\n')
        l++;
      если (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      иначе если (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  если (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

целое
основной(целое argc, char *argv[])
{
  целое fd, i;

  если (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  для (i = 1; i < argc; i++) {
    если ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
