#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  ถ้า (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  ถ้า (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
