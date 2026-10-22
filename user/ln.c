#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  எனில் (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  எனில் (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
