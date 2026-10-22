#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(ចំនួនគត់ fd, var_na *name)
{
  ចំនួនគត់ i, n;
  ចំនួនគត់ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ខណៈពេល ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    សម្រាប់ (i = 0; i < n; i++) {
      c++;
      ប្រសិនបើ (buf[i] == '\n')
        l++;
      ប្រសិនបើ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      បើមិនដូច្នេះ ប្រសិនបើ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ប្រសិនបើ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ fd, i;

  ប្រសិនបើ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  សម្រាប់ (i = 1; i < argc; i++) {
    ប្រសិនបើ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
