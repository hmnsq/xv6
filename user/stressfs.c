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
مکھ(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف رستہ[] = "stressfs0";
  حرف انکڑے[512];

  لکھو("stressfs starting\n");
  memset(انکڑے, 'a', حجم(انکڑے));

  لكل (i = 0; i < 4; i++)
    إذا (fork() > 0)
      اكسر;

  لکھو("write %d\n", i);

  رستہ[8] += i;
  fd = open(رستہ, O_CREATE | O_RDWR);
  لكل (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, انکڑے, حجم(انکڑے));
  }
  close(fd);

  لکھو("read\n");

  fd = open(رستہ, O_RDONLY);
  لكل (i = 0; i < 20; i++)
    اقرأ(fd, انکڑے, حجم(انکڑے));
  close(fd);

  wait(0);

  باہر_نکلو(0);
}
