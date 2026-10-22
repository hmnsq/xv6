#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ (i = 1; i < argc; i++) {
    ಲಿಖ(1, argv[i], strlen(argv[i]));
    ಯದಿ (i + 1 < argc) {
      ಲಿಖ(1, " ", 1);
    } ಅನ್ಯಥಾ {
      ಲಿಖ(1, "\n", 1);
    }
  }
  exit(0);
}
