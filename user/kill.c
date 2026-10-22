#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    ख_लिखो(2, "usage: kill pid...\n");
    निकास(1);
  }
  लागि (i = 1; i < argc; i++)
    समाप्त(म_से_प(argv[i]));
  निकास(0);
}
