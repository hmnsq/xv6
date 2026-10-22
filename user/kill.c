#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ಗಾಗಿ (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  ನಿರ್ಗಮಿಸು(0);
}
