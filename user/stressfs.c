// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ fd, i;
  ਅੱਖਰ ਰਸਤਾ[] = "stressfs0";
  ਅੱਖਰ ਅੰਕੜੇ[512];

  ਲਿਖੋ("stressfs starting\n");
  memset(ਅੰਕੜੇ, 'a', ਆਕਾਰ(ਅੰਕੜੇ));

  ਲਈ (i = 0; i < 4; i++)
    ਜੇ (fork() > 0)
      ਰੋਕੋ;

  ਲਿਖੋ("write %d\n", i);

  ਰਸਤਾ[8] += i;
  fd = open(ਰਸਤਾ, O_CREATE | O_RDWR);
  ਲਈ (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    ਲਿਖ(fd, ਅੰਕੜੇ, ਆਕਾਰ(ਅੰਕੜੇ));
  }
  close(fd);

  ਲਿਖੋ("read\n");

  fd = open(ਰਸਤਾ, O_RDONLY);
  ਲਈ (i = 0; i < 20; i++)
    ਪਠ(fd, ਅੰਕੜੇ, ਆਕਾਰ(ਅੰਕੜੇ));
  close(fd);

  wait(0);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
