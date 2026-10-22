// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

#تعریف_کړه N 1000

فراغ
print(ثابت حرف *s)
{
  اكتب(1, s, مزي_اوږدوالی(s));
}

فراغ
forktest(فراغ)
{
  صحيح n, pid;

  print("fork test\n");

  لكل (n = 0; n < N; n++) {
    pid = fork();
    إذا (pid < 0)
      اكسر;
    إذا (pid == 0)
      وتل(0);
  }

  إذا (n == N) {
    print("fork claimed to work N times!\n");
    وتل(1);
  }

  لكل (; n > 0; n--) {
    إذا (wait(0) < 0) {
      print("wait stopped early\n");
      وتل(1);
    }
  }

  إذا (wait(0) != -1) {
    print("wait got too many\n");
    وتل(1);
  }

  print("fork test OK\n");
}

صحيح
اصلي(فراغ)
{
  forktest();
  وتل(0);
}
