// init: The initial user-level program

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/spinlock.h"
#ضمن "kernel/sleeplock.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/file.h"
#ضمن "user/user.h"
#ضمن "kernel/fcntl.h"

حرف *argv[] = {"sh", 0};

صحيح
مکيه(فراغ)
{
  صحيح pid, wpid;

  إذا (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  لكل (;;) {
    لکو("init: starting sh\n");
    pid = fork();
    إذا (pid < 0) {
      لکو("init: fork failed\n");
      ٻاهر_نڪرو(1);
    }
    إذا (pid == 0) {
      exec("sh", argv);
      لکو("init: exec sh failed\n");
      ٻاهر_نڪرو(1);
    }

    لكل (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((صحيح *)0);
      إذا (wpid == pid) {
        // the shell exited; restart it.
        اكسر;
      } وإلا إذا (wpid < 0) {
        لکو("init: wait returned an error\n");
        ٻاهر_نڪرو(1);
      } وإلا {
        // it was a parentless process; do nothing.
      }
    }
  }
}
