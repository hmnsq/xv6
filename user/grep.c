// Simple grep.  Only supports ^ . * $ operators.

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

অক্ষর buf[1024];
পূর্ণসংখ্যা match(অক্ষর *, অক্ষর *);

শূন্য
grep(অক্ষর *pattern, পূর্ণসংখ্যা fd)
{
  পূর্ণসংখ্যা n, m;
  অক্ষর *p, *q;

  m = 0;
  যখন ((n = পঠ(fd, buf + m, আকার(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    যখন ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      যদি (match(pattern, p)) {
        *q = '\n';
        লিখ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    যদি (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা fd, i;
  অক্ষর *pattern;

  যদি (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  যদি (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  জন্য (i = 2; i < argc; i++) {
    যদি ((fd = open(argv[i], O_RDONLY)) < 0) {
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

পূর্ণসংখ্যা matchhere(অক্ষর *, অক্ষর *);
পূর্ণসংখ্যা matchstar(পূর্ণসংখ্যা, অক্ষর *, অক্ষর *);

পূর্ণসংখ্যা
match(অক্ষর *re, অক্ষর *text)
{
  যদি (re[0] == '^')
    ফেরত matchhere(re + 1, text);
  করো { // must look at empty string
    যদি (matchhere(re, text))
      ফেরত 1;
  } যখন (*text++ != '\0');
  ফেরত 0;
}

// matchhere: search for re at beginning of text
পূর্ণসংখ্যা
matchhere(অক্ষর *re, অক্ষর *text)
{
  যদি (re[0] == '\0')
    ফেরত 1;
  যদি (re[1] == '*')
    ফেরত matchstar(re[0], re + 2, text);
  যদি (re[0] == '$' && re[1] == '\0')
    ফেরত *text == '\0';
  যদি (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ফেরত matchhere(re + 1, text + 1);
  ফেরত 0;
}

// matchstar: search for c*re at beginning of text
পূর্ণসংখ্যা
matchstar(পূর্ণসংখ্যা c, অক্ষর *re, অক্ষর *text)
{
  করো { // a * matches zero or more instances
    যদি (matchhere(re, text))
      ফেরত 1;
  } যখন (*text != '\0' && (*text++ == c || c == '.'));
  ফেরত 0;
}
