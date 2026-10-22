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

ចំនួនគត់
main(void)
{
  ចំនួនគត់ pid, wpid;

  ប្រសិនបើ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  សម្រាប់ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ប្រសិនបើ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ប្រសិនបើ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    សម្រាប់ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ចំនួនគត់ *)0);
      ប្រសិនបើ (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } បើមិនដូច្នេះ ប្រសិនបើ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } បើមិនដូច្នេះ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
