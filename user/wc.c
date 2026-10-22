#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(цяло fd, char *name)
{
  цяло i, n;
  цяло l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  докато ((n = read(fd, buf, sizeof(buf))) > 0) {
    за (i = 0; i < n; i++) {
      c++;
      ако (buf[i] == '\n')
        l++;
      ако (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      иначе ако (!inword) {
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

цяло
main(цяло argc, char *argv[])
{
  цяло fd, i;

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
