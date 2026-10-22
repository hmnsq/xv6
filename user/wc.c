#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(정수 fd, char *name)
{
  정수 i, n;
  정수 l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  동안 ((n = read(fd, buf, sizeof(buf))) > 0) {
    위해 (i = 0; i < n; i++) {
      c++;
      만약 (buf[i] == '\n')
        l++;
      만약 (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      아니면 만약 (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  만약 (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

정수
main(정수 argc, char *argv[])
{
  정수 fd, i;

  만약 (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  위해 (i = 1; i < argc; i++) {
    만약 ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
