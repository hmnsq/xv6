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

  लागि (पूर्णाङ्क i = 1; i < argc; i++) {
    पूर्णाङ्क pid1 = विभाजन();
    यदि (pid1 < 0) {
      म_लिखो("%s: fork failed\n", argv[0]);
      निकास(1);
    }
    यदि (pid1 == 0) {
      fd = खोलो(argv[i], O_CREATE | O_RDWR);
      यदि (fd < 0) {
        म_लिखो("%s: create %s failed\n", argv[0], argv[i]);
        निकास(1);
      }
      स_रखो(buf, '0' + i, SZ);
      लागि (i = 0; i < N; i++) {
        यदि ((n = लिख(fd, buf, SZ)) != SZ) {
          म_लिखो("write failed %d\n", n);
          निकास(1);
        }
      }
      निकास(0);
    }
  }
  पूर्णाङ्क xstatus;
  लागि (पूर्णाङ्क i = 1; i < argc; i++) {
    रुको(&xstatus);
    यदि (xstatus != 0)
      निकास(xstatus);
  }
  फर्काऊ 0;
}
