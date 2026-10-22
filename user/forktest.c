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
  ຈຳນວນເຕັມ n, pid;

  print("fork test\n");

  ສຳລັບ (n = 0; n < N; n++) {
    pid = fork();
    ຖ້າ (pid < 0)
      bha_nga;
    ຖ້າ (pid == 0)
      exit(0);
  }

  ຖ້າ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  ສຳລັບ (; n > 0; n--) {
    ຖ້າ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ຖ້າ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ຈຳນວນເຕັມ
main(void)
{
  forktest();
  exit(0);
}
