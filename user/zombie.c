// Create a zombie process that
// must be reparented at exit.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

पूर्णांक
main(शून्य)
{
  जर (विभाजन() > 0)
    छोड़ो(5); // Let child exit before parent.
  निकास(0);
}
