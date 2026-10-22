// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

var_na *argv[] = {"sh", 0};

පූර්ණ_සංඛ්‍යා
main(void)
{
  පූර්ණ_සංඛ්‍යා pid, wpid;

  නම් (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  සඳහා (;;) {
    printf("init: starting sh\n");
    pid = fork();
    නම් (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    නම් (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    සඳහා (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((පූර්ණ_සංඛ්‍යා *)0);
      නම් (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } නැතහොත් නම් (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } නැතහොත් {
        // it was a parentless process; do nothing.
      }
    }
  }
}
