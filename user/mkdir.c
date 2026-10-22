#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  सचे (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  अत्थाय (i = 1; i < argc; i++) {
    सचे (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      भङ्ग;
    }
  }

  exit(0);
}
