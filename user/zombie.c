// Create a zombie process that
// must be reparented at exit.

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

పూర్ణాంకం
ప్రధాన(శూన్య)
{
  ఒకవేళ (fork() > 0)
    pause(5); // Let child exit before parent.
  నిష్క్రమించు(0);
}
