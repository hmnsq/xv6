// Create a zombie process that
// must be reparented at exit.

#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

पूर्णांक
मुख्य(व्योम)
{
  अगर (विभाजन() > 0)
    छोड़ो(5); // Let child exit before parent.
  निकास(0);
}
