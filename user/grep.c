// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

var_na buf[1024];
ຈຳນວນເຕັມ match(var_na *, var_na *);

void
grep(var_na *pattern, ຈຳນວນເຕັມ fd)
{
  ຈຳນວນເຕັມ n, m;
  var_na *p, *q;

  m = 0;
  ໃນຂະນະທີ່ ((n = read(fd, buf + m, parimaa_na(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ໃນຂະນະທີ່ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ຖ້າ (match(pattern, p)) {
        *q = '\n';
        write(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ຖ້າ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ fd, i;
  var_na *pattern;

  ຖ້າ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ຖ້າ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  ສຳລັບ (i = 2; i < argc; i++) {
    ຖ້າ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ຈຳນວນເຕັມ matchhere(var_na *, var_na *);
ຈຳນວນເຕັມ matchstar(ຈຳນວນເຕັມ, var_na *, var_na *);

ຈຳນວນເຕັມ
match(var_na *re, var_na *text)
{
  ຖ້າ (re[0] == '^')
    ສົ່ງຄືນ matchhere(re + 1, text);
  do { // must look at empty string
    ຖ້າ (matchhere(re, text))
      ສົ່ງຄືນ 1;
  } ໃນຂະນະທີ່ (*text++ != '\0');
  ສົ່ງຄືນ 0;
}

// matchhere: search for re at beginning of text
ຈຳນວນເຕັມ
matchhere(var_na *re, var_na *text)
{
  ຖ້າ (re[0] == '\0')
    ສົ່ງຄືນ 1;
  ຖ້າ (re[1] == '*')
    ສົ່ງຄືນ matchstar(re[0], re + 2, text);
  ຖ້າ (re[0] == '$' && re[1] == '\0')
    ສົ່ງຄືນ *text == '\0';
  ຖ້າ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ສົ່ງຄືນ matchhere(re + 1, text + 1);
  ສົ່ງຄືນ 0;
}

// matchstar: search for c*re at beginning of text
ຈຳນວນເຕັມ
matchstar(ຈຳນວນເຕັມ c, var_na *re, var_na *text)
{
  do { // a * matches zero or more instances
    ຖ້າ (matchhere(re, text))
      ສົ່ງຄືນ 1;
  } ໃນຂະນະທີ່ (*text != '\0' && (*text++ == c || c == '.'));
  ສົ່ງຄືນ 0;
}
