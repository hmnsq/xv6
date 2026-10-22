#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#define BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क fd = 0;
  वर्ण *s = argv[0];
  संरचना stat st;
  वर्ण *ff = "file0";

  यदि ((fd = खोलो(ff, O_CREATE | O_WRONLY)) < 0) {
    म_लिखो("%s: open failed\n", s);
    निकास(1);
  }
  यदि (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "%s: cannot stat %s\n", s, "ff");
    निकास(1);
  }
  यदि (unlink(ff) < 0) {
    म_लिखो("%s: unlink failed\n", s);
    निकास(1);
  }
  यदि (खोलो(ff, O_RDONLY) != -1) {
    म_लिखो("%s: open successed\n", s);
    निकास(1);
  }
  म_लिखो("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  लागि (;;)
    छोड़ो(1000);
}
