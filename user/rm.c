#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် i;

  အကယ်၍ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  အတွက် (i = 1; i < argc; i++) {
    အကယ်၍ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
