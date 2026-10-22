// init: The initial user-level program

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/spinlock.h"
#અન્તર્ભાવ "kernel/sleeplock.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/file.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fcntl.h"

અક્ષર *argv[] = {"sh", 0};

પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
  પૂર્ણાંક pid, wpid;

  જો (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  માટે (;;) {
    printf("init: starting sh\n");
    pid = fork();
    જો (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    જો (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    માટે (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((પૂર્ણાંક *)0);
      જો (wpid == pid) {
        // the shell exited; restart it.
        તોડો;
      } નહીંતર જો (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } નહીંતર {
        // it was a parentless process; do nothing.
      }
    }
  }
}
