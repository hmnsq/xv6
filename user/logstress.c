#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

বর্ণ buf[BUFSZ];

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
  পূর্ণাঙ্ক fd, n;
  গণনা { N = 250, SZ = 2000 };

  প্রতি (পূর্ণাঙ্ক i = 1; i < argc; i++) {
    পূর্ণাঙ্ক pid1 = fork();
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
      প্রতি (i = 0; i < N; i++) {
        যদি ((n = লিখ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  পূর্ণাঙ্ক xstatus;
  প্রতি (পূর্ণাঙ্ক i = 1; i < argc; i++) {
    wait(&xstatus);
    যদি (xstatus != 0)
      exit(xstatus);
  }
  প্রত্যাগম 0;
}
