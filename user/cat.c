#شامل "kernel/types.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

حرف buf[512];

خالی
cat(صحیح fd)
{
  صحیح n;

  جب ((n = اقرأ(fd, buf, حجم(buf))) > 0) {
    اگر (اكتب(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      باہر_نکلو(1);
    }
  }
  اگر (n < 0) {
    fprintf(2, "cat: read error\n");
    باہر_نکلو(1);
  }
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح fd, i;

  اگر (argc <= 1) {
    cat(0);
    باہر_نکلو(0);
  }

  برائے (i = 1; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      باہر_نکلو(1);
    }
    cat(fd);
    close(fd);
  }
  باہر_نکلو(0);
}
