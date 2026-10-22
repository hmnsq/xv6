#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(целое fd)
{
  целое n;

  пока ((n = read(fd, buf, sizeof(buf))) > 0) {
    если (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  если (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

целое
основной(целое argc, char *argv[])
{
  целое fd, i;

  если (argc <= 1) {
    cat(0);
    exit(0);
  }

  для (i = 1; i < argc; i++) {
    если ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
