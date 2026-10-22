#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح i;

  اگر (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  برای (i = 1; i < argc; i++) {
    اگر (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      بشکن;
    }
  }

  exit(0);
}
