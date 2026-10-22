// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = {"sh", 0};

ціле
main(void)
{
  ціле pid, wpid;

  якщо (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  для (;;) {
    printf("init: starting sh\n");
    pid = fork();
    якщо (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    якщо (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    для (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ціле *)0);
      якщо (wpid == pid) {
        // the shell exited; restart it.
        break;
      } інакше якщо (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } інакше {
        // it was a parentless process; do nothing.
      }
    }
  }
}
