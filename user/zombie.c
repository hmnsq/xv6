// Create a zombie process that
// must be reparented at exit.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
اصلي(فراغ)
{
  إذا (fork() > 0)
    pause(5); // Let child exit before parent.
  وتل(0);
}
