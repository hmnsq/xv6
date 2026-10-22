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
اصلی(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف مسیر[] = "stressfs0";
  حرف داده[512];

  چاپ_کن("stressfs starting\n");
  memset(داده, 'a', حجم(داده));

  لكل (i = 0; i < 4; i++)
    إذا (fork() > 0)
      اكسر;

  چاپ_کن("write %d\n", i);

  مسیر[8] += i;
  fd = open(مسیر, O_CREATE | O_RDWR);
  لكل (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, داده, حجم(داده));
  }
  close(fd);

  چاپ_کن("read\n");

  fd = open(مسیر, O_RDONLY);
  لكل (i = 0; i < 20; i++)
    اقرأ(fd, داده, حجم(داده));
  close(fd);

  wait(0);

  خروج(0);
}
