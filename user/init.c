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

ཧྲིལ
main(void)
{
  ཧྲིལ pid, wpid;

  གལ་སྲིད (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  དོན་ལུ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    གལ་སྲིད (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    གལ་སྲིད (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    དོན་ལུ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ཧྲིལ *)0);
      གལ་སྲིད (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } དེ་མིན གལ་སྲིད (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } དེ་མིན {
        // it was a parentless process; do nothing.
      }
    }
  }
}
