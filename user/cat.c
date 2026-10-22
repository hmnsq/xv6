#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ವರ್ಣ buf[512];

ಶೂನ್ಯ
cat(ಪೂರ್ಣಾಙ್ಕ fd)
{
  ಪೂರ್ಣಾಙ್ಕ n;

  ಯಾವತ್ ((n = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf))) > 0) {
    ಯದಿ (ಲಿಖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ಯದಿ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ fd, i;

  ಯದಿ (argc <= 1) {
    cat(0);
    exit(0);
  }

  ಪ್ರತಿ (i = 1; i < argc; i++) {
    ಯದಿ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
