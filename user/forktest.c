// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

#define N 1000

فراغ
اطبع(ثابت حرف *s)
{
  اكتب(1, s, strlen(s));
}

فراغ
forktest(فراغ)
{
  صحيح n, pid;

  اطبع("fork test\n");

  لكل (n = 0; n < N; n++) {
    pid = fork();
    إذا (pid < 0)
      اكسر;
    إذا (pid == 0)
      exit(0);
  }

  إذا (n == N) {
    اطبع("fork claimed to work N times!\n");
    exit(1);
  }

  لكل (; n > 0; n--) {
    إذا (wait(0) < 0) {
      اطبع("wait stopped early\n");
      exit(1);
    }
  }

  إذا (wait(0) != -1) {
    اطبع("wait got too many\n");
    exit(1);
  }

  اطبع("fork test OK\n");
}

صحيح
main(فراغ)
{
  forktest();
  exit(0);
}
