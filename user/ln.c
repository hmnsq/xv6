#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  अगर (argc != 3) {
    ख_लिखो(2, "Usage: ln old new\n");
    निकास(1);
  }
  अगर (link(argv[1], argv[2]) < 0)
    ख_लिखो(2, "link %s %s: failed\n", argv[1], argv[2]);
  निकास(0);
}
