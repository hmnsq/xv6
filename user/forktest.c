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
  số_nguyên n, pid;

  print("fork test\n");

  cho (n = 0; n < N; n++) {
    pid = fork();
    nếu (pid < 0)
      break;
    nếu (pid == 0)
      exit(0);
  }

  nếu (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  cho (; n > 0; n--) {
    nếu (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  nếu (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

số_nguyên
main(void)
{
  forktest();
  exit(0);
}
