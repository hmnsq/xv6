// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
số_nguyên match(char *, char *);

void
grep(char *pattern, số_nguyên fd)
{
  số_nguyên n, m;
  char *p, *q;

  m = 0;
  trong_khi ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    trong_khi ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      nếu (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    nếu (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên fd, i;
  char *pattern;

  nếu (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  nếu (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  cho (i = 2; i < argc; i++) {
    nếu ((fd = open(argv[i], O_RDONLY)) < 0) {
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

số_nguyên matchhere(char *, char *);
số_nguyên matchstar(số_nguyên, char *, char *);

số_nguyên
match(char *re, char *text)
{
  nếu (re[0] == '^')
    trả_về matchhere(re + 1, text);
  do { // must look at empty string
    nếu (matchhere(re, text))
      trả_về 1;
  } trong_khi (*text++ != '\0');
  trả_về 0;
}

// matchhere: search for re at beginning of text
số_nguyên
matchhere(char *re, char *text)
{
  nếu (re[0] == '\0')
    trả_về 1;
  nếu (re[1] == '*')
    trả_về matchstar(re[0], re + 2, text);
  nếu (re[0] == '$' && re[1] == '\0')
    trả_về *text == '\0';
  nếu (*text != '\0' && (re[0] == '.' || re[0] == *text))
    trả_về matchhere(re + 1, text + 1);
  trả_về 0;
}

// matchstar: search for c*re at beginning of text
số_nguyên
matchstar(số_nguyên c, char *re, char *text)
{
  do { // a * matches zero or more instances
    nếu (matchhere(re, text))
      trả_về 1;
  } trong_khi (*text != '\0' && (*text++ == c || c == '.'));
  trả_về 0;
}
