// Create a zombie process that
// must be reparented at exit.

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"

പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
  എങ്കിൽ (fork() > 0)
    pause(5); // Let child exit before parent.
  exit(0);
}
