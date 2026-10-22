#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "user/user.h"

ৱর্ণ buf[512];

শূন্য
wc(পূর্ণাঙ্ক fd, ৱর্ণ *name)
{
  পূর্ণাঙ্ক i, n;
  পূর্ণাঙ্ক l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  যেতিয়ালৈ ((n = পঠ(fd, buf, পরিমাণ(buf))) > 0) {
    বাবে (i = 0; i < n; i++) {
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

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক fd, i;

  যদি (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  বাবে (i = 1; i < argc; i++) {
    যদি ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
