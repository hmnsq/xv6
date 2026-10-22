#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح i;

  برای (i = 1; i < argc; i++) {
    اكتب(1, argv[i], strlen(argv[i]));
    اگر (i + 1 < argc) {
      اكتب(1, " ", 1);
    } وگرنه {
      اكتب(1, "\n", 1);
    }
  }
  exit(0);
}
