#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(số_nguyên fd)
{
  số_nguyên n;

  trong_khi ((n = read(fd, buf, sizeof(buf))) > 0) {
    nếu (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  nếu (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên fd, i;

  nếu (argc <= 1) {
    cat(0);
    exit(0);
  }

  cho (i = 1; i < argc; i++) {
    nếu ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
