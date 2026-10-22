#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

वर्ण buf[512];

शून्य
cat(पूर्णाङ्क fd)
{
  पूर्णाङ्क n;

  जाव ((n = पठ(fd, buf, परिमाण(buf))) > 0) {
    जइ (लिख(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  जइ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;

  जइ (argc <= 1) {
    cat(0);
    exit(0);
  }

  प्रति (i = 1; i < argc; i++) {
    जइ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
