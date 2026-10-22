// Create a zombie process that
// must be reparented at exit.

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
  ਜੇ (fork() > 0)
    pause(5); // Let child exit before parent.
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
