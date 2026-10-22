#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[512];

शून्य
cat(पूर्णाङ्क fd)
{
  पूर्णाङ्क n;

  यावत् ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    यदि (लिख(1, buf, n) != n) {
      ख_लिखो(2, "cat: write error\n");
      निकास(1);
    }
  }
  यदि (n < 0) {
    ख_लिखो(2, "cat: read error\n");
    निकास(1);
  }
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  यदि (argc <= 1) {
    cat(0);
    निकास(0);
  }

  प्रति (i = 1; i < argc; i++) {
    यदि ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      ख_लिखो(2, "cat: cannot open %s\n", argv[i]);
      निकास(1);
    }
    cat(fd);
    बंद(fd);
  }
  निकास(0);
}
