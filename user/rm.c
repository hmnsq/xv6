#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
main(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ i;

  གལ་སྲིད (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  དོན་ལུ (i = 1; i < argc; i++) {
    གལ་སྲིད (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
