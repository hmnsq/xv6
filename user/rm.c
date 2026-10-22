#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ *argv[])
{
  পূর্ণাঙ্ক i;

  যদি (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  প্রতি (i = 1; i < argc; i++) {
    যদি (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      ভঙ্গ;
    }
  }

  exit(0);
}
