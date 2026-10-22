// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(void)
{
  만약 (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
