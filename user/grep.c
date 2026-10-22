// Simple grep.  Only supports ^ . * $ operators.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಅಕ್ಷರ buf[1024];
ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ(ಅಕ್ಷರ *, ಅಕ್ಷರ *);

ಶೂನ್ಯ
grep(ಅಕ್ಷರ *ನಮೂನೆ, ಪೂರ್ಣಾಂಕ fd)
{
  ಪೂರ್ಣಾಂಕ n, m;
  ಅಕ್ಷರ *p, *q;

  m = 0;
  ಆಗ ((n = ಪಠ(fd, buf + m, ಗಾತ್ರ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ಆಗ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ಆದರೆ (ಹೊಂದಿಕೆ(ನಮೂನೆ, p)) {
        *q = '\n';
        ಲಿಖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ಆದರೆ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ fd, i;
  ಅಕ್ಷರ *ನಮೂನೆ;

  ಆದರೆ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    ನಿರ್ಗಮಿಸು(1);
  }
  ನಮೂನೆ = argv[1];

  ಆದರೆ (argc <= 2) {
    grep(ನಮೂನೆ, 0);
    ನಿರ್ಗಮಿಸು(0);
  }

  ಗಾಗಿ (i = 2; i < argc; i++) {
    ಆದರೆ ((fd = open(argv[i], O_RDONLY)) < 0) {
      ಬರೆ("grep: cannot open %s\n", argv[i]);
      ನಿರ್ಗಮಿಸು(1);
    }
    grep(ನಮೂನೆ, fd);
    close(fd);
  }
  ನಿರ್ಗಮಿಸು(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

ಪೂರ್ಣಾಂಕ matchhere(ಅಕ್ಷರ *, ಅಕ್ಷರ *);
ಪೂರ್ಣಾಂಕ matchstar(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ *, ಅಕ್ಷರ *);

ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ(ಅಕ್ಷರ *re, ಅಕ್ಷರ *ಪಠ್ಯ)
{
  ಆದರೆ (re[0] == '^')
    ಹಿಂತಿರುಗಿ matchhere(re + 1, ಪಠ್ಯ);
  ಮಾಡು { // must look at empty string
    ಆದರೆ (matchhere(re, ಪಠ್ಯ))
      ಹಿಂತಿರುಗಿ 1;
  } ಆಗ (*ಪಠ್ಯ++ != '\0');
  ಹಿಂತಿರುಗಿ 0;
}

// matchhere: search for re at beginning of text
ಪೂರ್ಣಾಂಕ
matchhere(ಅಕ್ಷರ *re, ಅಕ್ಷರ *ಪಠ್ಯ)
{
  ಆದರೆ (re[0] == '\0')
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ (re[1] == '*')
    ಹಿಂತಿರುಗಿ matchstar(re[0], re + 2, ಪಠ್ಯ);
  ಆದರೆ (re[0] == '$' && re[1] == '\0')
    ಹಿಂತಿರುಗಿ *ಪಠ್ಯ == '\0';
  ಆದರೆ (*ಪಠ್ಯ != '\0' && (re[0] == '.' || re[0] == *ಪಠ್ಯ))
    ಹಿಂತಿರುಗಿ matchhere(re + 1, ಪಠ್ಯ + 1);
  ಹಿಂತಿರುಗಿ 0;
}

// matchstar: search for c*re at beginning of text
ಪೂರ್ಣಾಂಕ
matchstar(ಪೂರ್ಣಾಂಕ c, ಅಕ್ಷರ *re, ಅಕ್ಷರ *ಪಠ್ಯ)
{
  ಮಾಡು { // a * matches zero or more instances
    ಆದರೆ (matchhere(re, ಪಠ್ಯ))
      ಹಿಂತಿರುಗಿ 1;
  } ಆಗ (*ಪಠ್ಯ != '\0' && (*ಪಠ್ಯ++ == c || c == '.'));
  ಹಿಂತಿರುಗಿ 0;
}
