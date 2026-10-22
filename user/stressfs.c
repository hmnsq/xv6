// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح fd, i;
  نویسه path[] = "stressfs0";
  نویسه data[512];

  printf("stressfs starting\n");
  memset(data, 'a', اندازه(data));

  برای (i = 0; i < 4; i++)
    اگر (fork() > 0)
      بشکن;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  برای (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, data, اندازه(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  برای (i = 0; i < 20; i++)
    اقرأ(fd, data, اندازه(data));
  close(fd);

  wait(0);

  exit(0);
}
