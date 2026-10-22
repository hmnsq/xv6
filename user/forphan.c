#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Create an orphaned file and check if test-xv6.py recovers it.

#परिभाषय BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क fd = 0;
  वर्ण *s = argv[0];
  संरचना stat st;
  वर्ण *ff = "file0";

  यदि ((fd = उद्घाटय(ff, O_CREATE | O_WRONLY)) < 0) {
    प्रारूप_लिख("%s: open failed\n", s);
    निर्गच्छ(1);
  }
  यदि (fstat(fd, &st) < 0) {
    fprintf(2, "%s: cannot stat %s\n", s, "ff");
    निर्गच्छ(1);
  }
  यदि (unlink(ff) < 0) {
    प्रारूप_लिख("%s: unlink failed\n", s);
    निर्गच्छ(1);
  }
  यदि (उद्घाटय(ff, O_RDONLY) != -1) {
    प्रारूप_लिख("%s: open successed\n", s);
    निर्गच्छ(1);
  }
  प्रारूप_लिख("wait for kill and reclaim %d\n", st.ino);
  // sit around until killed
  प्रति (;;)
    pause(1000);
}
