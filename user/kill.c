#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na **argv)
{
  จำนวนเต็ม i;

  ถ้า (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  สำหรับ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
