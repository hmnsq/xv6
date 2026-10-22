#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर **argv)
{
  पूर्णांक i;

  अगर (argc < 2) {
    ख_लिखो(2, "usage: kill pid...\n");
    निकास(1);
  }
  क्रम (i = 1; i < argc; i++)
    समाप्त(म_से_प(argv[i]));
  निकास(0);
}
