// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 1000

void
print(const char *s)
{
  write(1, s, strlen(s));
}

void
forktest(void)
{
  整数 n, pid;

  print("fork test\n");

  ために (n = 0; n < N; n++) {
    pid = fork();
    もし (pid < 0)
      break;
    もし (pid == 0)
      exit(0);
  }

  もし (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  ために (; n > 0; n--) {
    もし (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  もし (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

整数
メインページ(void)
{
  forktest();
  exit(0);
}
