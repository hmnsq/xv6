#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

अक्षर buf[512];

शून्य
wc(पूर्णांक fd, अक्षर *name)
{
  पूर्णांक i, n;
  पूर्णांक l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  पर्यंत ((n = पठ(fd, buf, आकार(buf))) > 0) {
    साठी (i = 0; i < n; i++) {
      c++;
      जर (buf[i] == '\n')
        l++;
      जर (म_अक्षर(" \r\t\n\v", buf[i]))
        inword = 0;
      अन्यथा जर (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  जर (n < 0) {
    म_लिखो("wc: read error\n");
    निकास(1);
  }
  म_लिखो("%d %d %d %s\n", l, w, c, name);
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;

  जर (argc <= 1) {
    wc(0, "");
    निकास(0);
  }

  साठी (i = 1; i < argc; i++) {
    जर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      म_लिखो("wc: cannot open %s\n", argv[i]);
      निकास(1);
    }
    wc(fd, argv[i]);
    बंद(fd);
  }
  निकास(0);
}
