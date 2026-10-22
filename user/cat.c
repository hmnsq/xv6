#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
cat(puur_naa_nka fd)
{
  puur_naa_nka n;

  ᯀᯬᯝ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    ᯀᯮᯝ (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ᯀᯮᯝ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka fd, i;

  ᯀᯮᯝ (argc <= 1) {
    cat(0);
    exit(0);
  }

  for (i = 1; i < argc; i++) {
    ᯀᯮᯝ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
