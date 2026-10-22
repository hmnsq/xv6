// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
цео match(char *, char *);

void
grep(char *pattern, цео fd)
{
  цео n, m;
  char *p, *q;

  m = 0;
  док ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    док ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ако (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ако (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

цео
main(цео argc, char *argv[])
{
  цео fd, i;
  char *pattern;

  ако (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ако (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  за (i = 2; i < argc; i++) {
    ако ((fd = open(argv[i], O_RDONLY)) < 0) {
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

цео matchhere(char *, char *);
цео matchstar(цео, char *, char *);

цео
match(char *re, char *text)
{
  ако (re[0] == '^')
    врати matchhere(re + 1, text);
  do { // must look at empty string
    ако (matchhere(re, text))
      врати 1;
  } док (*text++ != '\0');
  врати 0;
}

// matchhere: search for re at beginning of text
цео
matchhere(char *re, char *text)
{
  ако (re[0] == '\0')
    врати 1;
  ако (re[1] == '*')
    врати matchstar(re[0], re + 2, text);
  ако (re[0] == '$' && re[1] == '\0')
    врати *text == '\0';
  ако (*text != '\0' && (re[0] == '.' || re[0] == *text))
    врати matchhere(re + 1, text + 1);
  врати 0;
}

// matchstar: search for c*re at beginning of text
цео
matchstar(цео c, char *re, char *text)
{
  do { // a * matches zero or more instances
    ако (matchhere(re, text))
      врати 1;
  } док (*text != '\0' && (*text++ == c || c == '.'));
  врати 0;
}
