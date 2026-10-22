#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[512];

शून्य
wc(पूर्णाङ्क fd, वर्ण *name)
{
  पूर्णाङ्क i, n;
  पूर्णाङ्क l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  जाव ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    प्रति (i = 0; i < n; i++) {
      c++;
      जइ (buf[i] == '\n')
        l++;
      जइ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      अण्णहा जइ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  जइ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  जइ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  प्रति (i = 1; i < argc; i++) {
    जइ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
