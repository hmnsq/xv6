#समावेश "kernel/types.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

अक्षर buf[512];

व्योम
cat(पूर्णांक fd)
{
  पूर्णांक n;

  जबतक ((n = पढ़ो(fd, buf, माप(buf))) > 0) {
    अगर (ग_लिखो(1, buf, n) != n) {
      ख_लिखो(2, "cat: write error\n");
      निकास(1);
    }
  }
  अगर (n < 0) {
    ख_लिखो(2, "cat: read error\n");
    निकास(1);
  }
}

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;

  अगर (argc <= 1) {
    cat(0);
    निकास(0);
  }

  क्रम (i = 1; i < argc; i++) {
    अगर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      ख_लिखो(2, "cat: cannot open %s\n", argv[i]);
      निकास(1);
    }
    cat(fd);
    बंद(fd);
  }
  निकास(0);
}
