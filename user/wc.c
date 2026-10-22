#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(цео fd, char *name)
{
  цео i, n;
  цео l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  док ((n = read(fd, buf, sizeof(buf))) > 0) {
    за (i = 0; i < n; i++) {
      c++;
      ако (buf[i] == '\n')
        l++;
      ако (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      инако ако (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ако (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

цео
main(цео argc, char *argv[])
{
  цео fd, i;

  ако (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  за (i = 1; i < argc; i++) {
    ако ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
