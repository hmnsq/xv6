#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಅಕ್ಷರ buf[512];

ಶೂನ್ಯ
cat(ಪೂರ್ಣಾಂಕ fd)
{
  ಪೂರ್ಣಾಂಕ n;

  ಆಗ ((n = ಪಠ(fd, buf, ಗಾತ್ರ(buf))) > 0) {
    ಆದರೆ (ಲಿಖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      ನಿರ್ಗಮಿಸು(1);
    }
  }
  ಆದರೆ (n < 0) {
    fprintf(2, "cat: read error\n");
    ನಿರ್ಗಮಿಸು(1);
  }
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ fd, i;

  ಆದರೆ (argc <= 1) {
    cat(0);
    ನಿರ್ಗಮಿಸು(0);
  }

  ಗಾಗಿ (i = 1; i < argc; i++) {
    ಆದರೆ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      ನಿರ್ಗಮಿಸು(1);
    }
    cat(fd);
    close(fd);
  }
  ನಿರ್ಗಮಿಸು(0);
}
