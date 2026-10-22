// init: The initial user-level program

#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/spinlock.h"
#समावेश "kernel/sleeplock.h"
#समावेश "kernel/fs.h"
#समावेश "kernel/file.h"
#समावेश "user/user.h"
#समावेश "kernel/fcntl.h"

अक्षर *argv[] = {"sh", 0};

पूर्णांक
मुख्य(व्योम)
{
  पूर्णांक pid, wpid;

  अगर (खोलो("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    खोलो("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  क्रम (;;) {
    म_लिखो("init: starting sh\n");
    pid = विभाजन();
    अगर (pid < 0) {
      म_लिखो("init: fork failed\n");
      निकास(1);
    }
    अगर (pid == 0) {
      exec("sh", argv);
      म_लिखो("init: exec sh failed\n");
      निकास(1);
    }

    क्रम (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = रुको((पूर्णांक *)0);
      अगर (wpid == pid) {
        // the shell exited; restart it.
        अवरोध;
      } अन्यथा अगर (wpid < 0) {
        म_लिखो("init: wait returned an error\n");
        निकास(1);
      } अन्यथा {
        // it was a parentless process; do nothing.
      }
    }
  }
}
