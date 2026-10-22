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
  ကိန်းပြည့် n, pid;

  print("fork test\n");

  အတွက် (n = 0; n < N; n++) {
    pid = fork();
    အကယ်၍ (pid < 0)
      bha_nga;
    အကယ်၍ (pid == 0)
      exit(0);
  }

  အကယ်၍ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  အတွက် (; n > 0; n--) {
    အကယ်၍ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  အကယ်၍ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ကိန်းပြည့်
main(void)
{
  forktest();
  exit(0);
}
