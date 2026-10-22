#شامل "kernel/types.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

نویسه buf[512];

تهی
cat(صحیح fd)
{
  صحیح n;

  وقتی ((n = اقرأ(fd, buf, اندازه(buf))) > 0) {
    اگر (اكتب(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  اگر (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح fd, i;

  اگر (argc <= 1) {
    cat(0);
    exit(0);
  }

  برای (i = 1; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
