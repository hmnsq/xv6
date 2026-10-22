#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
مین(صحیح argc, حرف *argv[])
{
  اگر (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    باہر_نکلو(1);
  }
  اگر (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  باہر_نکلو(0);
}
