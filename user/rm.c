#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i;

  अगर (argc < 2) {
    ख_लिखो(2, "Usage: rm files...\n");
    निकास(1);
  }

  क्रम (i = 1; i < argc; i++) {
    अगर (unlink(argv[i]) < 0) {
      ख_लिखो(2, "rm: %s failed to delete\n", argv[i]);
      अवरोध;
    }
  }

  निकास(0);
}
