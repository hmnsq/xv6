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
  याव ((n = पठ(fd, buf + m, परिमाण(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    याव ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      सचे (match(pattern, p)) {
        *q = '\n';
        लिख(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    सचे (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;
  वर्ण *pattern;

  सचे (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  सचे (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  अत्थाय (i = 2; i < argc; i++) {
    सचे ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);
    }
    grep(pattern, fd);
    close(fd);
  }
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

पूर्णाङ्क matchhere(वर्ण *, वर्ण *);
पूर्णाङ्क matchstar(पूर्णाङ्क, वर्ण *, वर्ण *);

पूर्णाङ्क
match(वर्ण *re, वर्ण *text)
{
  सचे (re[0] == '^')
    पटिनिवत्त matchhere(re + 1, text);
  करोहि { // must look at empty string
    सचे (matchhere(re, text))
      पटिनिवत्त 1;
  } याव (*text++ != '\0');
  पटिनिवत्त 0;
}

// matchhere: search for re at beginning of text
पूर्णाङ्क
matchhere(वर्ण *re, वर्ण *text)
{
  सचे (re[0] == '\0')
    पटिनिवत्त 1;
  सचे (re[1] == '*')
    पटिनिवत्त matchstar(re[0], re + 2, text);
  सचे (re[0] == '$' && re[1] == '\0')
    पटिनिवत्त *text == '\0';
  सचे (*text != '\0' && (re[0] == '.' || re[0] == *text))
    पटिनिवत्त matchhere(re + 1, text + 1);
  पटिनिवत्त 0;
}

// matchstar: search for c*re at beginning of text
पूर्णाङ्क
matchstar(पूर्णाङ्क c, वर्ण *re, वर्ण *text)
{
  करोहि { // a * matches zero or more instances
    सचे (matchhere(re, text))
      पटिनिवत्त 1;
  } याव (*text != '\0' && (*text++ == c || c == '.'));
  पटिनिवत्त 0;
}
