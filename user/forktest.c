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

  为 (n = 0; n < N; n++) {
    pid = fork();
    如果 (pid < 0)
      break;
    如果 (pid == 0)
      exit(0);
  }

  如果 (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  为 (; n > 0; n--) {
    如果 (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  如果 (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

整数
主要(void)
{
  forktest();
  exit(0);
}
