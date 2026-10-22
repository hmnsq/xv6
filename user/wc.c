#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

حرف buf[512];

خالی
wc(صحیح fd, حرف *name)
{
  صحیح i, n;
  صحیح l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  جب ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    برائے (i = 0; i < n; i++) {
      c++;
      اگر (buf[i] == '\n')
        l++;
      اگر (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ورنہ اگر (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  اگر (n < 0) {
    لکھو("wc: read error\n");
    باہر_نکلو(1);
  }
  لکھو("%d %d %d %s\n", l, w, c, name);
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح fd, i;

  اگر (argc <= 1) {
    wc(0, "");
    باہر_نکلو(0);
  }

  برائے (i = 1; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
      لکھو("wc: cannot open %s\n", argv[i]);
      باہر_نکلو(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  باہر_نکلو(0);
}
