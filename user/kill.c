#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
مکھ(صحيح argc, حرف **argv)
{
  صحيح i;

  إذا (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    باہر_نکلو(1);
  }
  لكل (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  باہر_نکلو(0);
}
