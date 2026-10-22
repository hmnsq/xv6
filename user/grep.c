// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
ကိန်းပြည့် match(var_na *, var_na *);

void
grep(var_na *pattern, ကိန်းပြည့် fd)
{
  ကိန်းပြည့် n, m;
  var_na *p, *q;

  m = 0;
  နေစဉ် ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    နေစဉ် ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      အကယ်၍ (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    အကယ်၍ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် fd, i;
  var_na *pattern;

  အကယ်၍ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  အကယ်၍ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  အတွက် (i = 2; i < argc; i++) {
    အကယ်၍ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ကိန်းပြည့် matchhere(var_na *, var_na *);
ကိန်းပြည့် matchstar(ကိန်းပြည့်, var_na *, var_na *);

ကိန်းပြည့်
match(var_na *re, var_na *text)
{
  အကယ်၍ (re[0] == '^')
    ပြန်ပေး matchhere(re + 1, text);
  do { // must look at empty string
    အကယ်၍ (matchhere(re, text))
      ပြန်ပေး 1;
  } နေစဉ် (*text++ != '\0');
  ပြန်ပေး 0;
}

// matchhere: search for re at beginning of text
ကိန်းပြည့်
matchhere(var_na *re, var_na *text)
{
  အကယ်၍ (re[0] == '\0')
    ပြန်ပေး 1;
  အကယ်၍ (re[1] == '*')
    ပြန်ပေး matchstar(re[0], re + 2, text);
  အကယ်၍ (re[0] == '$' && re[1] == '\0')
    ပြန်ပေး *text == '\0';
  အကယ်၍ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ပြန်ပေး matchhere(re + 1, text + 1);
  ပြန်ပေး 0;
}

// matchstar: search for c*re at beginning of text
ကိန်းပြည့်
matchstar(ကိန်းပြည့် c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    အကယ်၍ (matchhere(re, text))
      ပြန်ပေး 1;
  } နေစဉ် (*text != '\0' && (*text++ == c || c == '.'));
  ပြန်ပေး 0;
}
