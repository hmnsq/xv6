#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  प्रति (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
