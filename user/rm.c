#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு i;

  எனில் (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  க்கு (i = 1; i < argc; i++) {
    எனில் (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      நிறுத்து;
    }
  }

  exit(0);
}
