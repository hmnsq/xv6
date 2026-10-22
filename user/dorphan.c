#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#define BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  वर्ण *s = argv[0];

  यदि (सूची_गढ़ो("dd") != 0) {
    म_लिखो("%s: mkdir dd failed\n", s);
    निकास(1);
  }

  यदि (स_बदलो("dd") != 0) {
    म_लिखो("%s: chdir dd failed\n", s);
    निकास(1);
  }

  यदि (unlink("../dd") < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim\n");
  // sit around until killed
  लागि (;;)
    छोड़ो(1000);
}
