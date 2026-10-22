// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
පූර්ණ_සංඛ්‍යා match(var_na *, var_na *);

void
grep(var_na *pattern, පූර්ණ_සංඛ්‍යා fd)
{
  පූර්ණ_සංඛ්‍යා n, m;
  var_na *p, *q;

  m = 0;
  අතරතුර ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    අතරතුර ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      නම් (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    නම් (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා fd, i;
  var_na *pattern;

  නම් (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  නම් (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  සඳහා (i = 2; i < argc; i++) {
    නම් ((fd = open(argv[i], O_RDONLY)) < 0) {
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

පූර්ණ_සංඛ්‍යා matchhere(var_na *, var_na *);
පූර්ණ_සංඛ්‍යා matchstar(පූර්ණ_සංඛ්‍යා, var_na *, var_na *);

පූර්ණ_සංඛ්‍යා
match(var_na *re, var_na *text)
{
  නම් (re[0] == '^')
    ආපසු matchhere(re + 1, text);
  do { // must look at empty string
    නම් (matchhere(re, text))
      ආපසු 1;
  } අතරතුර (*text++ != '\0');
  ආපසු 0;
}

// matchhere: search for re at beginning of text
පූර්ණ_සංඛ්‍යා
matchhere(var_na *re, var_na *text)
{
  නම් (re[0] == '\0')
    ආපසු 1;
  නම් (re[1] == '*')
    ආපසු matchstar(re[0], re + 2, text);
  නම් (re[0] == '$' && re[1] == '\0')
    ආපසු *text == '\0';
  නම් (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ආපසු matchhere(re + 1, text + 1);
  ආපසු 0;
}

// matchstar: search for c*re at beginning of text
පූර්ණ_සංඛ්‍යා
matchstar(පූර්ණ_සංඛ්‍යා c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    නම් (matchhere(re, text))
      ආපසු 1;
  } අතරතුර (*text != '\0' && (*text++ == c || c == '.'));
  ආපසු 0;
}
