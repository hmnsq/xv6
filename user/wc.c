#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

نویسه buf[512];

تهی
wc(صحیح fd, نویسه *name)
{
  صحیح i, n;
  صحیح l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  وقتی ((n = اقرأ(fd, buf, اندازه(buf))) > 0) {
    برای (i = 0; i < n; i++) {
      c++;
      اگر (buf[i] == '\n')
        l++;
      اگر (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      وگرنه اگر (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  اگر (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح fd, i;

  اگر (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  برای (i = 1; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
