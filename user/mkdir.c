#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i;

  जर (argc < 2) {
    ख_लिखो(2, "Usage: mkdir files...\n");
    निकास(1);
  }

  साठी (i = 1; i < argc; i++) {
    जर (सूची_गढ़ो(argv[i]) < 0) {
      ख_लिखो(2, "mkdir: %s failed to create\n", argv[i]);
      थांबा;
    }
  }

  निकास(0);
}
