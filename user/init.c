// init: The initial user-level program

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/spinlock.h"
#شامل "kernel/sleeplock.h"
#شامل "kernel/fs.h"
#شامل "kernel/file.h"
#شامل "user/user.h"
#شامل "kernel/fcntl.h"

نویسه *argv[] = {"sh", 0};

صحیح
اصلی(تهی)
{
  صحیح pid, wpid;

  اگر (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  برای (;;) {
    printf("init: starting sh\n");
    pid = fork();
    اگر (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    اگر (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    برای (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((صحیح *)0);
      اگر (wpid == pid) {
        // the shell exited; restart it.
        بشکن;
      } وگرنه اگر (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } وگرنه {
        // it was a parentless process; do nothing.
      }
    }
  }
}
