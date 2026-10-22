#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு i;

  எனில் (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  க்கு (i = 1; i < argc; i++) {
    எனில் (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      நிறுத்து;
    }
  }

  exit(0);
}
