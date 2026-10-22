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
  ціле n, pid;

  print("fork test\n");

  для (n = 0; n < N; n++) {
    pid = fork();
    якщо (pid < 0)
      break;
    якщо (pid == 0)
      exit(0);
  }

  якщо (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  для (; n > 0; n--) {
    якщо (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  якщо (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ціле
main(void)
{
  forktest();
  exit(0);
}
