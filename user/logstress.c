#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#परिभाषय BUFSZ 500

वर्ण buf[BUFSZ];

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क fd, n;
  गणना { N = 250, SZ = 2000 };

  प्रति (पूर्णाङ्क i = 1; i < argc; i++) {
    पूर्णाङ्क pid1 = fork();
    यदि (pid1 < 0) {
      प्रारूप_लिख("%s: fork failed\n", argv[0]);
      निर्गच्छ(1);
    }
    यदि (pid1 == 0) {
      fd = उद्घाटय(argv[i], O_CREATE | O_RDWR);
      यदि (fd < 0) {
        प्रारूप_लिख("%s: create %s failed\n", argv[0], argv[i]);
        निर्गच्छ(1);
      }
      स्मृतिं_पूरय(buf, '0' + i, SZ);
      प्रति (i = 0; i < N; i++) {
        यदि ((n = लिख(fd, buf, SZ)) != SZ) {
          प्रारूप_लिख("write failed %d\n", n);
          निर्गच्छ(1);
        }
      }
      निर्गच्छ(0);
    }
  }
  पूर्णाङ्क xstatus;
  प्रति (पूर्णाङ्क i = 1; i < argc; i++) {
    प्रतीक्षस्व(&xstatus);
    यदि (xstatus != 0)
      निर्गच्छ(xstatus);
  }
  प्रत्यागम 0;
}
