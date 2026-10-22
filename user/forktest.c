// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 1000

void
इलिम्बाग(const char *s)
{
  write(1, s, strlen(s));
}

void
forktest(void)
{
  int n, pid;

  इलिम्बाग("fork test\n");

  for (n = 0; n < N; n++) {
    pid = fork();
    if (pid < 0)
      break;
    if (pid == 0)
      exit(0);
  }

  if (n == N) {
    इलिम्बाग("fork claimed to work N times!\n");
    exit(1);
  }

  for (; n > 0; n--) {
    if (wait(0) < 0) {
      इलिम्बाग("wait stopped early\n");
      exit(1);
    }
  }

  if (wait(0) != -1) {
    इलिम्बाग("wait got too many\n");
    exit(1);
  }

  इलिम्बाग("fork test OK\n");
}

int
main(void)
{
  forktest();
  exit(0);
}
