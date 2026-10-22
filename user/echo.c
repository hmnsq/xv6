#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ཧྲིལ
main(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ i;

  དོན་ལུ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    གལ་སྲིད (i + 1 < argc) {
      write(1, " ", 1);
    } དེ་མིན {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
