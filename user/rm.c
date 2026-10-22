#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ i;

  ប្រសិនបើ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  សម្រាប់ (i = 1; i < argc; i++) {
    ប្រសិនបើ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
