#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[512];

शून्य
wc(पूर्णाङ्क fd, वर्ण *name)
{
  पूर्णाङ्क i, n;
  पूर्णाङ्क l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  जबसम्म ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    लागि (i = 0; i < n; i++) {
      c++;
      यदि (buf[i] == '\n')
        l++;
      यदि (म_अक्षर(" \r\t\n\v", buf[i]))
        inword = 0;
      अन्यथा यदि (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  यदि (n < 0) {
    म_लिखो("wc: read error\n");
    निकास(1);
  }
  म_लिखो("%d %d %d %s\n", l, w, c, name);
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  यदि (argc <= 1) {
    wc(0, "");
    निकास(0);
  }

  लागि (i = 1; i < argc; i++) {
    यदि ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      म_लिखो("wc: cannot open %s\n", argv[i]);
      निकास(1);
    }
    wc(fd, argv[i]);
    बंद(fd);
  }
  निकास(0);
}
