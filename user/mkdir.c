#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i;

  अगर (argc < 2) {
    ख_लिखो(2, "Usage: mkdir files...\n");
    निकास(1);
  }

  क्रम (i = 1; i < argc; i++) {
    अगर (सूची_गढ़ो(argv[i]) < 0) {
      ख_लिखो(2, "mkdir: %s failed to create\n", argv[i]);
      अवरोध;
    }
  }

  निकास(0);
}
