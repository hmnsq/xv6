// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
puur_naa_nka match(var_na *, var_na *);

void
grep(var_na *pattern, puur_naa_nka fd)
{
  puur_naa_nka n, m;
  var_na *p, *q;

  m = 0;
  ᱡᱚᱠᱷᱚᱱ ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ᱡᱚᱠᱷᱚᱱ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ᱡᱩᱫᱤ (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ᱡᱩᱫᱤ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka fd, i;
  var_na *pattern;

  ᱡᱩᱫᱤ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ᱡᱩᱫᱤ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  for (i = 2; i < argc; i++) {
    ᱡᱩᱫᱤ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

puur_naa_nka matchhere(var_na *, var_na *);
puur_naa_nka matchstar(puur_naa_nka, var_na *, var_na *);

puur_naa_nka
match(var_na *re, var_na *text)
{
  ᱡᱩᱫᱤ (re[0] == '^')
    ᱨᱩᱣᱟᱹᱲ matchhere(re + 1, text);
  do { // must look at empty string
    ᱡᱩᱫᱤ (matchhere(re, text))
      ᱨᱩᱣᱟᱹᱲ 1;
  } ᱡᱚᱠᱷᱚᱱ (*text++ != '\0');
  ᱨᱩᱣᱟᱹᱲ 0;
}

// matchhere: search for re at beginning of text
puur_naa_nka
matchhere(var_na *re, var_na *text)
{
  ᱡᱩᱫᱤ (re[0] == '\0')
    ᱨᱩᱣᱟᱹᱲ 1;
  ᱡᱩᱫᱤ (re[1] == '*')
    ᱨᱩᱣᱟᱹᱲ matchstar(re[0], re + 2, text);
  ᱡᱩᱫᱤ (re[0] == '$' && re[1] == '\0')
    ᱨᱩᱣᱟᱹᱲ *text == '\0';
  ᱡᱩᱫᱤ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ᱨᱩᱣᱟᱹᱲ matchhere(re + 1, text + 1);
  ᱨᱩᱣᱟᱹᱲ 0;
}

// matchstar: search for c*re at beginning of text
puur_naa_nka
matchstar(puur_naa_nka c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    ᱡᱩᱫᱤ (matchhere(re, text))
      ᱨᱩᱣᱟᱹᱲ 1;
  } ᱡᱚᱠᱷᱚᱱ (*text != '\0' && (*text++ == c || c == '.'));
  ᱨᱩᱣᱟᱹᱲ 0;
}
