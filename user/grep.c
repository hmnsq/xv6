// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
จำนวนเต็ม match(var_na *, var_na *);

void
grep(var_na *pattern, จำนวนเต็ม fd)
{
  จำนวนเต็ม n, m;
  var_na *p, *q;

  m = 0;
  ขณะ ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ขณะ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ถ้า (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ถ้า (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม fd, i;
  var_na *pattern;

  ถ้า (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ถ้า (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  สำหรับ (i = 2; i < argc; i++) {
    ถ้า ((fd = open(argv[i], O_RDONLY)) < 0) {
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

จำนวนเต็ม matchhere(var_na *, var_na *);
จำนวนเต็ม matchstar(จำนวนเต็ม, var_na *, var_na *);

จำนวนเต็ม
match(var_na *re, var_na *text)
{
  ถ้า (re[0] == '^')
    คืน matchhere(re + 1, text);
  do { // must look at empty string
    ถ้า (matchhere(re, text))
      คืน 1;
  } ขณะ (*text++ != '\0');
  คืน 0;
}

// matchhere: search for re at beginning of text
จำนวนเต็ม
matchhere(var_na *re, var_na *text)
{
  ถ้า (re[0] == '\0')
    คืน 1;
  ถ้า (re[1] == '*')
    คืน matchstar(re[0], re + 2, text);
  ถ้า (re[0] == '$' && re[1] == '\0')
    คืน *text == '\0';
  ถ้า (*text != '\0' && (re[0] == '.' || re[0] == *text))
    คืน matchhere(re + 1, text + 1);
  คืน 0;
}

// matchstar: search for c*re at beginning of text
จำนวนเต็ม
matchstar(จำนวนเต็ม c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    ถ้า (matchhere(re, text))
      คืน 1;
  } ขณะ (*text != '\0' && (*text++ == c || c == '.'));
  คืน 0;
}
