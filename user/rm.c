#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা i;

  যদি (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  জন্য (i = 1; i < argc; i++) {
    যদি (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      বিরতি;
    }
  }

  exit(0);
}
