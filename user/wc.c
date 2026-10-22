#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

అక్షరం buf[512];

శూన్య
wc(పూర్ణాంకం fd, అక్షరం *name)
{
  పూర్ణాంకం i, n;
  పూర్ణాంకం l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  అయితే ((n = పఠ(fd, buf, పరిమాణం(buf))) > 0) {
    కొరకు (i = 0; i < n; i++) {
      c++;
      ఒకవేళ (buf[i] == '\n')
        l++;
      ఒకవేళ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      లేకపోతే ఒకవేళ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ఒకవేళ (n < 0) {
    రాయి("wc: read error\n");
    నిష్క్రమించు(1);
  }
  రాయి("%d %d %d %s\n", l, w, c, name);
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం fd, i;

  ఒకవేళ (argc <= 1) {
    wc(0, "");
    నిష్క్రమించు(0);
  }

  కొరకు (i = 1; i < argc; i++) {
    ఒకవేళ ((fd = open(argv[i], O_RDONLY)) < 0) {
      రాయి("wc: cannot open %s\n", argv[i]);
      నిష్క్రమించు(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  నిష్క్రమించు(0);
}
