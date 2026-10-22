// Simple grep.  Only supports ^ . * $ operators.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[1024];
पूर्णाङ्क match(वर्ण *, वर्ण *);

शून्य
grep(वर्ण *pattern, पूर्णाङ्क fd)
{
  पूर्णाङ्क n, m;
  वर्ण *p, *q;

  m = 0;
  जबसम्म ((n = पठ(fd, buf + m, परिमाण(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    जबसम्म ((q = म_अक्षर(p, '\n')) != 0) {
      *q = 0;
      यदि (match(pattern, p)) {
        *q = '\n';
        लिख(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    यदि (m > 0) {
      m -= p - buf;
      स_हटाओ(buf, p, m);
    }
  }
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;
  वर्ण *pattern;

  यदि (argc <= 1) {
    ख_लिखो(2, "usage: grep pattern [file ...]\n");
    निकास(1);
  }
  pattern = argv[1];

  यदि (argc <= 2) {
    grep(pattern, 0);
    निकास(0);
  }

  लागि (i = 2; i < argc; i++) {
    यदि ((fd = खोलो(argv[i], O_RDONLY)) < 0) {
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

पूर्णाङ्क matchhere(वर्ण *, वर्ण *);
पूर्णाङ्क matchstar(पूर्णाङ्क, वर्ण *, वर्ण *);

पूर्णाङ्क
match(वर्ण *re, वर्ण *text)
{
  यदि (re[0] == '^')
    फर्काऊ matchhere(re + 1, text);
  गर { // must look at empty string
    यदि (matchhere(re, text))
      फर्काऊ 1;
  } जबसम्म (*text++ != '\0');
  फर्काऊ 0;
}

// matchhere: search for re at beginning of text
पूर्णाङ्क
matchhere(वर्ण *re, वर्ण *text)
{
  यदि (re[0] == '\0')
    फर्काऊ 1;
  यदि (re[1] == '*')
    फर्काऊ matchstar(re[0], re + 2, text);
  यदि (re[0] == '$' && re[1] == '\0')
    फर्काऊ *text == '\0';
  यदि (*text != '\0' && (re[0] == '.' || re[0] == *text))
    फर्काऊ matchhere(re + 1, text + 1);
  फर्काऊ 0;
}

// matchstar: search for c*re at beginning of text
पूर्णाङ्क
matchstar(पूर्णाङ्क c, वर्ण *re, वर्ण *text)
{
  गर { // a * matches zero or more instances
    यदि (matchhere(re, text))
      फर्काऊ 1;
  } जबसम्म (*text != '\0' && (*text++ == c || c == '.'));
  फर्काऊ 0;
}
