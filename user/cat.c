#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(цяло fd)
{
  цяло n;

  докато ((n = read(fd, buf, sizeof(buf))) > 0) {
    ако (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ако (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

цяло
main(цяло argc, char *argv[])
{
  цяло fd, i;

  ако (argc <= 1) {
    cat(0);
    exit(0);
  }

  за (i = 1; i < argc; i++) {
    ако ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
