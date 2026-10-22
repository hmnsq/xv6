#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு i;

  க்கு (i = 1; i < argc; i++) {
    லிக(1, argv[i], strlen(argv[i]));
    எனில் (i + 1 < argc) {
      லிக(1, " ", 1);
    } இல்லையெனில் {
      லிக(1, "\n", 1);
    }
  }
  exit(0);
}
