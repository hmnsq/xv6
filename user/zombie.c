// Create a zombie process that
// must be reparented at exit.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
  ಯದಿ (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
