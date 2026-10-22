// init: The initial user-level program

#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/spinlock.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/file.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"

ଅକ୍ଷର *argv[] = {"sh", 0};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid, wpid;

  ଯଦି (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  ପାଇଁ (;;) {
    printf("init: starting sh\n");
    pid = fork();
    ଯଦି (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    ଯଦି (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    ପାଇଁ (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)0);
      ଯଦି (wpid == pid) {
        // the shell exited; restart it.
        ଭାଙ୍ଗ;
      } ଅନ୍ୟଥା ଯଦି (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } ଅନ୍ୟଥା {
        // it was a parentless process; do nothing.
      }
    }
  }
}
