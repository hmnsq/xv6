// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

#تعریف_کرو N 1000

خالی
print(ثابت حرف *s)
{
  اكتب(1, s, لڑی_لمبائی(s));
}

خالی
forktest(خالی)
{
  صحیح n, pid;

  print("fork test\n");

  برائے (n = 0; n < N; n++) {
    pid = fork();
    اگر (pid < 0)
      توڑو;
    اگر (pid == 0)
      باہر_نکلو(0);
  }

  اگر (n == N) {
    print("fork claimed to work N times!\n");
    باہر_نکلو(1);
  }

  برائے (; n > 0; n--) {
    اگر (wait(0) < 0) {
      print("wait stopped early\n");
      باہر_نکلو(1);
    }
  }

  اگر (wait(0) != -1) {
    print("wait got too many\n");
    باہر_نکلو(1);
  }

  print("fork test OK\n");
}

صحیح
مین(خالی)
{
  forktest();
  باہر_نکلو(0);
}
