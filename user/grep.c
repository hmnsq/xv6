// Simple grep.  Only supports ^ . * $ operators.

#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

ଅକ୍ଷର buf[1024];
ପୂର୍ଣ୍ଣସଂଖ୍ୟା match(ଅକ୍ଷର *, ଅକ୍ଷର *);

ଶୂନ୍ୟ
grep(ଅକ୍ଷର *pattern, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, m;
  ଅକ୍ଷର *p, *q;

  m = 0;
  ଯେତେବେଳେ ((n = ପଠ(fd, buf + m, ଆକାର(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    ଯେତେବେଳେ ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ଯଦି (match(pattern, p)) {
        *q = '\n';
        ଲିଖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ଯଦି (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, i;
  ଅକ୍ଷର *pattern;

  ଯଦି (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  pattern = argv[1];

  ଯଦି (argc <= 2) {
    grep(pattern, 0);
    exit(0);
  }

  ପାଇଁ (i = 2; i < argc; i++) {
    ଯଦି ((fd = open(argv[i], O_RDONLY)) < 0) {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା matchhere(ଅକ୍ଷର *, ଅକ୍ଷର *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା matchstar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର *, ଅକ୍ଷର *);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match(ଅକ୍ଷର *re, ଅକ୍ଷର *text)
{
  ଯଦି (re[0] == '^')
    ଫେରନ୍ତୁ matchhere(re + 1, text);
  କର { // must look at empty string
    ଯଦି (matchhere(re, text))
      ଫେରନ୍ତୁ 1;
  } ଯେତେବେଳେ (*text++ != '\0');
  ଫେରନ୍ତୁ 0;
}

// matchhere: search for re at beginning of text
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
matchhere(ଅକ୍ଷର *re, ଅକ୍ଷର *text)
{
  ଯଦି (re[0] == '\0')
    ଫେରନ୍ତୁ 1;
  ଯଦି (re[1] == '*')
    ଫେରନ୍ତୁ matchstar(re[0], re + 2, text);
  ଯଦି (re[0] == '$' && re[1] == '\0')
    ଫେରନ୍ତୁ *text == '\0';
  ଯଦି (*text != '\0' && (re[0] == '.' || re[0] == *text))
    ଫେରନ୍ତୁ matchhere(re + 1, text + 1);
  ଫେରନ୍ତୁ 0;
}

// matchstar: search for c*re at beginning of text
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
matchstar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ଅକ୍ଷର *re, ଅକ୍ଷର *text)
{
  କର { // a * matches zero or more instances
    ଯଦି (matchhere(re, text))
      ଫେରନ୍ତୁ 1;
  } ଯେତେବେଳେ (*text != '\0' && (*text++ == c || c == '.'));
  ଫେରନ୍ତୁ 0;
}
