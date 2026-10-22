#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(정수 argc, char *argv[])
{
  정수 i;

  위해 (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    만약 (i + 1 < argc) {
      write(1, " ", 1);
    } 아니면 {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
