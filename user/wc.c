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
  כלעוד ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    עבור (i = 0; i < n; i++) {
      c++;
      אם (buf[i] == '\n')
        l++;
      אם (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      אחרת אם (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  אם (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  אם (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  עבור (i = 1; i < argc; i++) {
    אם ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
