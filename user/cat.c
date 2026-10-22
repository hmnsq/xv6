#ضمن "kernel/types.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

حرف buf[512];

فراغ
cat(صحيح fd)
{
  صحيح n;

  طالما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    𐤀𐤌 (اكتب(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  𐤀𐤌 (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  𐤀𐤌 (argc <= 1) {
    cat(0);
    exit(0);
  }

  لكل (i = 1; i < argc; i++) {
    𐤀𐤌 ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
