#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

अक्षर buf[BUFSZ];

पूर्णांक
main(पूर्णांक argc, अक्षर **argv)
{
  पूर्णांक fd = 0;
  अक्षर *s = argv[0];
  संरचना stat st;
  अक्षर *ff = "file0";

  जर ((fd = खोलो(ff, O_CREATE | O_WRONLY)) < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  जर (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "%s: cannot stat %s\n", s, "ff");
    निकास(1);
  }
  जर (unlink(ff) < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  जर (खोलो(ff, O_RDONLY) != -1) {
    म_लिखो("%s: open successed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  साठी (;;)
    छोड़ो(1000);
}
