#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
مکيه(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    ٻاهر_نڪرو(1);
  }

  لكل (i = 1; i < argc; i++) {
    إذا (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      اكسر;
    }
  }

  ٻاهر_نڪرو(0);
}
