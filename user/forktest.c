// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

#define N 1000

فراغ
بغ(ثابت حرف *s)
{
  اكتب(1, s, strlen(s));
}

فراغ
forktest(فراغ)
{
  صحيح n, pid;

  بغ("fork test\n");

  لكل (n = 0; n < N; n++) {
    pid = fork();
    إذا (pid < 0)
      اكسر;
    إذا (pid == 0)
      exit(0);
  }

  إذا (n == N) {
    بغ("fork claimed to work N times!\n");
    exit(1);
  }

  لكل (; n > 0; n--) {
    إذا (wait(0) < 0) {
      بغ("wait stopped early\n");
      exit(1);
    }
  }

  إذا (wait(0) != -1) {
    بغ("wait got too many\n");
    exit(1);
  }

  بغ("fork test OK\n");
}

صحيح
main(فراغ)
{
  forktest();
  exit(0);
}
