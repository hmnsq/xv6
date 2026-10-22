#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(整数 fd)
{
  整数 n;

  当 ((n = read(fd, buf, sizeof(buf))) > 0) {
    如果 (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  如果 (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

整数
主要(整数 argc, char *argv[])
{
  整数 fd, i;

  如果 (argc <= 1) {
    cat(0);
    exit(0);
  }

  为 (i = 1; i < argc; i++) {
    如果 ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
