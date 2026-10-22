#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

અક્ષર buf[512];

શૂન્ય
cat(પૂર્ણાંક fd)
{
  પૂર્ણાંક n;

  જ્યારે ((n = પઠ(fd, buf, કદ(buf))) > 0) {
    જો (લિખ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  જો (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક fd, i;

  જો (argc <= 1) {
    cat(0);
    exit(0);
  }

  માટે (i = 1; i < argc; i++) {
    જો ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
