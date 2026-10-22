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

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ fd, i;
  ಅಕ್ಷರ ಮಾರ್ಗ[] = "stressfs0";
  ಅಕ್ಷರ ದತ್ತಾಂಶ[512];

  ಬರೆ("stressfs starting\n");
  memset(ದತ್ತಾಂಶ, 'a', ಗಾತ್ರ(ದತ್ತಾಂಶ));

  ಗಾಗಿ (i = 0; i < 4; i++)
    ಆದರೆ (fork() > 0)
      ನಿಲ್ಲಿಸು;

  ಬರೆ("write %d\n", i);

  ಮಾರ್ಗ[8] += i;
  fd = open(ಮಾರ್ಗ, O_CREATE | O_RDWR);
  ಗಾಗಿ (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ಲಿಖ(fd, ದತ್ತಾಂಶ, ಗಾತ್ರ(ದತ್ತಾಂಶ));
  }
  close(fd);

  ಬರೆ("read\n");

  fd = open(ಮಾರ್ಗ, O_RDONLY);
  ಗಾಗಿ (i = 0; i < 20; i++)
    ಪಠ(fd, ದತ್ತಾಂಶ, ಗಾತ್ರ(ದತ್ತಾಂಶ));
  close(fd);

  wait(0);

  ನಿರ್ಗಮಿಸು(0);
}
