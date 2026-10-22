// Create a zombie process that
// must be reparented at exit.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णाङ्क
मुख्यम्(शून्य)
{
  यदि (fork() > 0)
    pause(5); // Let child exit before parent.
  निर्गच्छ(0);
}
