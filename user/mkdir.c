#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  ପାଇଁ (i = 1; i < argc; i++) {
    ଯଦି (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      ଭାଙ୍ଗ;
    }
  }

  exit(0);
}
