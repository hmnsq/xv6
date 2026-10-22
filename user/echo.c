#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح i;

  עבור (i = 1; i < argc; i++) {
    اكتب(1, argv[i], strlen(argv[i]));
    אם (i + 1 < argc) {
      اكتب(1, " ", 1);
    } אחרת {
      اكتب(1, "\n", 1);
    }
  }
  exit(0);
}
