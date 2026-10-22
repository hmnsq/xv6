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
  целое n, pid;

  print("fork test\n");

  для (n = 0; n < N; n++) {
    pid = fork();
    если (pid < 0)
      break;
    если (pid == 0)
      exit(0);
  }

  если (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  для (; n > 0; n--) {
    если (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  если (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

целое
основной(void)
{
  forktest();
  exit(0);
}
