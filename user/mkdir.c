#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ i;

  གལ་ཏེ (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  ཆེད་དུ (i = 1; i < argc; i++) {
    གལ་ཏེ (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
