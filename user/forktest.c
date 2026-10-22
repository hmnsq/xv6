// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

#define N 1000

শূন্য
print(ধ্রুবক অক্ষর *s)
{
  লিখ(1, s, strlen(s));
}

শূন্য
forktest(শূন্য)
{
  পূর্ণসংখ্যা n, pid;

  print("fork test\n");

  জন্য (n = 0; n < N; n++) {
    pid = fork();
    যদি (pid < 0)
      বিরতি;
    যদি (pid == 0)
      exit(0);
  }

  যদি (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  জন্য (; n > 0; n--) {
    যদি (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  যদি (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

পূর্ণসংখ্যা
প্রধান(শূন্য)
{
  forktest();
  exit(0);
}
