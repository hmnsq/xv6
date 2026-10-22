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

ကိန်းပြည့်
main(void)
{
  ကိန်းပြည့် pid, wpid;

  အကယ်၍ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  အတွက် (;;) {
    printf("init: starting sh\n");
    pid = fork();
    အကယ်၍ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    အကယ်၍ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    အတွက် (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ကိန်းပြည့် *)0);
      အကယ်၍ (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } မဟုတ်ပါက အကယ်၍ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } မဟုတ်ပါက {
        // it was a parentless process; do nothing.
      }
    }
  }
}
