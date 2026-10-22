#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

puur_naa_nka
main(puur_naa_nka argc, var_na **argv)
{
  puur_naa_nka i;

  ꦪꦼꦤ꧀ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  ꦏꦁꦒꦺꦴ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
