// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/fcntl.h"

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం fd, i;
  అక్షరం మార్గం[] = "stressfs0";
  అక్షరం దత్తాంశం[512];

  రాయి("stressfs starting\n");
  memset(దత్తాంశం, 'a', పరిమాణం(దత్తాంశం));

  కొరకు (i = 0; i < 4; i++)
    ఒకవేళ (fork() > 0)
      ఆపు;

  రాయి("write %d\n", i);

  మార్గం[8] += i;
  fd = open(మార్గం, O_CREATE | O_RDWR);
  కొరకు (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    లిఖ(fd, దత్తాంశం, పరిమాణం(దత్తాంశం));
  }
  close(fd);

  రాయి("read\n");

  fd = open(మార్గం, O_RDONLY);
  కొరకు (i = 0; i < 20; i++)
    పఠ(fd, దత్తాంశం, పరిమాణం(దత్తాంశం));
  close(fd);

  wait(0);

  నిష్క్రమించు(0);
}
