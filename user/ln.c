#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  सचे (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  सचे (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
