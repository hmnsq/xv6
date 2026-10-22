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

ຈຳນວນເຕັມ
main(void)
{
  ຈຳນວນເຕັມ pid, wpid;

  ຖ້າ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ສຳລັບ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ຖ້າ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ຖ້າ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    ສຳລັບ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ຈຳນວນເຕັມ *)0);
      ຖ້າ (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } ບໍ່ດັ່ງນັ້ນ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
