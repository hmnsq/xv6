#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णांक
main(पूर्णांक argc, अक्षर **argv)
{
  पूर्णांक i;

  जर (argc < 2) {
    ख_लिखो(2, "usage: kill pid...\n");
    निकास(1);
  }
  साठी (i = 1; i < argc; i++)
    समाप्त(म_से_प(argv[i]));
  निकास(0);
}
