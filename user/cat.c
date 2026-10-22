#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

অক্ষর buf[512];

শূন্য
cat(পূর্ণসংখ্যা fd)
{
  পূর্ণসংখ্যা n;

  যখন ((n = পঠ(fd, buf, আকার(buf))) > 0) {
    যদি (লিখ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  যদি (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা fd, i;

  যদি (argc <= 1) {
    cat(0);
    exit(0);
  }

  জন্য (i = 1; i < argc; i++) {
    যদি ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
