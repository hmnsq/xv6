#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

अक्षर buf[BUFSZ];

पूर्णांक
main(पूर्णांक argc, अक्षर **argv)
{
  अक्षर *s = argv[0];

  जर (सूची_गढ़ो("dd") != 0) {
    म_लिखो("%s: mkdir dd failed\n", s);
    निकास(1);
  }

  जर (स_बदलो("dd") != 0) {
    म_लिखो("%s: chdir dd failed\n", s);
    निकास(1);
  }

  जर (unlink("../dd") < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim\n");
  // sit around until killed
  साठी (;;)
    छोड़ो(1000);
}
