#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح i;

  אם (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  עבור (i = 1; i < argc; i++) {
    אם (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      اكسر;
    }
  }

  exit(0);
}
