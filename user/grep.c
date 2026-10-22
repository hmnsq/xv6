// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
整数 match(char *, char *);

void
grep(char *pattern, 整数 fd)
{
  整数 n, m;
  char *p, *q;

  m = 0;
  当 ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    当 ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      如果 (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    如果 (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

整数
主要(整数 argc, char *argv[])
{
  整数 fd, i;
  char *pattern;

  如果 (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  如果 (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  为 (i = 2; i < argc; i++) {
    如果 ((fd = open(argv[i], O_RDONLY)) < 0) {
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

整数 matchhere(char *, char *);
整数 matchstar(整数, char *, char *);

整数
match(char *re, char *text)
{
  如果 (re[0] == '^')
    返回 matchhere(re + 1, text);
  do { // must look at empty string
    如果 (matchhere(re, text))
      返回 1;
  } 当 (*text++ != '\0');
  返回 0;
}

// matchhere: search for re at beginning of text
整数
matchhere(char *re, char *text)
{
  如果 (re[0] == '\0')
    返回 1;
  如果 (re[1] == '*')
    返回 matchstar(re[0], re + 2, text);
  如果 (re[0] == '$' && re[1] == '\0')
    返回 *text == '\0';
  如果 (*text != '\0' && (re[0] == '.' || re[0] == *text))
    返回 matchhere(re + 1, text + 1);
  返回 0;
}

// matchstar: search for c*re at beginning of text
整数
matchstar(整数 c, char *re, char *text)
{
  do { // a * matches zero or more instances
    如果 (matchhere(re, text))
      返回 1;
  } 当 (*text != '\0' && (*text++ == c || c == '.'));
  返回 0;
}
