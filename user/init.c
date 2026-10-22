// init: The initial user-level program

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"
#ಅನ್ತರ್ಭಾವ "kernel/sleeplock.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/file.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

ವರ್ಣ *argv[] = {"sh", 0};

ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ pid, wpid;

  ಯದಿ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ಪ್ರತಿ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ಯದಿ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ಯದಿ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    ಪ್ರತಿ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ಪೂರ್ಣಾಙ್ಕ *)0);
      ಯದಿ (wpid == pid) {
        // the shell exited; restart it.
        ಭಙ್ಗ;
      } ಅನ್ಯಥಾ ಯದಿ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } ಅನ್ಯಥಾ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
