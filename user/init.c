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

จำนวนเต็ม
หลัก(void)
{
  จำนวนเต็ม pid, wpid;

  ถ้า (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  สำหรับ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ถ้า (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ถ้า (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    สำหรับ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((จำนวนเต็ม *)0);
      ถ้า (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } มิฉะนั้น ถ้า (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } มิฉะนั้น {
        // it was a parentless process; do nothing.
      }
    }
  }
}
