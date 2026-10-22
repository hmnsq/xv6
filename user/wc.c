#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

حرف buf[512];

فراغ
wc(صحيح fd, حرف *name)
{
  صحيح i, n;
  صحيح l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  بينما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    لكل (i = 0; i < n; i++) {
      c++;
      إذا (buf[i] == '\n')
        l++;
      إذا (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      وإلا إذا (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  إذا (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

صحيح
الرئيسية(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  إذا (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  لكل (i = 1; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
