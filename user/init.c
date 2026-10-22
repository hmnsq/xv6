// init: The initial user-level program

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/spinlock.h"
#अन्तर्भाव "kernel/sleeplock.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/file.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fcntl.h"

वर्ण *argv[] = {"sh", 0};

पूर्णाङ्क
main(शून्य)
{
  पूर्णाङ्क pid, wpid;

  जइ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  प्रति (;;) {
    printf("init: starting sh\n");
    pid = fork();
    जइ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    जइ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    प्रति (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((पूर्णाङ्क *)0);
      जइ (wpid == pid) {
        // the shell exited; restart it.
        भङ्ग;
      } अण्णहा जइ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } अण्णहा {
        // it was a parentless process; do nothing.
      }
    }
  }
}
