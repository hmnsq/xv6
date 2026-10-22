#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(පූර්ණ_සංඛ්‍යා fd, var_na *name)
{
  පූර්ණ_සංඛ්‍යා i, n;
  පූර්ණ_සංඛ්‍යා l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  අතරතුර ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    සඳහා (i = 0; i < n; i++) {
      c++;
      නම් (buf[i] == '\n')
        l++;
      නම් (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      නැතහොත් නම් (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  නම් (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා fd, i;

  නම් (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  සඳහා (i = 1; i < argc; i++) {
    නම් ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
