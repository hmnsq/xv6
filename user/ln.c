#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಆದರೆ (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಆದರೆ (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  ನಿರ್ಗಮಿಸು(0);
}
