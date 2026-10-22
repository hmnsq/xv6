#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

ଅକ୍ଷର buf[512];

ଶୂନ୍ୟ
wc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର *name)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, n;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ଯେତେବେଳେ ((n = ପଠ(fd, buf, ଆକାର(buf))) > 0) {
    ପାଇଁ (i = 0; i < n; i++) {
      c++;
      ଯଦି (buf[i] == '\n')
        l++;
      ଯଦି (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      ଅନ୍ୟଥା ଯଦି (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  ଯଦି (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, i;

  ଯଦି (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  ପାଇଁ (i = 1; i < argc; i++) {
    ଯଦି ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
