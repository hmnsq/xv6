// Simple grep.  Only supports ^ . * $ operators.

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

અક્ષર buf[1024];
પૂર્ણાંક match(અક્ષર *, અક્ષર *);

શૂન્ય
grep(અક્ષર *pattern, પૂર્ણાંક fd)
{
  પૂર્ણાંક n, m;
  અક્ષર *p, *q;

  m = 0;
  જ્યારે ((n = પઠ(fd, buf + m, કદ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    જ્યારે ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      જો (match(pattern, p)) {
        *q = '\n';
        લિખ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    જો (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક fd, i;
  અક્ષર *pattern;

  જો (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  જો (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  માટે (i = 2; i < argc; i++) {
    જો ((fd = open(argv[i], O_RDONLY)) < 0) {
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

પૂર્ણાંક matchhere(અક્ષર *, અક્ષર *);
પૂર્ણાંક matchstar(પૂર્ણાંક, અક્ષર *, અક્ષર *);

પૂર્ણાંક
match(અક્ષર *re, અક્ષર *text)
{
  જો (re[0] == '^')
    પાછા matchhere(re + 1, text);
  કરો { // must look at empty string
    જો (matchhere(re, text))
      પાછા 1;
  } જ્યારે (*text++ != '\0');
  પાછા 0;
}

// matchhere: search for re at beginning of text
પૂર્ણાંક
matchhere(અક્ષર *re, અક્ષર *text)
{
  જો (re[0] == '\0')
    પાછા 1;
  જો (re[1] == '*')
    પાછા matchstar(re[0], re + 2, text);
  જો (re[0] == '$' && re[1] == '\0')
    પાછા *text == '\0';
  જો (*text != '\0' && (re[0] == '.' || re[0] == *text))
    પાછા matchhere(re + 1, text + 1);
  પાછા 0;
}

// matchstar: search for c*re at beginning of text
પૂર્ણાંક
matchstar(પૂર્ણાંક c, અક્ષર *re, અક્ષર *text)
{
  કરો { // a * matches zero or more instances
    જો (matchhere(re, text))
      પાછા 1;
  } જ્યારે (*text != '\0' && (*text++ == c || c == '.'));
  પાછા 0;
}
