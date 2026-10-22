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

  དོན་ལུ (n = 0; n < N; n++) {
    pid = fork();
    གལ་སྲིད (pid < 0)
      bha_nga;
    གལ་སྲིད (pid == 0)
      exit(0);
  }

  གལ་སྲིད (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  དོན་ལུ (; n > 0; n--) {
    གལ་སྲིད (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  གལ་སྲིད (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ཧྲིལ
main(void)
{
  forktest();
  exit(0);
}
