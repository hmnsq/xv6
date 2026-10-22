#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ವರ್ಣ buf[512];

ಶೂನ್ಯ
wc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ *name)
{
  ಪೂರ್ಣಾಙ್ಕ i, n;
  ಪೂರ್ಣಾಙ್ಕ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ಯಾವತ್ ((n = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf))) > 0) {
    ಪ್ರತಿ (i = 0; i < n; i++) {
      c++;
      ಯದಿ (buf[i] == '\n')
        l++;
      ಯದಿ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ಅನ್ಯಥಾ ಯದಿ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ಯದಿ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ fd, i;

  ಯದಿ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  ಪ್ರತಿ (i = 1; i < argc; i++) {
    ಯದಿ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
