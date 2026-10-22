#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਅੱਖਰ buf[512];

ਖਾਲੀ
cat(ਪੂਰਨ fd)
{
  ਪੂਰਨ n;

  ਜਦੋਂ ((n = ਪਠ(fd, buf, ਆਕਾਰ(buf))) > 0) {
    ਜੇ (ਲਿਖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }
  ਜੇ (n < 0) {
    fprintf(2, "cat: read error\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ fd, i;

  ਜੇ (argc <= 1) {
    cat(0);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਲਈ (i = 1; i < argc; i++) {
    ਜੇ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    cat(fd);
    close(fd);
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
