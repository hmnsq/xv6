// init: The initial user-level program

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"
#ಅನ್ತರ್ಭಾವ "kernel/sleeplock.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/file.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

ಅಕ್ಷರ *argv[] = {"sh", 0};

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ pid, wpid;

  ಆದರೆ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ಗಾಗಿ (;;) {
    ಬರೆ("init: starting sh\n");
    pid = fork();
    ಆದರೆ (pid < 0) {
      ಬರೆ("init: fork failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }
    ಆದರೆ (pid == 0) {
      exec("sh", argv);
      ಬರೆ("init: exec sh failed\n");
      ನಿರ್ಗಮಿಸು(1);
    }

    ಗಾಗಿ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ಪೂರ್ಣಾಂಕ *)0);
      ಆದರೆ (wpid == pid) {
        // the shell exited; restart it.
        ನಿಲ್ಲಿಸು;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wpid < 0) {
        ಬರೆ("init: wait returned an error\n");
        ನಿರ್ಗಮಿಸು(1);
      } ಇಲ್ಲದಿದ್ದರೆ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
