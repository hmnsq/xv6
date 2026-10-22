// Simple grep.  Only supports ^ . * $ operators.

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "user/user.h"

حرف buf[1024];
صحیح مطابقت(حرف *, حرف *);

خالی
grep(حرف *نمونہ, صحیح fd)
{
  صحیح n, m;
  حرف *p, *q;

  m = 0;
  جب ((n = اقرأ(fd, buf + m, حجم(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    جب ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      اگر (مطابقت(نمونہ, p)) {
        *q = '\n';
        اكتب(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    اگر (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح fd, i;
  حرف *نمونہ;

  اگر (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    باہر_نکلو(1);
  }
  نمونہ = argv[1];

  اگر (argc <= 2) {
    grep(نمونہ, 0);
    باہر_نکلو(0);
  }

  برائے (i = 2; i < argc; i++) {
    اگر ((fd = open(argv[i], O_RDONLY)) < 0) {
      لکھو("grep: cannot open %s\n", argv[i]);
      باہر_نکلو(1);
    }
    grep(نمونہ, fd);
    close(fd);
  }
  باہر_نکلو(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

صحیح matchhere(حرف *, حرف *);
صحیح matchstar(صحیح, حرف *, حرف *);

صحیح
مطابقت(حرف *re, حرف *متن)
{
  اگر (re[0] == '^')
    واپس matchhere(re + 1, متن);
  کرو { // must look at empty string
    اگر (matchhere(re, متن))
      واپس 1;
  } جب (*متن++ != '\0');
  واپس 0;
}

// matchhere: search for re at beginning of text
صحیح
matchhere(حرف *re, حرف *متن)
{
  اگر (re[0] == '\0')
    واپس 1;
  اگر (re[1] == '*')
    واپس matchstar(re[0], re + 2, متن);
  اگر (re[0] == '$' && re[1] == '\0')
    واپس *متن == '\0';
  اگر (*متن != '\0' && (re[0] == '.' || re[0] == *متن))
    واپس matchhere(re + 1, متن + 1);
  واپس 0;
}

// matchstar: search for c*re at beginning of text
صحیح
matchstar(صحیح c, حرف *re, حرف *متن)
{
  کرو { // a * matches zero or more instances
    اگر (matchhere(re, متن))
      واپس 1;
  } جب (*متن != '\0' && (*متن++ == c || c == '.'));
  واپس 0;
}
