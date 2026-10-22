#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(整数 fd)
{
  整数 n;

  ながら ((n = read(fd, buf, sizeof(buf))) > 0) {
    もし (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  もし (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

整数
メインページ(整数 argc, char *argv[])
{
  整数 fd, i;

  もし (argc <= 1) {
    cat(0);
    exit(0);
  }

  ために (i = 1; i < argc; i++) {
    もし ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
