#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
مکھ(صحيح argc, حرف *argv[])
{
  إذا (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    باہر_نکلو(1);
  }
  إذا (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  باہر_نکلو(0);
}
