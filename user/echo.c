#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ i;

  សម្រាប់ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ប្រសិនបើ (i + 1 < argc) {
      write(1, " ", 1);
    } បើមិនដូច្នេះ {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
