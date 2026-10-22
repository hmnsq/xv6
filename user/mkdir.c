#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ i;

  ਜੇ (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਈ (i = 1; i < argc; i++) {
    ਜੇ (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      ਰੋਕੋ;
    }
  }

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
