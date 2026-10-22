#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਜੇ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਜੇ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
