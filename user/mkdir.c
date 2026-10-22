#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক i;

  যদি (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  বাবে (i = 1; i < argc; i++) {
    যদি (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      ভঙ্গ;
    }
  }

  exit(0);
}
