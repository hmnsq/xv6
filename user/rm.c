#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  ಪ್ರತಿ (i = 1; i < argc; i++) {
    ಯದಿ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      ಭಙ್ಗ;
    }
  }

  exit(0);
}
