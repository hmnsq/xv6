#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม i;

  ถ้า (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  สำหรับ (i = 1; i < argc; i++) {
    ถ้า (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
