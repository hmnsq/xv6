// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
целое match(char *, char *);

void
grep(char *pattern, целое fd)
{
  целое n, m;
  char *p, *q;

  m = 0;
  пока ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    пока ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      если (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    если (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

целое
основной(целое argc, char *argv[])
{
  целое fd, i;
  char *pattern;

  если (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  если (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  для (i = 2; i < argc; i++) {
    если ((fd = open(argv[i], O_RDONLY)) < 0) {
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

целое matchhere(char *, char *);
целое matchstar(целое, char *, char *);

целое
match(char *re, char *text)
{
  если (re[0] == '^')
    вернуть matchhere(re + 1, text);
  do { // must look at empty string
    если (matchhere(re, text))
      вернуть 1;
  } пока (*text++ != '\0');
  вернуть 0;
}

// matchhere: search for re at beginning of text
целое
matchhere(char *re, char *text)
{
  если (re[0] == '\0')
    вернуть 1;
  если (re[1] == '*')
    вернуть matchstar(re[0], re + 2, text);
  если (re[0] == '$' && re[1] == '\0')
    вернуть *text == '\0';
  если (*text != '\0' && (re[0] == '.' || re[0] == *text))
    вернуть matchhere(re + 1, text + 1);
  вернуть 0;
}

// matchstar: search for c*re at beginning of text
целое
matchstar(целое c, char *re, char *text)
{
  do { // a * matches zero or more instances
    если (matchhere(re, text))
      вернуть 1;
  } пока (*text != '\0' && (*text++ == c || c == '.'));
  вернуть 0;
}
