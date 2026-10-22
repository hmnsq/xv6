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
  طالما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
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
    چاپ_کن("wc: read error\n");
    خروج(1);
  }
  چاپ_کن("%d %d %d %s\n", l, w, c, name);
}

صحيح
اصلی(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  إذا (argc <= 1) {
    wc(0, "");
    خروج(0);
  }

  لكل (i = 1; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
      چاپ_کن("wc: cannot open %s\n", argv[i]);
      خروج(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  خروج(0);
}
