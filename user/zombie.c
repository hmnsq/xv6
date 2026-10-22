// Create a zombie process that
// must be reparented at exit.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
  ಆದರೆ (fork() > 0)
    pause(5); // Let child exit before parent.
  ನಿರ್ಗಮಿಸು(0);
}
