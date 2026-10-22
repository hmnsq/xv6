// Simple grep.  Only supports ^ . * $ operators.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

حرف buf[1024];
صحيح match(حرف *, حرف *);

فراغ
grep(حرف *pattern, صحيح fd)
{
  صحيح n, m;
  حرف *p, *q;

  m = 0;
  بينما ((n = اقرأ(fd, buf + m, حجم(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    بينما ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      إذا (match(pattern, p)) {
        *q = '\n';
        اكتب(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    إذا (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

صحيح
الرئيسية(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف *pattern;

  إذا (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  إذا (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  لكل (i = 2; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
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

صحيح matchhere(حرف *, حرف *);
صحيح matchstar(صحيح, حرف *, حرف *);

صحيح
match(حرف *re, حرف *text)
{
  إذا (re[0] == '^')
    إرجاع matchhere(re + 1, text);
  افعل { // must look at empty string
    إذا (matchhere(re, text))
      إرجاع 1;
  } بينما (*text++ != '\0');
  إرجاع 0;
}

// matchhere: search for re at beginning of text
صحيح
matchhere(حرف *re, حرف *text)
{
  إذا (re[0] == '\0')
    إرجاع 1;
  إذا (re[1] == '*')
    إرجاع matchstar(re[0], re + 2, text);
  إذا (re[0] == '$' && re[1] == '\0')
    إرجاع *text == '\0';
  إذا (*text != '\0' && (re[0] == '.' || re[0] == *text))
    إرجاع matchhere(re + 1, text + 1);
  إرجاع 0;
}

// matchstar: search for c*re at beginning of text
صحيح
matchstar(صحيح c, حرف *re, حرف *text)
{
  افعل { // a * matches zero or more instances
    إذا (matchhere(re, text))
      إرجاع 1;
  } بينما (*text != '\0' && (*text++ == c || c == '.'));
  إرجاع 0;
}
