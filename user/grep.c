// Simple grep.  Only supports ^ . * $ operators.

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

نویسه buf[1024];
صحیح match(نویسه *, نویسه *);

تهی
grep(نویسه *pattern, صحیح fd)
{
  صحیح n, m;
  نویسه *p, *q;

  m = 0;
  وقتی ((n = اقرأ(fd, buf + m, اندازه(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    وقتی ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      اگر (match(pattern, p)) {
        *q = '\n';
        اكتب(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    اگر (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح fd, i;
  نویسه *pattern;

  اگر (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  اگر (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  برای (i = 2; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
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

صحیح matchhere(نویسه *, نویسه *);
صحیح matchstar(صحیح, نویسه *, نویسه *);

صحیح
match(نویسه *re, نویسه *text)
{
  اگر (re[0] == '^')
    بازگشت matchhere(re + 1, text);
  انجامبده { // must look at empty string
    اگر (matchhere(re, text))
      بازگشت 1;
  } وقتی (*text++ != '\0');
  بازگشت 0;
}

// matchhere: search for re at beginning of text
صحیح
matchhere(نویسه *re, نویسه *text)
{
  اگر (re[0] == '\0')
    بازگشت 1;
  اگر (re[1] == '*')
    بازگشت matchstar(re[0], re + 2, text);
  اگر (re[0] == '$' && re[1] == '\0')
    بازگشت *text == '\0';
  اگر (*text != '\0' && (re[0] == '.' || re[0] == *text))
    بازگشت matchhere(re + 1, text + 1);
  بازگشت 0;
}

// matchstar: search for c*re at beginning of text
صحیح
matchstar(صحیح c, نویسه *re, نویسه *text)
{
  انجامبده { // a * matches zero or more instances
    اگر (matchhere(re, text))
      بازگشت 1;
  } وقتی (*text != '\0' && (*text++ == c || c == '.'));
  بازگشت 0;
}
