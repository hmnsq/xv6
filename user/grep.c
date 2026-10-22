// Simple grep.  Only supports ^ . * $ operators.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

अक्षर buf[1024];
पूर्णांक match(अक्षर *, अक्षर *);

शून्य
grep(अक्षर *pattern, पूर्णांक fd)
{
  पूर्णांक n, m;
  अक्षर *p, *q;

  m = 0;
  पर्यंत ((n = पठ(fd, buf + m, आकार(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    पर्यंत ((q = म_अक्षर(p, '\n')) != 0) {
      *q = 0;
      जर (match(pattern, p)) {
        *q = '\n';
        लिख(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    जर (m > 0) {
      m -= p - buf;
      स_हटाओ(buf, p, m);
    }
  }
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;
  अक्षर *pattern;

  जर (argc <= 1) {
    ख_लिखो(2, "usage: grep pattern [file ...]\n");
    निकास(1);
  }
  pattern = argv[1];

  जर (argc <= 2) {
    grep(pattern, 0);
    निकास(0);
  }

  साठी (i = 2; i < argc; i++) {
    जर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
      म_लिखो("grep: cannot open %s\n", argv[i]);
      निकास(1);
    }
    grep(pattern, fd);
    बंद(fd);
  }
  निकास(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

पूर्णांक matchhere(अक्षर *, अक्षर *);
पूर्णांक matchstar(पूर्णांक, अक्षर *, अक्षर *);

पूर्णांक
match(अक्षर *re, अक्षर *text)
{
  जर (re[0] == '^')
    परत matchhere(re + 1, text);
  करा { // must look at empty string
    जर (matchhere(re, text))
      परत 1;
  } पर्यंत (*text++ != '\0');
  परत 0;
}

// matchhere: search for re at beginning of text
पूर्णांक
matchhere(अक्षर *re, अक्षर *text)
{
  जर (re[0] == '\0')
    परत 1;
  जर (re[1] == '*')
    परत matchstar(re[0], re + 2, text);
  जर (re[0] == '$' && re[1] == '\0')
    परत *text == '\0';
  जर (*text != '\0' && (re[0] == '.' || re[0] == *text))
    परत matchhere(re + 1, text + 1);
  परत 0;
}

// matchstar: search for c*re at beginning of text
पूर्णांक
matchstar(पूर्णांक c, अक्षर *re, अक्षर *text)
{
  करा { // a * matches zero or more instances
    जर (matchhere(re, text))
      परत 1;
  } पर्यंत (*text != '\0' && (*text++ == c || c == '.'));
  परत 0;
}
