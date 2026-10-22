#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#घोषणा BUFSZ 500

अक्षर buf[BUFSZ];

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर **argv)
{
  पूर्णांक fd = 0;
  अक्षर *s = argv[0];
  काष्ठा stat st;
  अक्षर *ff = "file0";

  अगर ((fd = खोलो(ff, O_CREATE | O_WRONLY)) < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  अगर (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "%s: cannot stat %s\n", s, "ff");
    निकास(1);
  }
  अगर (unlink(ff) < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  अगर (खोलो(ff, O_RDONLY) != -1) {
    म_लिखो("%s: open successed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  क्रम (;;)
    छोड़ो(1000);
}
