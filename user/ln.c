#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
اصلی(صحيح argc, حرف *argv[])
{
  إذا (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    خروج(1);
  }
  إذا (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  خروج(0);
}
