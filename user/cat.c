#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

अक्षर buf[512];

शून्य
cat(पूर्णांक fd)
{
  पूर्णांक n;

  पर्यंत ((n = पठ(fd, buf, आकार(buf))) > 0) {
    जर (लिख(1, buf, n) != n) {
      ख_लिखो(2, "cat: write error\n");
      निकास(1);
    }
  }
  जर (n < 0) {
    ख_लिखो(2, "cat: read error\n");
    निकास(1);
  }
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;

  जर (argc <= 1) {
    cat(0);
    निकास(0);
  }

  साठी (i = 1; i < argc; i++) {
    जर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      ख_लिखो(2, "cat: cannot open %s\n", argv[i]);
      निकास(1);
    }
    cat(fd);
    बंद(fd);
  }
  निकास(0);
}
