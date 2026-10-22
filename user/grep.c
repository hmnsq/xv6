// Simple grep.  Only supports ^ . * $ operators.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "user/user.h"

حرف buf[1024];
صحيح ميلاپ(حرف *, حرف *);

فراغ
grep(حرف *نمونو, صحيح fd)
{
  صحيح n, m;
  حرف *p, *q;

  m = 0;
  طالما ((n = اقرأ(fd, buf + m, حجم(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    طالما ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      إذا (ميلاپ(نمونو, p)) {
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
مکيه(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف *نمونو;

  إذا (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    ٻاهر_نڪرو(1);
  }
  نمونو = argv[1];

  إذا (argc <= 2) {
    grep(نمونو, 0);
    ٻاهر_نڪرو(0);
  }

  لكل (i = 2; i < argc; i++) {
    إذا ((fd = open(argv[i], O_RDONLY)) < 0) {
      لکو("grep: cannot open %s\n", argv[i]);
      ٻاهر_نڪرو(1);
    }
    grep(نمونو, fd);
    close(fd);
  }
  ٻاهر_نڪرو(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

صحيح matchhere(حرف *, حرف *);
صحيح matchstar(صحيح, حرف *, حرف *);

صحيح
ميلاپ(حرف *re, حرف *متن)
{
  إذا (re[0] == '^')
    أرجع matchhere(re + 1, متن);
  افعل { // must look at empty string
    إذا (matchhere(re, متن))
      أرجع 1;
  } طالما (*متن++ != '\0');
  أرجع 0;
}

// matchhere: search for re at beginning of text
صحيح
matchhere(حرف *re, حرف *متن)
{
  إذا (re[0] == '\0')
    أرجع 1;
  إذا (re[1] == '*')
    أرجع matchstar(re[0], re + 2, متن);
  إذا (re[0] == '$' && re[1] == '\0')
    أرجع *متن == '\0';
  إذا (*متن != '\0' && (re[0] == '.' || re[0] == *متن))
    أرجع matchhere(re + 1, متن + 1);
  أرجع 0;
}

// matchstar: search for c*re at beginning of text
صحيح
matchstar(صحيح c, حرف *re, حرف *متن)
{
  افعل { // a * matches zero or more instances
    إذا (matchhere(re, متن))
      أرجع 1;
  } طالما (*متن != '\0' && (*متن++ == c || c == '.'));
  أرجع 0;
}
