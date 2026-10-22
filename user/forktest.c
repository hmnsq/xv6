// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

#define N 1000

فراغ
print(ثابت حرف *s)
{
  اكتب(1, s, strlen(s));
}

فراغ
forktest(فراغ)
{
  صحيح n, pid;

  print("fork test\n");

  لكل (n = 0; n < N; n++) {
    pid = fork();
    𐤀𐤌 (pid < 0)
      اكسر;
    𐤀𐤌 (pid == 0)
      exit(0);
  }

  𐤀𐤌 (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  لكل (; n > 0; n--) {
    𐤀𐤌 (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  𐤀𐤌 (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

صحيح
main(فراغ)
{
  forktest();
  exit(0);
}
