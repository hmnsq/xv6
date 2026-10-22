#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
مین(صحیح argc, حرف **argv)
{
  صحیح i;

  اگر (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    باہر_نکلو(1);
  }
  برائے (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  باہر_نکلو(0);
}
