// init: The initial user-level program

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/spinlock.h"
#अन्तर्भाव "kernel/sleeplock.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/file.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fcntl.h"

अक्षर *argv[] = {"sh", 0};

पूर्णांक
main(शून्य)
{
  पूर्णांक pid, wpid;

  जर (खोलो("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    खोलो("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  साठी (;;) {
    म_लिखो("init: starting sh\n");
    pid = विभाजन();
    जर (pid < 0) {
      म_लिखो("init: fork failed\n");
      निकास(1);
    }
    जर (pid == 0) {
      exec("sh", argv);
      म_लिखो("init: exec sh failed\n");
      निकास(1);
    }

    साठी (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = रुको((पूर्णांक *)0);
      जर (wpid == pid) {
        // the shell exited; restart it.
        थांबा;
      } अन्यथा जर (wpid < 0) {
        म_लिखो("init: wait returned an error\n");
        निकास(1);
      } अन्यथा {
        // it was a parentless process; do nothing.
      }
    }
  }
}
