// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ fd, i;
  ವರ್ಣ path[] = "stressfs0";
  ವರ್ಣ data[512];

  printf("stressfs starting\n");
  memset(data, 'a', ಪರಿಮಾಣ(data));

  ಪ್ರತಿ (i = 0; i < 4; i++)
    ಯದಿ (fork() > 0)
      ಭಙ್ಗ;

  printf("write %d\n", i);

  path[8] += i;
  fd = open(path, O_CREATE | O_RDWR);
  ಪ್ರತಿ (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ಲಿಖ(fd, data, ಪರಿಮಾಣ(data));
  }
  close(fd);

  printf("read\n");

  fd = open(path, O_RDONLY);
  ಪ್ರತಿ (i = 0; i < 20; i++)
    ಪಠ(fd, data, ಪರಿಮಾಣ(data));
  close(fd);

  wait(0);

  exit(0);
}
