#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ i;

  ཆེད་དུ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    གལ་ཏེ (i + 1 < argc) {
      write(1, " ", 1);
    } དེ་མིན {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
