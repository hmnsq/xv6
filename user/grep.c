// Simple grep.  Only supports ^ . * $ operators.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ವರ್ಣ buf[1024];
ಪೂರ್ಣಾಙ್ಕ match(ವರ್ಣ *, ವರ್ಣ *);

ಶೂನ್ಯ
grep(ವರ್ಣ *pattern, ಪೂರ್ಣಾಙ್ಕ fd)
{
  ಪೂರ್ಣಾಙ್ಕ n, m;
  ವರ್ಣ *p, *q;

  m = 0;
  ಯಾವತ್ ((n = ಪಠ(fd, buf + m, ಪರಿಮಾಣ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ಯಾವತ್ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ಯದಿ (match(pattern, p)) {
        *q = '\n';
        ಲಿಖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ಯದಿ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ fd, i;
  ವರ್ಣ *pattern;

  ಯದಿ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ಯದಿ (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  ಪ್ರತಿ (i = 2; i < argc; i++) {
    ಯದಿ ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ಪೂರ್ಣಾಙ್ಕ matchhere(ವರ್ಣ *, ವರ್ಣ *);
ಪೂರ್ಣಾಙ್ಕ matchstar(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ *, ವರ್ಣ *);

ಪೂರ್ಣಾಙ್ಕ
match(ವರ್ಣ *re, ವರ್ಣ *text)
{
  ಯದಿ (re[0] == '^')
    ಪ್ರತ್ಯಾಗಮ matchhere(re + 1, text);
  ಕುರು { // must look at empty string
    ಯದಿ (matchhere(re, text))
      ಪ್ರತ್ಯಾಗಮ 1;
  } ಯಾವತ್ (*text++ != '\0');
  ಪ್ರತ್ಯಾಗಮ 0;
}

// matchhere: search for re at beginning of text
ಪೂರ್ಣಾಙ್ಕ
matchhere(ವರ್ಣ *re, ವರ್ಣ *text)
{
  ಯದಿ (re[0] == '\0')
    ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ (re[1] == '*')
    ಪ್ರತ್ಯಾಗಮ matchstar(re[0], re + 2, text);
  ಯದಿ (re[0] == '$' && re[1] == '\0')
    ಪ್ರತ್ಯಾಗಮ *text == '\0';
  ಯದಿ (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ಪ್ರತ್ಯಾಗಮ matchhere(re + 1, text + 1);
  ಪ್ರತ್ಯಾಗಮ 0;
}

// matchstar: search for c*re at beginning of text
ಪೂರ್ಣಾಙ್ಕ
matchstar(ಪೂರ್ಣಾಙ್ಕ c, ವರ್ಣ *re, ವರ್ಣ *text)
{
  ಕುರು { // a * matches zero or more instances
    ಯದಿ (matchhere(re, text))
      ಪ್ರತ್ಯಾಗಮ 1;
  } ಯಾವತ್ (*text != '\0' && (*text++ == c || c == '.'));
  ಪ್ರತ್ಯಾಗಮ 0;
}
