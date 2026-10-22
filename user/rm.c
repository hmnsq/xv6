#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên i;

  nếu (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  cho (i = 1; i < argc; i++) {
    nếu (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
