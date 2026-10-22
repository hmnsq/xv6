// init: The initial user-level program

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/spinlock.h"
#അന്തര്ഭാവ "kernel/sleeplock.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/file.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fcntl.h"

അക്ഷരം *argv[] = {"sh", 0};

പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ pid, wpid;

  എങ്കിൽ (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  വേണ്ടി (;;) {
    printf("init: starting sh\n");
    pid = fork();
    എങ്കിൽ (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    എങ്കിൽ (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    വേണ്ടി (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((പൂർണ്ണസംഖ്യ *)0);
      എങ്കിൽ (wpid == pid) {
        // the shell exited; restart it.
        നിർത്തുക;
      } അല്ലെങ്കിൽ എങ്കിൽ (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } അല്ലെങ്കിൽ {
        // it was a parentless process; do nothing.
      }
    }
  }
}
