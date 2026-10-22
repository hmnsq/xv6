// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
정수 match(char *, char *);

void
grep(char *pattern, 정수 fd)
{
  정수 n, m;
  char *p, *q;

  m = 0;
  동안 ((n = read(fd, buf + m, sizeof(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    동안 ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      만약 (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    만약 (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

정수
main(정수 argc, char *argv[])
{
  정수 fd, i;
  char *pattern;

  만약 (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  만약 (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  위해 (i = 2; i < argc; i++) {
    만약 ((fd = open(argv[i], O_RDONLY)) < 0) {
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

정수 matchhere(char *, char *);
정수 matchstar(정수, char *, char *);

정수
match(char *re, char *text)
{
  만약 (re[0] == '^')
    반환 matchhere(re + 1, text);
  do { // must look at empty string
    만약 (matchhere(re, text))
      반환 1;
  } 동안 (*text++ != '\0');
  반환 0;
}

// matchhere: search for re at beginning of text
정수
matchhere(char *re, char *text)
{
  만약 (re[0] == '\0')
    반환 1;
  만약 (re[1] == '*')
    반환 matchstar(re[0], re + 2, text);
  만약 (re[0] == '$' && re[1] == '\0')
    반환 *text == '\0';
  만약 (*text != '\0' && (re[0] == '.' || re[0] == *text))
    반환 matchhere(re + 1, text + 1);
  반환 0;
}

// matchstar: search for c*re at beginning of text
정수
matchstar(정수 c, char *re, char *text)
{
  do { // a * matches zero or more instances
    만약 (matchhere(re, text))
      반환 1;
  } 동안 (*text != '\0' && (*text++ == c || c == '.'));
  반환 0;
}
