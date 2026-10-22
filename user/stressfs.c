// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"
#অন্তর্ভাৱ "kernel/fs.h"
#অন্তর্ভাৱ "kernel/fcntl.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক fd, i;
  ৱর্ণ path[] = "stressfs0";
  ৱর্ণ data[512];

  printf("stressfs starting\n");
  memset(data, 'a', পরিমাণ(data));

  বাবে (i = 0; i < 4; i++)
    যদি (fork() > 0)
      ভঙ্গ;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  বাবে (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    লিখ(fd, data, পরিমাণ(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  বাবে (i = 0; i < 20; i++)
    পঠ(fd, data, পরিমাণ(data));
  close(fd);

  wait(0);

  exit(0);
}
