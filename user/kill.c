#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
  ਪੂਰਨ i;

  ਜੇ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਲਈ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
