// Create a zombie process that
// must be reparented at exit.

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
  જો (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
