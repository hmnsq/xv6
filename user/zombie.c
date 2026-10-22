// Create a zombie process that
// must be reparented at exit.

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

صحیح
مین(خالی)
{
  اگر (fork() > 0)
    pause(5); // Let child exit before parent.
  باہر_نکلو(0);
}
