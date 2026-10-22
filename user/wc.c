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
    لکو("wc: read error\n");
    ٻاهر_نڪرو(1);
  }
  لکو("%d %d %d %s\n", l, w, c, name);
}

صحيح
مکيه(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  إذا (argc <= 1) {
    wc(0, "");
    ٻاهر_نڪرو(0);
  }

  لكل (i = 1; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
      لکو("wc: cannot open %s\n", argv[i]);
      ٻاهر_نڪرو(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  ٻاهر_نڪرو(0);
}
