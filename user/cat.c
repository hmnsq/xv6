#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

అక్షరం buf[512];

శూన్య
cat(పూర్ణాంకం fd)
{
  పూర్ణాంకం n;

  అయితే ((n = పఠ(fd, buf, పరిమాణం(buf))) > 0) {
    ఒకవేళ (లిఖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      నిష్క్రమించు(1);
    }
  }
  ఒకవేళ (n < 0) {
    fprintf(2, "cat: read error\n");
    నిష్క్రమించు(1);
  }
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం fd, i;

  ఒకవేళ (argc <= 1) {
    cat(0);
    నిష్క్రమించు(0);
  }

  కొరకు (i = 1; i < argc; i++) {
    ఒకవేళ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      నిష్క్రమించు(1);
    }
    cat(fd);
    close(fd);
  }
  నిష్క్రమించు(0);
}
