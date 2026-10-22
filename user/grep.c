// Simple grep.  Only supports ^ . * $ operators.

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

বর্ণ buf[1024];
পূর্ণাঙ্ক match(বর্ণ *, বর্ণ *);

শূন্য
grep(বর্ণ *pattern, পূর্ণাঙ্ক fd)
{
  পূর্ণাঙ্ক n, m;
  বর্ণ *p, *q;

  m = 0;
  যাবত্ ((n = পঠ(fd, buf + m, পরিমাণ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    যাবত্ ((q = strchr(p, '\n')) != 0) {
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

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ *argv[])
{
  পূর্ণাঙ্ক fd, i;
  বর্ণ *pattern;

  যদি (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  যদি (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  প্রতি (i = 2; i < argc; i++) {
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

পূর্ণাঙ্ক matchhere(বর্ণ *, বর্ণ *);
পূর্ণাঙ্ক matchstar(পূর্ণাঙ্ক, বর্ণ *, বর্ণ *);

পূর্ণাঙ্ক
match(বর্ণ *re, বর্ণ *text)
{
  যদি (re[0] == '^')
    প্রত্যাগম matchhere(re + 1, text);
  কুরু { // must look at empty string
    যদি (matchhere(re, text))
      প্রত্যাগম 1;
  } যাবত্ (*text++ != '\0');
  প্রত্যাগম 0;
}

// matchhere: search for re at beginning of text
পূর্ণাঙ্ক
matchhere(বর্ণ *re, বর্ণ *text)
{
  যদি (re[0] == '\0')
    প্রত্যাগম 1;
  যদি (re[1] == '*')
    প্রত্যাগম matchstar(re[0], re + 2, text);
  যদি (re[0] == '$' && re[1] == '\0')
    প্রত্যাগম *text == '\0';
  যদি (*text != '\0' && (re[0] == '.' || re[0] == *text))
    প্রত্যাগম matchhere(re + 1, text + 1);
  প্রত্যাগম 0;
}

// matchstar: search for c*re at beginning of text
পূর্ণাঙ্ক
matchstar(পূর্ণাঙ্ক c, বর্ণ *re, বর্ণ *text)
{
  কুরু { // a * matches zero or more instances
    যদি (matchhere(re, text))
      প্রত্যাগম 1;
  } যাবত্ (*text != '\0' && (*text++ == c || c == '.'));
  প্রত্যাগম 0;
}
