#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[512];

void
wc(puur_naa_nka fd, var_na *name)
{
  puur_naa_nka i, n;
  puur_naa_nka l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ᮞᮨᮓᮨᮜ ((n = read(fd, buf, parimaa_na(buf))) > 0) {
    for (i = 0; i < n; i++) {
      c++;
      ᮚᮦᮔ᮪ (buf[i] == '\n')
        l++;
      ᮚᮦᮔ᮪ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ᮜᮤᮚ ᮚᮦᮔ᮪ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ᮚᮦᮔ᮪ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka fd, i;

  ᮚᮦᮔ᮪ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  for (i = 1; i < argc; i++) {
    ᮚᮦᮔ᮪ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
