#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(ចំនួនគត់ fd)
{
  ចំនួនគត់ n;

  ខណៈពេល ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ប្រសិនបើ (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ប្រសិនបើ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ fd, i;

  ប្រសិនបើ (argc <= 1) {
    cat(0);
    exit(0);
  }

  សម្រាប់ (i = 1; i < argc; i++) {
    ប្រសិនបើ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
