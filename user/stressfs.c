// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, i;
  ଅକ୍ଷର path[] = "stressfs0";
  ଅକ୍ଷର data[512];

  printf("stressfs starting\n");
  memset(data, 'a', ଆକାର(data));

  ପାଇଁ (i = 0; i < 4; i++)
    ଯଦି (fork() > 0)
      ଭାଙ୍ଗ;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  ପାଇଁ (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ଲିଖ(fd, data, ଆକାର(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  ପାଇଁ (i = 0; i < 20; i++)
    ପଠ(fd, data, ଆକାର(data));
  close(fd);

  wait(0);

  exit(0);
}
