#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka i;

  ꦏꦁꦒꦺꦴ (i = 1; i < argc; i++) {
    write(1, argv[i], strlen(argv[i]));
    ꦪꦼꦤ꧀ (i + 1 < argc) {
      write(1, " ", 1);
    } ꦭꦶꦪ {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
