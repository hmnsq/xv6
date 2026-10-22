// Create a zombie process that
// must be reparented at exit.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(فراغ)
{
  إذا (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
