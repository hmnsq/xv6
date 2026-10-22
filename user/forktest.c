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
  정수 n, pid;

  print("fork test\n");

  위해 (n = 0; n < N; n++) {
    pid = fork();
    만약 (pid < 0)
      break;
    만약 (pid == 0)
      exit(0);
  }

  만약 (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  위해 (; n > 0; n--) {
    만약 (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  만약 (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

정수
main(void)
{
  forktest();
  exit(0);
}
