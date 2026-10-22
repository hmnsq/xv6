#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
الرئيسية(صحيح argc, حرف *argv[])
{
  صحيح i;

  لكل (i = 1; i < argc; i++) {
    اكتب(1, argv[i], strlen(argv[i]));
    إذا (i + 1 < argc) {
      اكتب(1, " ", 1);
    } وإلا {
      اكتب(1, "\n", 1);
    }
  }
  exit(0);
}
