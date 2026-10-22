#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/fcntl.h"
#समावेश "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#घोषणा BUFSZ 500

अक्षर buf[BUFSZ];

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर **argv)
{
  पूर्णांक fd, n;
  क्रमागत { N = 250, SZ = 2000 };

  क्रम (पूर्णांक i = 1; i < argc; i++) {
    पूर्णांक pid1 = विभाजन();
    अगर (pid1 < 0) {
      म_लिखो("%s: fork failed\n", argv[0]);
      निकास(1);
    }
    अगर (pid1 == 0) {
      fd = खोलो(argv[i], O_CREATE | O_RDWR);
      अगर (fd < 0) {
        म_लिखो("%s: create %s failed\n", argv[0], argv[i]);
        निकास(1);
      }
      स_रखो(buf, '0' + i, SZ);
      क्रम (i = 0; i < N; i++) {
        अगर ((n = ग_लिखो(fd, buf, SZ)) != SZ) {
          म_लिखो("write failed %d\n", n);
          निकास(1);
        }
      }
      निकास(0);
    }
  }
  पूर्णांक xstatus;
  क्रम (पूर्णांक i = 1; i < argc; i++) {
    रुको(&xstatus);
    अगर (xstatus != 0)
      निकास(xstatus);
  }
  वापस 0;
}
