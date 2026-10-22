#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  ᰀᰧ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  ᰀᰧ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
