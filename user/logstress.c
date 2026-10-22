#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

અક્ષર buf[BUFSZ];

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
  પૂર્ણાંક fd, n;
  ગણના { N = 250, SZ = 2000 };

  માટે (પૂર્ણાંક i = 1; i < argc; i++) {
    પૂર્ણાંક pid1 = fork();
    જો (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    જો (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      જો (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      માટે (i = 0; i < N; i++) {
        જો ((n = લિખ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  પૂર્ણાંક xstatus;
  માટે (પૂર્ણાંક i = 1; i < argc; i++) {
    wait(&xstatus);
    જો (xstatus != 0)
      exit(xstatus);
  }
  પાછા 0;
}
