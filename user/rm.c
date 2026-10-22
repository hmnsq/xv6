#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka i;

  ᱡᱩᱫᱤ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    ᱡᱩᱫᱤ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      bha_nga;
    }
  }

  exit(0);
}
