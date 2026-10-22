#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
اصلی(صحیح argc, نویسه **argv)
{
  صحیح i;

  اگر (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  برای (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
