#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/fcntl.h"
#అన్తర్భావ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#నిర్వచించు BUFSZ 500

అక్షరం buf[BUFSZ];

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
  పూర్ణాంకం fd, n;
  గణన { N = 250, SZ = 2000 };

  కొరకు (పూర్ణాంకం i = 1; i < argc; i++) {
    పూర్ణాంకం pid1 = fork();
    ఒకవేళ (pid1 < 0) {
      రాయి("%s: fork failed\n", argv[0]);
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      ఒకవేళ (fd < 0) {
        రాయి("%s: create %s failed\n", argv[0], argv[i]);
        నిష్క్రమించు(1);
      }
      memset(buf, '0' + i, SZ);
      కొరకు (i = 0; i < N; i++) {
        ఒకవేళ ((n = లిఖ(fd, buf, SZ)) != SZ) {
          రాయి("write failed %d\n", n);
          నిష్క్రమించు(1);
        }
      }
      నిష్క్రమించు(0);
    }
  }
  పూర్ణాంకం xstatus;
  కొరకు (పూర్ణాంకం i = 1; i < argc; i++) {
    wait(&xstatus);
    ఒకవేళ (xstatus != 0)
      నిష్క్రమించు(xstatus);
  }
  తిరిగి 0;
}
