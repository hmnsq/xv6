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
  цяло n, pid;

  print("fork test\n");

  за (n = 0; n < N; n++) {
    pid = fork();
    ако (pid < 0)
      break;
    ако (pid == 0)
      exit(0);
  }

  ако (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  за (; n > 0; n--) {
    ако (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ако (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

цяло
main(void)
{
  forktest();
  exit(0);
}
