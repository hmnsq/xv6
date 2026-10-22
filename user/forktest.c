// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 1000

void
print(const var_na *s)
{
  write(1, s, strlen(s));
}

void
forktest(void)
{
  පූර්ණ_සංඛ්‍යා n, pid;

  print("fork test\n");

  සඳහා (n = 0; n < N; n++) {
    pid = fork();
    නම් (pid < 0)
      bha_nga;
    නම් (pid == 0)
      exit(0);
  }

  නම් (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  සඳහා (; n > 0; n--) {
    නම් (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  නම් (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

පූර්ණ_සංඛ්‍යා
main(void)
{
  forktest();
  exit(0);
}
