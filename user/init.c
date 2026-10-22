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
main(शून्य)
{
  पूर्णाङ्क pid, wpid;

  यदि (खोलो("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    खोलो("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  लागि (;;) {
    म_लिखो("init: starting sh\n");
    pid = विभाजन();
    यदि (pid < 0) {
      म_लिखो("init: fork failed\n");
      निकास(1);
    }
    यदि (pid == 0) {
      exec("sh", argv);
      म_लिखो("init: exec sh failed\n");
      निकास(1);
    }

    लागि (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = रुको((पूर्णाङ्क *)0);
      यदि (wpid == pid) {
        // the shell exited; restart it.
        रोक;
      } अन्यथा यदि (wpid < 0) {
        म_लिखो("init: wait returned an error\n");
        निकास(1);
      } अन्यथा {
        // it was a parentless process; do nothing.
      }
    }
  }
}
