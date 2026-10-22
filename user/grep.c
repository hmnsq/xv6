// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
ціле match(char *, char *);

void
grep(char *pattern, ціле fd)
{
  ціле n, m;
  char *p, *q;

  m = 0;
  поки ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    поки ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      якщо (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    якщо (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ціле
main(ціле argc, char *argv[])
{
  ціле fd, i;
  char *pattern;

  якщо (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  якщо (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  для (i = 2; i < argc; i++) {
    якщо ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ціле matchhere(char *, char *);
ціле matchstar(ціле, char *, char *);

ціле
match(char *re, char *text)
{
  якщо (re[0] == '^')
    повернути matchhere(re + 1, text);
  do { // must look at empty string
    якщо (matchhere(re, text))
      повернути 1;
  } поки (*text++ != '\0');
  повернути 0;
}

// matchhere: search for re at beginning of text
ціле
matchhere(char *re, char *text)
{
  якщо (re[0] == '\0')
    повернути 1;
  якщо (re[1] == '*')
    повернути matchstar(re[0], re + 2, text);
  якщо (re[0] == '$' && re[1] == '\0')
    повернути *text == '\0';
  якщо (*text != '\0' && (re[0] == '.' || re[0] == *text))
    повернути matchhere(re + 1, text + 1);
  повернути 0;
}

// matchstar: search for c*re at beginning of text
ціле
matchstar(ціле c, char *re, char *text)
{
  do { // a * matches zero or more instances
    якщо (matchhere(re, text))
      повернути 1;
  } поки (*text != '\0' && (*text++ == c || c == '.'));
  повернути 0;
}
