#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

অক্ষর buf[512];

শূন্য
wc(পূর্ণসংখ্যা fd, অক্ষর *name)
{
  পূর্ণসংখ্যা i, n;
  পূর্ণসংখ্যা l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  যখন ((n = পঠ(fd, buf, আকার(buf))) > 0) {
    জন্য (i = 0; i < n; i++) {
      c++;
      যদি (buf[i] == '\n')
        l++;
      যদি (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      অন্যথা যদি (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  যদি (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা fd, i;

  যদি (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  জন্য (i = 1; i < argc; i++) {
    যদি ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
