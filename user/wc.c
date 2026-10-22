#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(số_nguyên fd, char *name)
{
  số_nguyên i, n;
  số_nguyên l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  trong_khi ((n = read(fd, buf, sizeof(buf))) > 0) {
    cho (i = 0; i < n; i++) {
      c++;
      nếu (buf[i] == '\n')
        l++;
      nếu (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      không_thì nếu (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  nếu (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên fd, i;

  nếu (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  cho (i = 1; i < argc; i++) {
    nếu ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
