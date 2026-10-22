// Simple grep.  Only supports ^ . * $ operators.

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "user/user.h"

எழுத்து buf[1024];
முழு match(எழுத்து *, எழுத்து *);

வெறுமை
grep(எழுத்து *pattern, முழு fd)
{
  முழு n, m;
  எழுத்து *p, *q;

  m = 0;
  போது ((n = பட(fd, buf + m, அளவு(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    போது ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      எனில் (match(pattern, p)) {
        *q = '\n';
        லிக(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    எனில் (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு fd, i;
  எழுத்து *pattern;

  எனில் (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  எனில் (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  க்கு (i = 2; i < argc; i++) {
    எனில் ((fd = open(argv[i], O_RDONLY)) < 0) {
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

முழு matchhere(எழுத்து *, எழுத்து *);
முழு matchstar(முழு, எழுத்து *, எழுத்து *);

முழு
match(எழுத்து *re, எழுத்து *text)
{
  எனில் (re[0] == '^')
    திரும்பு matchhere(re + 1, text);
  செய் { // must look at empty string
    எனில் (matchhere(re, text))
      திரும்பு 1;
  } போது (*text++ != '\0');
  திரும்பு 0;
}

// matchhere: search for re at beginning of text
முழு
matchhere(எழுத்து *re, எழுத்து *text)
{
  எனில் (re[0] == '\0')
    திரும்பு 1;
  எனில் (re[1] == '*')
    திரும்பு matchstar(re[0], re + 2, text);
  எனில் (re[0] == '$' && re[1] == '\0')
    திரும்பு *text == '\0';
  எனில் (*text != '\0' && (re[0] == '.' || re[0] == *text))
    திரும்பு matchhere(re + 1, text + 1);
  திரும்பு 0;
}

// matchstar: search for c*re at beginning of text
முழு
matchstar(முழு c, எழுத்து *re, எழுத்து *text)
{
  செய் { // a * matches zero or more instances
    எனில் (matchhere(re, text))
      திரும்பு 1;
  } போது (*text != '\0' && (*text++ == c || c == '.'));
  திரும்பு 0;
}
