// init: The initial user-level program

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/spinlock.h"
#అన్తర్భావ "kernel/sleeplock.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/file.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fcntl.h"

అక్షరం *argv[] = {"sh", 0};

పూర్ణాంకం
ప్రధాన(శూన్య)
{
  పూర్ణాంకం pid, wpid;

  ఒకవేళ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  కొరకు (;;) {
    రాయి("init: starting sh\n");
    pid = fork();
    ఒకవేళ (pid < 0) {
      రాయి("init: fork failed\n");
      నిష్క్రమించు(1);
    }
    ఒకవేళ (pid == 0) {
      exec("sh", argv);
      రాయి("init: exec sh failed\n");
      నిష్క్రమించు(1);
    }

    కొరకు (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((పూర్ణాంకం *)0);
      ఒకవేళ (wpid == pid) {
        // the shell exited; restart it.
        ఆపు;
      } లేకపోతే ఒకవేళ (wpid < 0) {
        రాయి("init: wait returned an error\n");
        నిష్క్రమించు(1);
      } లేకపోతే {
        // it was a parentless process; do nothing.
      }
    }
  }
}
