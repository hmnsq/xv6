#ضمن "kernel/types.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

حرف buf[512];

فراغ
cat(صحيح fd)
{
  صحيح n;

  طالما ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    إذا (اكتب(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      باہر_نکلو(1);
    }
  }
  إذا (n < 0) {
    fprintf(2, "cat: read error\n");
    باہر_نکلو(1);
  }
}

صحيح
مکھ(صحيح argc, حرف *argv[])
{
  صحيح fd, i;

  إذا (argc <= 1) {
    cat(0);
    باہر_نکلو(0);
  }

  لكل (i = 1; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      باہر_نکلو(1);
    }
    cat(fd);
    close(fd);
  }
  باہر_نکلو(0);
}
