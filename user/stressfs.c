// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/fcntl.h"

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা fd, i;
  অক্ষর path[] = "stressfs0";
  অক্ষর তথ্য[512];

  printf("stressfs starting\n");
  memset(তথ্য, 'a', আকার(তথ্য));

  জন্য (i = 0; i < 4; i++)
    যদি (fork() > 0)
      বিরতি;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  জন্য (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    লিখ(fd, তথ্য, আকার(তথ্য));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  জন্য (i = 0; i < 20; i++)
    পঠ(fd, তথ্য, আকার(তথ্য));
  close(fd);

  wait(0);

  exit(0);
}
