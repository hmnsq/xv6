// init: The initial user-level program

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/spinlock.h"
#ضمن "kernel/sleeplock.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/file.h"
#ضمن "user/user.h"
#ضمن "kernel/fcntl.h"

حرف *argv[] = {"sh", 0};

صحيح
main(فراغ)
{
  صحيح pid, wpid;

  𐤀𐤌 (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  لكل (;;) {
    printf("init: starting sh\n");
    pid = fork();
    𐤀𐤌 (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    𐤀𐤌 (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    لكل (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((صحيح *)0);
      𐤀𐤌 (wpid == pid) {
        // the shell exited; restart it.
        اكسر;
      } وإلا 𐤀𐤌 (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } وإلا {
        // it was a parentless process; do nothing.
      }
    }
  }
}
