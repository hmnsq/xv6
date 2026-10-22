#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ប្រសិនបើ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  ប្រសិនបើ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
