#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  जर (argc != 3) {
    ख_लिखो(2, "Usage: ln old new\n");
    निकास(1);
  }
  जर (link(argv[1], argv[2]) < 0)
    ख_लिखो(2, "link %s %s: failed\n", argv[1], argv[2]);
  निकास(0);
}
