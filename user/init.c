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

цяло
main(void)
{
  цяло pid, wpid;

  ако (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  за (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ако (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ако (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    за (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((цяло *)0);
      ако (wpid == pid) {
        // the shell exited; restart it.
        break;
      } иначе ако (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } иначе {
        // it was a parentless process; do nothing.
      }
    }
  }
}
