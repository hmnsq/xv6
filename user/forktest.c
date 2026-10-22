// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

#define N 1000

تهی
print(ثابت نویسه *s)
{
  اكتب(1, s, strlen(s));
}

تهی
forktest(تهی)
{
  صحیح n, pid;

  print("fork test\n");

  برای (n = 0; n < N; n++) {
    pid = fork();
    اگر (pid < 0)
      بشکن;
    اگر (pid == 0)
      exit(0);
  }

  اگر (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  برای (; n > 0; n--) {
    اگر (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  اگر (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

صحیح
اصلی(تهی)
{
  forktest();
  exit(0);
}
