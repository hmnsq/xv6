// init: The initial user-level program

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/spinlock.h"
#شامل "kernel/sleeplock.h"
#شامل "kernel/fs.h"
#شامل "kernel/file.h"
#شامل "user/user.h"
#شامل "kernel/fcntl.h"

حرف *argv[] = {"sh", 0};

صحیح
مین(خالی)
{
  صحیح pid, wpid;

  اگر (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  برائے (;;) {
    لکھو("init: starting sh\n");
    pid = fork();
    اگر (pid < 0) {
      لکھو("init: fork failed\n");
      باہر_نکلو(1);
    }
    اگر (pid == 0) {
      exec("sh", argv);
      لکھو("init: exec sh failed\n");
      باہر_نکلو(1);
    }

    برائے (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((صحیح *)0);
      اگر (wpid == pid) {
        // the shell exited; restart it.
        توڑو;
      } ورنہ اگر (wpid < 0) {
        لکھو("init: wait returned an error\n");
        باہر_نکلو(1);
      } ورنہ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
