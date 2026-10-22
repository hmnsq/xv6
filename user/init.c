// init: The initial user-level program

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/spinlock.h"
#அந்தர்பாவ "kernel/sleeplock.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/file.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fcntl.h"

எழுத்து *argv[] = {"sh", 0};

முழு
முதன்மை(வெறுமை)
{
  முழு pid, wpid;

  எனில் (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  க்கு (;;) {
    printf("init: starting sh\n");
    pid = fork();
    எனில் (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    எனில் (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    க்கு (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((முழு *)0);
      எனில் (wpid == pid) {
        // the shell exited; restart it.
        நிறுத்து;
      } இல்லையெனில் எனில் (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } இல்லையெனில் {
        // it was a parentless process; do nothing.
      }
    }
  }
}
