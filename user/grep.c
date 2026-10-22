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
  ܟܡܐ ((n = اقرأ(fd, buf + m, حجم(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ܟܡܐ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ܐܢ (match(pattern, p)) {
        *q = '\n';
        اكتب(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ܐܢ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

صحيح
main(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف *pattern;

  ܐܢ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ܐܢ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  لكل (i = 2; i < argc; i++) {
    ܐܢ ((fd = open(argv[i], O_RDONLY)) < 0) {
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
  ܐܢ (re[0] == '^')
    ܦܢܐ matchhere(re + 1, text);
  ܥܒܕ { // must look at empty string
    ܐܢ (matchhere(re, text))
      ܦܢܐ 1;
  } ܟܡܐ (*text++ != '\0');
  ܦܢܐ 0;
}

// matchhere: search for re at beginning of text
صحيح
matchhere(حرف *re, حرف *text)
{
  ܐܢ (re[0] == '\0')
    ܦܢܐ 1;
  ܐܢ (re[1] == '*')
    ܦܢܐ matchstar(re[0], re + 2, text);
  ܐܢ (re[0] == '$' && re[1] == '\0')
    ܦܢܐ *text == '\0';
  ܐܢ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ܦܢܐ matchhere(re + 1, text + 1);
  ܦܢܐ 0;
}

// matchstar: search for c*re at beginning of text
صحيح
matchstar(صحيح c, حرف *re, حرف *text)
{
  ܥܒܕ { // a * matches zero or more instances
    ܐܢ (matchhere(re, text))
      ܦܢܐ 1;
  } ܟܡܐ (*text != '\0' && (*text++ == c || c == '.'));
  ܦܢܐ 0;
}
