// Simple grep.  Only supports ^ . * $ operators.

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

అక్షరం buf[1024];
పూర్ణాంకం సరిపోలిక(అక్షరం *, అక్షరం *);

శూన్య
grep(అక్షరం *నమూనా, పూర్ణాంకం fd)
{
  పూర్ణాంకం n, m;
  అక్షరం *p, *q;

  m = 0;
  అయితే ((n = పఠ(fd, buf + m, పరిమాణం(buf) - m - 1)) > 0) {
    m += n;
    buf[m] = '\0';
    p = buf;
    అయితే ((q = strchr(p, '\n')) != 0) {
      *q = 0;
      ఒకవేళ (సరిపోలిక(నమూనా, p)) {
        *q = '\n';
        లిఖ(1, p, q + 1 - p);
      }
      p = q + 1;
    }
    ఒకవేళ (m > 0) {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం fd, i;
  అక్షరం *నమూనా;

  ఒకవేళ (argc <= 1) {
    fprintf(2, "usage: grep pattern [file ...]\n");
    నిష్క్రమించు(1);
  }
  నమూనా = argv[1];

  ఒకవేళ (argc <= 2) {
    grep(నమూనా, 0);
    నిష్క్రమించు(0);
  }

  కొరకు (i = 2; i < argc; i++) {
    ఒకవేళ ((fd = open(argv[i], O_RDONLY)) < 0) {
      రాయి("grep: cannot open %s\n", argv[i]);
      నిష్క్రమించు(1);
    }
    grep(నమూనా, fd);
    close(fd);
  }
  నిష్క్రమించు(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

పూర్ణాంకం matchhere(అక్షరం *, అక్షరం *);
పూర్ణాంకం matchstar(పూర్ణాంకం, అక్షరం *, అక్షరం *);

పూర్ణాంకం
సరిపోలిక(అక్షరం *re, అక్షరం *పాఠ్యం)
{
  ఒకవేళ (re[0] == '^')
    తిరిగి matchhere(re + 1, పాఠ్యం);
  చేయి { // must look at empty string
    ఒకవేళ (matchhere(re, పాఠ్యం))
      తిరిగి 1;
  } అయితే (*పాఠ్యం++ != '\0');
  తిరిగి 0;
}

// matchhere: search for re at beginning of text
పూర్ణాంకం
matchhere(అక్షరం *re, అక్షరం *పాఠ్యం)
{
  ఒకవేళ (re[0] == '\0')
    తిరిగి 1;
  ఒకవేళ (re[1] == '*')
    తిరిగి matchstar(re[0], re + 2, పాఠ్యం);
  ఒకవేళ (re[0] == '$' && re[1] == '\0')
    తిరిగి *పాఠ్యం == '\0';
  ఒకవేళ (*పాఠ్యం != '\0' && (re[0] == '.' || re[0] == *పాఠ్యం))
    తిరిగి matchhere(re + 1, పాఠ్యం + 1);
  తిరిగి 0;
}

// matchstar: search for c*re at beginning of text
పూర్ణాంకం
matchstar(పూర్ణాంకం c, అక్షరం *re, అక్షరం *పాఠ్యం)
{
  చేయి { // a * matches zero or more instances
    ఒకవేళ (matchhere(re, పాఠ్యం))
      తిరిగి 1;
  } అయితే (*పాఠ్యం != '\0' && (*పాఠ్యం++ == c || c == '.'));
  తిరిగి 0;
}
