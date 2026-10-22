#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(정수 fd)
{
  정수 n;

  동안 ((n = read(fd, buf, sizeof(buf))) > 0) {
    만약 (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  만약 (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

정수
main(정수 argc, char *argv[])
{
  정수 fd, i;

  만약 (argc <= 1) {
    cat(0);
    exit(0);
  }

  위해 (i = 1; i < argc; i++) {
    만약 ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
