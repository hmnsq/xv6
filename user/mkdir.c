#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(정수 argc, char *argv[])
{
  정수 i;

  만약 (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  위해 (i = 1; i < argc; i++) {
    만약 (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit(0);
}
