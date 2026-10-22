#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  အကယ်၍ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  အကယ်၍ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
