// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक fd, i;
  अक्षर path[] = "stressfs0";
  अक्षर data[512];

  म_लिखो("stressfs starting\n");
  स_रखो(data, 'a', आकार(data));

  साठी (i = 0; i < 4; i++)
    जर (विभाजन() > 0)
      थांबा;

  म_लिखो("write %d\n", i);

  path[8] += i;
  fd = खोलो(path, O_CREATE | O_RDWR);
  साठी (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    लिख(fd, data, आकार(data));
  }
  बंद(fd);

  म_लिखो("read\n");

  fd = खोलो(path, O_RDONLY);
  साठी (i = 0; i < 20; i++)
    पठ(fd, data, आकार(data));
  बंद(fd);

  रुको(0);

  निकास(0);
}
