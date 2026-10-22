// Simple grep.  Only supports ^ . * $ operators.

#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

अक्षर buf[1024];
पूर्णांक match(अक्षर *, अक्षर *);

व्योम
grep(अक्षर *pattern, पूर्णांक fd)
{
  पूर्णांक n, m;
  अक्षर *p, *q;

  m = 0;
  जबतक ((n = पढ़ो(fd, buf + m, माप(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    जबतक ((q = म_अक्षर(p, '\n')) != 0) {
      *q = 0;
      अगर (match(pattern, p)) {
        *q = '\n';
        ग_लिखो(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    अगर (m > 0) {
      m -= p - buf;
      स_हटाओ(buf, p, m);
    }
  }
}

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;
  अक्षर *pattern;

  अगर (argc <= 1) {
    ख_लिखो(2, "usage: grep pattern [file ...]\n");
    निकास(1);
  }
  pattern = argv[1];

  अगर (argc <= 2) {
    grep(pattern, 0);
    निकास(0);
  }

  क्रम (i = 2; i < argc; i++) {
    अगर ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
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
  अगर (re[0] == '^')
    वापस matchhere(re + 1, text);
  करो { // must look at empty string
    अगर (matchhere(re, text))
      वापस 1;
  } जबतक (*text++ != '\0');
  वापस 0;
}

// matchhere: search for re at beginning of text
पूर्णांक
matchhere(अक्षर *re, अक्षर *text)
{
  अगर (re[0] == '\0')
    वापस 1;
  अगर (re[1] == '*')
    वापस matchstar(re[0], re + 2, text);
  अगर (re[0] == '$' && re[1] == '\0')
    वापस *text == '\0';
  अगर (*text != '\0' && (re[0] == '.' || re[0] == *text))
    वापस matchhere(re + 1, text + 1);
  वापस 0;
}

// matchstar: search for c*re at beginning of text
पूर्णांक
matchstar(पूर्णांक c, अक्षर *re, अक्षर *text)
{
  करो { // a * matches zero or more instances
    अगर (matchhere(re, text))
      वापस 1;
  } जबतक (*text != '\0' && (*text++ == c || c == '.'));
  वापस 0;
}
