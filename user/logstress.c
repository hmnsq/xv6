#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क fd, n;
  गणना { N = 250, SZ = 2000 };

  अत्थाय (पूर्णाङ्क i = 1; i < argc; i++) {
    पूर्णाङ्क pid1 = fork();
    सचे (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    सचे (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      सचे (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      अत्थाय (i = 0; i < N; i++) {
        सचे ((n = लिख(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  पूर्णाङ्क xstatus;
  अत्थाय (पूर्णाङ्क i = 1; i < argc; i++) {
    wait(&xstatus);
    सचे (xstatus != 0)
      exit(xstatus);
  }
  पटिनिवत्त 0;
}
