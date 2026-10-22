#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
الرئيسية(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  لكل (i = 1; i < argc; i++) {
    إذا (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      اكسر;
    }
  }

  exit(0);
}
