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
main(صحيح argc, حرف *argv[])
{
  صحيح fd, i;
  حرف path[] = "stressfs0";
  حرف data[512];

  printf("stressfs starting\n");
  memset(data, 'a', حجم(data));

  لكل (i = 0; i < 4; i++)
    𐤀𐤌 (fork() > 0)
      اكسر;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  لكل (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, data, حجم(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  لكل (i = 0; i < 20; i++)
    اقرأ(fd, data, حجم(data));
  close(fd);

  wait(0);

  exit(0);
}
