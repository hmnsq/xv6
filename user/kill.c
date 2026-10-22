#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
  முழு i;

  எனில் (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  க்கு (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
