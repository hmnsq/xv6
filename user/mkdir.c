#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    निर्गच्छ(1);
  }

  प्रति (i = 1; i < argc; i++) {
    यदि (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      भङ्ग;
    }
  }

  निर्गच्छ(0);
}
