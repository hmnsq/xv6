// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
ចំនួនគត់ match(var_na *, var_na *);

void
grep(var_na *pattern, ចំនួនគត់ fd)
{
  ចំនួនគត់ n, m;
  var_na *p, *q;

  m = 0;
  ខណៈពេល ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ខណៈពេល ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ប្រសិនបើ (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ប្រសិនបើ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ fd, i;
  var_na *pattern;

  ប្រសិនបើ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ប្រសិនបើ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  សម្រាប់ (i = 2; i < argc; i++) {
    ប្រសិនបើ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ចំនួនគត់ matchhere(var_na *, var_na *);
ចំនួនគត់ matchstar(ចំនួនគត់, var_na *, var_na *);

ចំនួនគត់
match(var_na *re, var_na *text)
{
  ប្រសិនបើ (re[0] == '^')
    ត្រឡប់ matchhere(re + 1, text);
  do { // must look at empty string
    ប្រសិនបើ (matchhere(re, text))
      ត្រឡប់ 1;
  } ខណៈពេល (*text++ != '\0');
  ត្រឡប់ 0;
}

// matchhere: search for re at beginning of text
ចំនួនគត់
matchhere(var_na *re, var_na *text)
{
  ប្រសិនបើ (re[0] == '\0')
    ត្រឡប់ 1;
  ប្រសិនបើ (re[1] == '*')
    ត្រឡប់ matchstar(re[0], re + 2, text);
  ប្រសិនបើ (re[0] == '$' && re[1] == '\0')
    ត្រឡប់ *text == '\0';
  ប្រសិនបើ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ត្រឡប់ matchhere(re + 1, text + 1);
  ត្រឡប់ 0;
}

// matchstar: search for c*re at beginning of text
ចំនួនគត់
matchstar(ចំនួនគត់ c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    ប្រសិនបើ (matchhere(re, text))
      ត្រឡប់ 1;
  } ខណៈពេល (*text != '\0' && (*text++ == c || c == '.'));
  ត្រឡប់ 0;
}
