#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਅੱਖਰ buf[512];

ਖਾਲੀ
wc(ਪੂਰਨ fd, ਅੱਖਰ *name)
{
  ਪੂਰਨ i, n;
  ਪੂਰਨ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ਜਦੋਂ ((n = ਪਠ(fd, buf, ਆਕਾਰ(buf))) > 0) {
    ਲਈ (i = 0; i < n; i++) {
      c++;
      ਜੇ (buf[i] == '\n')
        l++;
      ਜੇ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ਨਹੀਂਤਾਂ ਜੇ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ਜੇ (n < 0) {
    ਲਿਖੋ("wc: read error\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਿਖੋ("%d %d %d %s\n", l, w, c, name);
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ fd, i;

  ਜੇ (argc <= 1) {
    wc(0, "");
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਲਈ (i = 1; i < argc; i++) {
    ਜੇ ((fd = open(argv[i], O_RDONLY)) < 0) {
      ਲਿਖੋ("wc: cannot open %s\n", argv[i]);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
