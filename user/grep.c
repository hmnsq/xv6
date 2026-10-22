// Simple grep.  Only supports ^ . * $ operators.

#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "user/user.h"

ৱর্ণ buf[1024];
পূর্ণাঙ্ক match(ৱর্ণ *, ৱর্ণ *);

শূন্য
grep(ৱর্ণ *pattern, পূর্ণাঙ্ক fd)
{
  পূর্ণাঙ্ক n, m;
  ৱর্ণ *p, *q;

  m = 0;
  যেতিয়ালৈ ((n = পঠ(fd, buf + m, পরিমাণ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    যেতিয়ালৈ ((q = strchr(p, '\n')) != 0) {
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
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক fd, i;
  ৱর্ণ *pattern;

  যদি (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  যদি (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  বাবে (i = 2; i < argc; i++) {
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

পূর্ণাঙ্ক matchhere(ৱর্ণ *, ৱর্ণ *);
পূর্ণাঙ্ক matchstar(পূর্ণাঙ্ক, ৱর্ণ *, ৱর্ণ *);

পূর্ণাঙ্ক
match(ৱর্ণ *re, ৱর্ণ *text)
{
  যদি (re[0] == '^')
    ঘূৰাই matchhere(re + 1, text);
  কৰা { // must look at empty string
    যদি (matchhere(re, text))
      ঘূৰাই 1;
  } যেতিয়ালৈ (*text++ != '\0');
  ঘূৰাই 0;
}

// matchhere: search for re at beginning of text
পূর্ণাঙ্ক
matchhere(ৱর্ণ *re, ৱর্ণ *text)
{
  যদি (re[0] == '\0')
    ঘূৰাই 1;
  যদি (re[1] == '*')
    ঘূৰাই matchstar(re[0], re + 2, text);
  যদি (re[0] == '$' && re[1] == '\0')
    ঘূৰাই *text == '\0';
  যদি (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ঘূৰাই matchhere(re + 1, text + 1);
  ঘূৰাই 0;
}

// matchstar: search for c*re at beginning of text
পূর্ণাঙ্ক
matchstar(পূর্ণাঙ্ক c, ৱর্ণ *re, ৱর্ণ *text)
{
  কৰা { // a * matches zero or more instances
    যদি (matchhere(re, text))
      ঘূৰাই 1;
  } যেতিয়ালৈ (*text != '\0' && (*text++ == c || c == '.'));
  ঘূৰাই 0;
}
