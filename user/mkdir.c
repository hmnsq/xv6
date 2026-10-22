#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    ख_लिखो(2, "Usage: mkdir files...\n");
    निकास(1);
  }

  लागि (i = 1; i < argc; i++) {
    यदि (सूची_गढ़ो(argv[i]) < 0) {
      ख_लिखो(2, "mkdir: %s failed to create\n", argv[i]);
      रोक;
    }
  }

  निकास(0);
}
