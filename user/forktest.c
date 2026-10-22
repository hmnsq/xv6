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
  ចំនួនគត់ n, pid;

  print("fork test\n");

  សម្រាប់ (n = 0; n < N; n++) {
    pid = fork();
    ប្រសិនបើ (pid < 0)
      bha_nga;
    ប្រសិនបើ (pid == 0)
      exit(0);
  }

  ប្រសិនបើ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  សម្រាប់ (; n > 0; n--) {
    ប្រសិនបើ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ប្រសិនបើ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ចំនួនគត់
main(void)
{
  forktest();
  exit(0);
}
