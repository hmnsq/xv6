// Create a zombie process that
// must be reparented at exit.

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

পূর্ণাঙ্ক
main(শূন্য)
{
  যদি (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
