#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

ଅକ୍ଷର buf[512];

ଶୂନ୍ୟ
cat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

  ଯେତେବେଳେ ((n = ପଠ(fd, buf, ଆକାର(buf))) > 0) {
    ଯଦି (ଲିଖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  ଯଦି (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, i;

  ଯଦି (argc <= 1) {
    cat(0);
    exit(0);
  }

  ପାଇଁ (i = 1; i < argc; i++) {
    ଯଦି ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
