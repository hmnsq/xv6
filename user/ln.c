#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(صحيح argc, حرف *argv[])
{
  אם (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  אם (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
