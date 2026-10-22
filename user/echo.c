#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် i;

  အတွက် (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    အကယ်၍ (i + 1 < argc) {
      write(1, " ", 1);
    } မဟုတ်ပါက {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
