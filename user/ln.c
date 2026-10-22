#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  اگر (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  اگر (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
