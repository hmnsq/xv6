#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned directory and check if test-xv6.py recovers it.

#परिभाषय BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
  वर्ण *s = argv[0];

  यदि (mkdir("dd") != 0) {
    प्रारूप_लिख("%s: mkdir dd failed\n", s);
    निर्गच्छ(1);
  }

  यदि (chdir("dd") != 0) {
    प्रारूप_लिख("%s: chdir dd failed\n", s);
    निर्गच्छ(1);
  }

  यदि (unlink("../dd") < 0) {
    प्रारूप_लिख("%s: unlink failed\n", s);
    निर्गच्छ(1);
  }
  प्रारूप_लिख("wait for kill and reclaim\n");
  // sit around until killed
  प्रति (;;)
    pause(1000);
}
