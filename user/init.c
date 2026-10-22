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
གཙོ(void)
{
  ཧྲིལ pid, wpid;

  གལ་ཏེ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ཆེད་དུ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    གལ་ཏེ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    གལ་ཏེ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    ཆེད་དུ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ཧྲིལ *)0);
      གལ་ཏེ (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } དེ་མིན གལ་ཏེ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } དེ་མིན {
        // it was a parentless process; do nothing.
      }
    }
  }
}
