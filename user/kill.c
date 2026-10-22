#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

ចំនួនគត់
main(ចំនួនគត់ argc, var_na **argv)
{
  ចំនួនគត់ i;

  ប្រសិនបើ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  សម្រាប់ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
