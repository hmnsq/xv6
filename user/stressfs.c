// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/fcntl.h"

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ fd, i;
  അക്ഷരം path[] = "stressfs0";
  അക്ഷരം data[512];

  printf("stressfs starting\n");
  memset(data, 'a', വലുപ്പം(data));

  വേണ്ടി (i = 0; i < 4; i++)
    എങ്കിൽ (fork() > 0)
      നിർത്തുക;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  വേണ്ടി (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ലിഖ(fd, data, വലുപ്പം(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  വേണ്ടി (i = 0; i < 20; i++)
    പഠ(fd, data, വലുപ്പം(data));
  close(fd);

  wait(0);

  exit(0);
}
