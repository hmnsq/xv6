#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#घोषणा BUFSZ 500

अक्षर buf[BUFSZ];

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर **argv)
{
  अक्षर *s = argv[0];

  अगर (सूची_गढ़ो("dd") != 0) {
    म_लिखो("%s: mkdir dd failed\n", s);
    निकास(1);
  }

  अगर (स_बदलो("dd") != 0) {
    म_लिखो("%s: chdir dd failed\n", s);
    निकास(1);
  }

  अगर (unlink("../dd") < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim\n");
  // sit around until killed
  क्रम (;;)
    छोड़ो(1000);
}
