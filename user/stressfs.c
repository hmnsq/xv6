// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"
#समावेश "kernel/fs.h"
#समावेश "kernel/fcntl.h"

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;
  अक्षर path[] = "stressfs0";
  अक्षर तथ्य[512];

  म_लिखो("stressfs starting\n");
  स_रखो(तथ्य, 'a', माप(तथ्य));

  क्रम (i = 0; i < 4; i++)
    अगर (विभाजन() > 0)
      अवरोध;

  म_लिखो("write %d\n", i);

  path[8] += i;
  fd = खोलो(path, O_CREATE | O_RDWR);
  क्रम (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ग_लिखो(fd, तथ्य, माप(तथ्य));
  }
  बंद(fd);

  म_लिखो("read\n");

  fd = खोलो(path, O_RDONLY);
  क्रम (i = 0; i < 20; i++)
    पढ़ो(fd, तथ्य, माप(तथ्य));
  बंद(fd);

  रुको(0);

  निकास(0);
}
