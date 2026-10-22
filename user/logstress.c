#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

ଅକ୍ଷର buf[BUFSZ];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, n;
  ଗଣନା { N = 250, SZ = 2000 };

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < argc; i++) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid1 = fork();
    ଯଦି (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    ଯଦି (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ଯଦି (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      ପାଇଁ (i = 0; i < N; i++) {
        ଯଦି ((n = ଲିଖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatus;
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < argc; i++) {
    wait(&xstatus);
    ଯଦି (xstatus != 0)
      exit(xstatus);
  }
  ଫେରନ୍ତୁ 0;
}
