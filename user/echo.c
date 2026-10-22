#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม i;

  สำหรับ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ถ้า (i + 1 < argc) {
      write(1, " ", 1);
    } มิฉะนั้น {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
