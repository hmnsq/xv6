#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  प्रति (i = 1; i < argc; i++) {
    लिख(1, argv[i], strlen(argv[i]));
    जइ (i + 1 < argc) {
      लिख(1, " ", 1);
    } अण्णहा {
      लिख(1, "\n", 1);
    }
  }
  exit(0);
}
