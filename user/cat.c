#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(ཧྲིལ fd)
{
  ཧྲིལ n;

  སྐབས ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    གལ་སྲིད (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  གལ་སྲིད (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ཧྲིལ
main(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ fd, i;

  གལ་སྲིད (argc <= 1) {
    cat(0);
    exit(0);
  }

  དོན་ལུ (i = 1; i < argc; i++) {
    གལ་སྲིད ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
