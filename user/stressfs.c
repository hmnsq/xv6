// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ fd, i;
  var_na path[] = "stressfs0";
  var_na data[512];

  printf("stressfs starting\n");
  memset(data, 'a', parimaa_na(data));

  សម្រាប់ (i = 0; i < 4; i++)
    ប្រសិនបើ (fork() > 0)
      bha_nga;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  សម្រាប់ (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    write(fd, data, parimaa_na(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  សម្រាប់ (i = 0; i < 20; i++)
    read(fd, data, parimaa_na(data));
  close(fd);

  wait(0);

  exit(0);
}
