#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(පූර්ණ_සංඛ්‍යා fd)
{
  පූර්ණ_සංඛ්‍යා n;

  අතරතුර ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    නම් (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  නම් (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා fd, i;

  නම් (argc <= 1) {
    cat(0);
    exit(0);
  }

  සඳහා (i = 1; i < argc; i++) {
    නම් ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
