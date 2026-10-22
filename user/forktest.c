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
  ཧྲིལ n, pid;

  print("fork test\n");

  ཆེད་དུ (n = 0; n < N; n++) {
    pid = fork();
    གལ་ཏེ (pid < 0)
      bha_nga;
    གལ་ཏེ (pid == 0)
      exit(0);
  }

  གལ་ཏེ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  ཆེད་དུ (; n > 0; n--) {
    གལ་ཏེ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  གལ་ཏེ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ཧྲིལ
གཙོ(void)
{
  forktest();
  exit(0);
}
