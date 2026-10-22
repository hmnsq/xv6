// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

จำนวนเต็ม
หลัก(void)
{
  ถ้า (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
