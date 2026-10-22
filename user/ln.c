#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  यदि (argc != 3) {
    fprintf(2, "Usage: ln old new\n");
    निर्गच्छ(1);
  }
  यदि (link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  निर्गच्छ(0);
}
