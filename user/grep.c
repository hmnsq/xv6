// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
ཧྲིལ match(var_na *, var_na *);

void
grep(var_na *pattern, ཧྲིལ fd)
{
  ཧྲིལ n, m;
  var_na *p, *q;

  m = 0;
  སྐབས ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    སྐབས ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      གལ་སྲིད (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    གལ་སྲིད (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ཧྲིལ
main(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ fd, i;
  var_na *pattern;

  གལ་སྲིད (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  གལ་སྲིད (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  དོན་ལུ (i = 2; i < argc; i++) {
    གལ་སྲིད ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ཧྲིལ matchhere(var_na *, var_na *);
ཧྲིལ matchstar(ཧྲིལ, var_na *, var_na *);

ཧྲིལ
match(var_na *re, var_na *text)
{
  གལ་སྲིད (re[0] == '^')
    ལོག matchhere(re + 1, text);
  do { // must look at empty string
    གལ་སྲིད (matchhere(re, text))
      ལོག 1;
  } སྐབས (*text++ != '\0');
  ལོག 0;
}

// matchhere: search for re at beginning of text
ཧྲིལ
matchhere(var_na *re, var_na *text)
{
  གལ་སྲིད (re[0] == '\0')
    ལོག 1;
  གལ་སྲིད (re[1] == '*')
    ལོག matchstar(re[0], re + 2, text);
  གལ་སྲིད (re[0] == '$' && re[1] == '\0')
    ལོག *text == '\0';
  གལ་སྲིད (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ལོག matchhere(re + 1, text + 1);
  ལོག 0;
}

// matchstar: search for c*re at beginning of text
ཧྲིལ
matchstar(ཧྲིལ c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    གལ་སྲིད (matchhere(re, text))
      ལོག 1;
  } སྐབས (*text != '\0' && (*text++ == c || c == '.'));
  ལོག 0;
}
