#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

எழுத்து buf[512];

வெறுமை
cat(முழு fd)
{
  முழு n;

  போது ((n = பட(fd, buf, அளவு(buf))) > 0) {
    எனில் (லிக(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  எனில் (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு fd, i;

  எனில் (argc <= 1) {
    cat(0);
    exit(0);
  }

  க்கு (i = 1; i < argc; i++) {
    எனில் ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
