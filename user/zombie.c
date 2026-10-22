// Create a zombie process that
// must be reparented at exit.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
مکيه(فراغ)
{
  إذا (fork() > 0)
    pause(5); // Let child exit before parent.
  ٻاهر_نڪرو(0);
}
