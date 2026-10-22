// init: The initial user-level program

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/spinlock.h"
#अन्तर्भाव "kernel/sleeplock.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/file.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fcntl.h"

वर्ण *argv[] = {"sh", 0};

पूर्णाङ्क
मुख्यम्(शून्य)
{
  पूर्णाङ्क pid, wpid;

  यदि (उद्घाटय("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    उद्घाटय("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  प्रति (;;) {
    प्रारूप_लिख("init: starting sh\n");
    pid = fork();
    यदि (pid < 0) {
      प्रारूप_लिख("init: fork failed\n");
      निर्गच्छ(1);
    }
    यदि (pid == 0) {
      exec("sh", argv);
      प्रारूप_लिख("init: exec sh failed\n");
      निर्गच्छ(1);
    }

    प्रति (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = प्रतीक्षस्व((पूर्णाङ्क *)0);
      यदि (wpid == pid) {
        // the shell exited; restart it.
        भङ्ग;
      } अन्यथा यदि (wpid < 0) {
        प्रारूप_लिख("init: wait returned an error\n");
        निर्गच्छ(1);
      } अन्यथा {
        // it was a parentless process; do nothing.
      }
    }
  }
}
