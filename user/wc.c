#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

अक्षर buf[512];

व्योम
wc(पूर्णांक fd, अक्षर *name)
{
  पूर्णांक i, n;
  पूर्णांक l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  जबतक ((n = पढ़ो(fd, buf, माप(buf))) > 0) {
    क्रम (i = 0; i < n; i++) {
      c++;
      अगर (buf[i] == '\n')
        l++;
      अगर (म_अक्षर(" \r\t\n\v", buf[i]))
        inword = 0;
      अन्यथा अगर (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  अगर (n < 0) {
    म_लिखो("wc: read error\n");
    निकास(1);
  }
  म_लिखो("%d %d %d %s\n", l, w, c, name);
}

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;

  अगर (argc <= 1) {
    wc(0, "");
    निकास(0);
  }

  क्रम (i = 1; i < argc; i++) {
    अगर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      म_लिखो("wc: cannot open %s\n", argv[i]);
      निकास(1);
    }
    wc(fd, argv[i]);
    बंद(fd);
  }
  निकास(0);
}
