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

puur_naa_nka
main(void)
{
  puur_naa_nka pid, wpid;

  ᮚᮦᮔ᮪ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  for (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ᮚᮦᮔ᮪ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ᮚᮦᮔ᮪ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((puur_naa_nka *)0);
      ᮚᮦᮔ᮪ (wpid == pid) {
        // the shell exited; restart it.
        bha_nga;
      } ᮜᮤᮚ ᮚᮦᮔ᮪ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } ᮜᮤᮚ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
