#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح i;

  اگر (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  برای (i = 1; i < argc; i++) {
    اگر (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      بشکن;
    }
  }

  exit(0);
}
