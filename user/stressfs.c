// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/fcntl.h"

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક fd, i;
  અક્ષર path[] = "stressfs0";
  અક્ષર data[512];

  printf("stressfs starting\n");
  memset(data, 'a', કદ(data));

  માટે (i = 0; i < 4; i++)
    જો (fork() > 0)
      તોડો;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  માટે (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    લિખ(fd, data, કદ(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  માટે (i = 0; i < 20; i++)
    પઠ(fd, data, કદ(data));
  close(fd);

  wait(0);

  exit(0);
}
