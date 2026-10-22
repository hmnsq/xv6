// Simple grep.  Only supports ^ . * $ operators.

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਅੱਖਰ buf[1024];
ਪੂਰਨ ਮੇਲ(ਅੱਖਰ *, ਅੱਖਰ *);

ਖਾਲੀ
grep(ਅੱਖਰ *ਨਮੂਨਾ, ਪੂਰਨ fd)
{
  ਪੂਰਨ n, m;
  ਅੱਖਰ *p, *q;

  m = 0;
  ਜਦੋਂ ((n = ਪਠ(fd, buf + m, ਆਕਾਰ(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ਜਦੋਂ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ਜੇ (ਮੇਲ(ਨਮੂਨਾ, p)) {
        *q = '\n';
        ਲਿਖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ਜੇ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ fd, i;
  ਅੱਖਰ *ਨਮੂਨਾ;

  ਜੇ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }
  ਨਮੂਨਾ = argv[1];

  ਜੇ (argc <= 2) {
    grep(ਨਮੂਨਾ, 0);
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਲਈ (i = 2; i < argc; i++) {
    ਜੇ ((fd = open(argv[i], O_RDONLY)) < 0) {
      ਲਿਖੋ("grep: cannot open %s\n", argv[i]);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    grep(ਨਮੂਨਾ, fd);
    close(fd);
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

ਪੂਰਨ matchhere(ਅੱਖਰ *, ਅੱਖਰ *);
ਪੂਰਨ matchstar(ਪੂਰਨ, ਅੱਖਰ *, ਅੱਖਰ *);

ਪੂਰਨ
ਮੇਲ(ਅੱਖਰ *re, ਅੱਖਰ *ਪਾਠ)
{
  ਜੇ (re[0] == '^')
    ਵਾਪਸ matchhere(re + 1, ਪਾਠ);
  ਕਰੋ { // must look at empty string
    ਜੇ (matchhere(re, ਪਾਠ))
      ਵਾਪਸ 1;
  } ਜਦੋਂ (*ਪਾਠ++ != '\0');
  ਵਾਪਸ 0;
}

// matchhere: search for re at beginning of text
ਪੂਰਨ
matchhere(ਅੱਖਰ *re, ਅੱਖਰ *ਪਾਠ)
{
  ਜੇ (re[0] == '\0')
    ਵਾਪਸ 1;
  ਜੇ (re[1] == '*')
    ਵਾਪਸ matchstar(re[0], re + 2, ਪਾਠ);
  ਜੇ (re[0] == '$' && re[1] == '\0')
    ਵਾਪਸ *ਪਾਠ == '\0';
  ਜੇ (*ਪਾਠ != '\0' && (re[0] == '.' || re[0] == *ਪਾਠ))
    ਵਾਪਸ matchhere(re + 1, ਪਾਠ + 1);
  ਵਾਪਸ 0;
}

// matchstar: search for c*re at beginning of text
ਪੂਰਨ
matchstar(ਪੂਰਨ c, ਅੱਖਰ *re, ਅੱਖਰ *ਪਾਠ)
{
  ਕਰੋ { // a * matches zero or more instances
    ਜੇ (matchhere(re, ਪਾਠ))
      ਵਾਪਸ 1;
  } ਜਦੋਂ (*ਪਾਠ != '\0' && (*ਪਾਠ++ == c || c == '.'));
  ਵਾਪਸ 0;
}
