// Simple grep.  Only supports ^ . * $ operators.

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "user/user.h"

അക്ഷരം buf[1024];
പൂർണ്ണസംഖ്യ match(അക്ഷരം *, അക്ഷരം *);

ശൂന്യം
grep(അക്ഷരം *pattern, പൂർണ്ണസംഖ്യ fd)
{
  പൂർണ്ണസംഖ്യ n, m;
  അക്ഷരം *p, *q;

  m = 0;
  ആകുമ്പോൾ ((n = പഠ(fd, buf + m, വലുപ്പം(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ആകുമ്പോൾ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      എങ്കിൽ (match(pattern, p)) {
        *q = '\n';
        ലിഖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    എങ്കിൽ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ fd, i;
  അക്ഷരം *pattern;

  എങ്കിൽ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  എങ്കിൽ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  വേണ്ടി (i = 2; i < argc; i++) {
    എങ്കിൽ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

പൂർണ്ണസംഖ്യ matchhere(അക്ഷരം *, അക്ഷരം *);
പൂർണ്ണസംഖ്യ matchstar(പൂർണ്ണസംഖ്യ, അക്ഷരം *, അക്ഷരം *);

പൂർണ്ണസംഖ്യ
match(അക്ഷരം *re, അക്ഷരം *text)
{
  എങ്കിൽ (re[0] == '^')
    തിരികെ matchhere(re + 1, text);
  ചെയ്യുക { // must look at empty string
    എങ്കിൽ (matchhere(re, text))
      തിരികെ 1;
  } ആകുമ്പോൾ (*text++ != '\0');
  തിരികെ 0;
}

// matchhere: search for re at beginning of text
പൂർണ്ണസംഖ്യ
matchhere(അക്ഷരം *re, അക്ഷരം *text)
{
  എങ്കിൽ (re[0] == '\0')
    തിരികെ 1;
  എങ്കിൽ (re[1] == '*')
    തിരികെ matchstar(re[0], re + 2, text);
  എങ്കിൽ (re[0] == '$' && re[1] == '\0')
    തിരികെ *text == '\0';
  എങ്കിൽ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    തിരികെ matchhere(re + 1, text + 1);
  തിരികെ 0;
}

// matchstar: search for c*re at beginning of text
പൂർണ്ണസംഖ്യ
matchstar(പൂർണ്ണസംഖ്യ c, അക്ഷരം *re, അക്ഷരം *text)
{
  ചെയ്യുക { // a * matches zero or more instances
    എങ്കിൽ (matchhere(re, text))
      തിരികെ 1;
  } ആകുമ്പോൾ (*text != '\0' && (*text++ == c || c == '.'));
  തിരികെ 0;
}
