// init: The initial user-level program

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/spinlock.h"
#অন্তর্ভাব "kernel/sleeplock.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/file.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fcntl.h"

অক্ষর *argv[] = {"sh", 0};

পূর্ণসংখ্যা
প্রধান(শূন্য)
{
  পূর্ণসংখ্যা pid, wpid;

  যদি (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  জন্য (;;) {
    printf("init: starting sh\n");
    pid = fork();
    যদি (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    যদি (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    জন্য (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((পূর্ণসংখ্যা *)0);
      যদি (wpid == pid) {
        // the shell exited; restart it.
        বিরতি;
      } অন্যথা যদি (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } অন্যথা {
        // it was a parentless process; do nothing.
      }
    }
  }
}
