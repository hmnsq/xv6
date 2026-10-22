// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

صحيح
اصلي(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف لار[] = "stressfs0";
  حرف معلومات[512];

  چاپ_کړه("stressfs starting\n");
  memset(معلومات, 'a', حجم(معلومات));

  لكل (i = 0; i < 4; i++)
    إذا (fork() > 0)
      اكسر;

  چاپ_کړه("write %d\n", i);

  لار[8] += i;
  fd = open(لار, O_CREATE | O_RDWR);
  لكل (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, معلومات, حجم(معلومات));
  }
  close(fd);

  چاپ_کړه("read\n");

  fd = open(لار, O_RDONLY);
  لكل (i = 0; i < 20; i++)
    اقرأ(fd, معلومات, حجم(معلومات));
  close(fd);

  wait(0);

  وتل(0);
}
