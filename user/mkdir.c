#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح i;

  اگر (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    باہر_نکلو(1);
  }

  برائے (i = 1; i < argc; i++) {
    اگر (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      توڑو;
    }
  }

  باہر_نکلو(0);
}
