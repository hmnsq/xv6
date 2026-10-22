#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên i;

  cho (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    nếu (i + 1 < argc) {
      write(1, " ", 1);
    } không_thì {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
