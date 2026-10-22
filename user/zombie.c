// Create a zombie process that
// must be reparented at exit.

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

முழு
முதன்மை(வெறுமை)
{
  எனில் (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
