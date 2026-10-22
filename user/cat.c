#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "user/user.h"

ৱর্ণ buf[512];

শূন্য
cat(পূর্ণাঙ্ক fd)
{
  পূর্ণাঙ্ক n;

  যেতিয়ালৈ ((n = পঠ(fd, buf, পরিমাণ(buf))) > 0) {
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

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক fd, i;

  যদি (argc <= 1) {
    cat(0);
    exit(0);
  }

  বাবে (i = 1; i < argc; i++) {
    যদি ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
