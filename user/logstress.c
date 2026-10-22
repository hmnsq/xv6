#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

অক্ষর buf[BUFSZ];

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
  পূর্ণসংখ্যা fd, n;
  গণনা { N = 250, SZ = 2000 };

  জন্য (পূর্ণসংখ্যা i = 1; i < argc; i++) {
    পূর্ণসংখ্যা pid1 = fork();
    যদি (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    যদি (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      যদি (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      জন্য (i = 0; i < N; i++) {
        যদি ((n = লিখ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  পূর্ণসংখ্যা xstatus;
  জন্য (পূর্ণসংখ্যা i = 1; i < argc; i++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(xstatus);
  }
  ফেরত 0;
}
