#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[512];

शून्य
cat(पूर्णाङ्क fd)
{
  पूर्णाङ्क n;

  यावत् ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    यदि (लिख(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      निर्गच्छ(1);
    }
  }
  यदि (n < 0) {
    fprintf(2, "cat: read error\n");
    निर्गच्छ(1);
  }
}

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  यदि (argc <= 1) {
    cat(0);
    निर्गच्छ(0);
  }

  प्रति (i = 1; i < argc; i++) {
    यदि ((fd = उद्घाटय(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      निर्गच्छ(1);
    }
    cat(fd);
    close(fd);
  }
  निर्गच्छ(0);
}
