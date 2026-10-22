#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح i;

  𐤀𐤌 (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  لكل (i = 1; i < argc; i++) {
    𐤀𐤌 (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      اكسر;
    }
  }

  exit(0);
}
