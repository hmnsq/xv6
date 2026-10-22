#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಅಕ್ಷರ buf[512];

ಶೂನ್ಯ
wc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ *name)
{
  ಪೂರ್ಣಾಂಕ i, n;
  ಪೂರ್ಣಾಂಕ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ಆಗ ((n = ಪಠ(fd, buf, ಗಾತ್ರ(buf))) > 0) {
    ಗಾಗಿ (i = 0; i < n; i++) {
      c++;
      ಆದರೆ (buf[i] == '\n')
        l++;
      ಆದರೆ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ಆದರೆ (n < 0) {
    ಬರೆ("wc: read error\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಬರೆ("%d %d %d %s\n", l, w, c, name);
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ fd, i;

  ಆದರೆ (argc <= 1) {
    wc(0, "");
    ನಿರ್ಗಮಿಸು(0);
  }

  ಗಾಗಿ (i = 1; i < argc; i++) {
    ಆದರೆ ((fd = open(argv[i], O_RDONLY)) < 0) {
      ಬರೆ("wc: cannot open %s\n", argv[i]);
      ನಿರ್ಗಮಿಸು(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  ನಿರ್ಗಮಿಸು(0);
}
