// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح fd, i;
  حرف راستہ[] = "stressfs0";
  حرف ڈیٹا[512];

  لکھو("stressfs starting\n");
  memset(ڈیٹا, 'a', حجم(ڈیٹا));

  برائے (i = 0; i < 4; i++)
    اگر (fork() > 0)
      توڑو;

  لکھو("write %d\n", i);

  راستہ[8] += i;
  fd = open(راستہ, O_CREATE | O_RDWR);
  برائے (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    اكتب(fd, ڈیٹا, حجم(ڈیٹا));
  }
  close(fd);

  لکھو("read\n");

  fd = open(راستہ, O_RDONLY);
  برائے (i = 0; i < 20; i++)
    اقرأ(fd, ڈیٹا, حجم(ڈیٹا));
  close(fd);

  wait(0);

  باہر_نکلو(0);
}
