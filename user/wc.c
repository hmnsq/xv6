#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

எழுத்து buf[512];

வெறுமை
wc(முழு fd, எழுத்து *name)
{
  முழு i, n;
  முழு l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  போது ((n = பட(fd, buf, அளவு(buf))) > 0) {
    க்கு (i = 0; i < n; i++) {
      c++;
      எனில் (buf[i] == '\n')
        l++;
      எனில் (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      இல்லையெனில் எனில் (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  எனில் (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு fd, i;

  எனில் (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  க்கு (i = 1; i < argc; i++) {
    எனில் ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
