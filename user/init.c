// init: The initial user-level program

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"

ਅੱਖਰ *argv[] = {"sh", 0};

ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
  ਪੂਰਨ pid, wpid;

  ਜੇ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ਲਈ (;;) {
    ਲਿਖੋ("init: starting sh\n");
    pid = fork();
    ਜੇ (pid < 0) {
      ਲਿਖੋ("init: fork failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਜੇ (pid == 0) {
      exec("sh", argv);
      ਲਿਖੋ("init: exec sh failed\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਲਈ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ਪੂਰਨ *)0);
      ਜੇ (wpid == pid) {
        // the shell exited; restart it.
        ਰੋਕੋ;
      } ਨਹੀਂਤਾਂ ਜੇ (wpid < 0) {
        ਲਿਖੋ("init: wait returned an error\n");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
      } ਨਹੀਂਤਾਂ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
