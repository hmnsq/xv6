// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/fcntl.h"

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு fd, i;
  எழுத்து path[] = "stressfs0";
  எழுத்து data[512];

  printf("stressfs starting\n");
  memset(data, 'a', அளவு(data));

  க்கு (i = 0; i < 4; i++)
    எனில் (fork() > 0)
      நிறுத்து;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  க்கு (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    லிக(fd, data, அளவு(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  க்கு (i = 0; i < 20; i++)
    பட(fd, data, அளவு(data));
  close(fd);

  wait(0);

  exit(0);
}
