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

정수
main(void)
{
  정수 pid, wpid;

  만약 (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  위해 (;;) {
    printf("init: starting sh\n");
    pid = fork();
    만약 (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    만약 (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    위해 (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((정수 *)0);
      만약 (wpid == pid) {
        // the shell exited; restart it.
        break;
      } 아니면 만약 (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } 아니면 {
        // it was a parentless process; do nothing.
      }
    }
  }
}
