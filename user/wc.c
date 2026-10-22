#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

અક્ષર buf[512];

શૂન્ય
wc(પૂર્ણાંક fd, અક્ષર *name)
{
  પૂર્ણાંક i, n;
  પૂર્ણાંક l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  જ્યારે ((n = પઠ(fd, buf, કદ(buf))) > 0) {
    માટે (i = 0; i < n; i++) {
      c++;
      જો (buf[i] == '\n')
        l++;
      જો (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      નહીંતર જો (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  જો (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક fd, i;

  જો (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  માટે (i = 1; i < argc; i++) {
    જો ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
