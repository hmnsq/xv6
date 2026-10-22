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
  यावत् ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    प्रति (i = 0; i < n; i++) {
      c++;
      यदि (buf[i] == '\n')
        l++;
      यदि (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      अन्यथा यदि (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  यदि (n < 0) {
    प्रारूप_लिख("wc: read error\n");
    निर्गच्छ(1);
  }
  प्रारूप_लिख("%d %d %d %s\n", l, w, c, name);
}

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  यदि (argc <= 1) {
    wc(0, "");
    निर्गच्छ(0);
  }

  प्रति (i = 1; i < argc; i++) {
    यदि ((fd = उद्घाटय(argv[i], O_RDONLY)) < 0) {
      प्रारूप_लिख("wc: cannot open %s\n", argv[i]);
      निर्गच्छ(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  निर्गच्छ(0);
}
