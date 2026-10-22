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
  puur_naa_nka n, pid;

  print("fork test\n");

  for (n = 0; n < N; n++) {
    pid = fork();
    ᬬᬾᬦ᭄ (pid < 0)
      bha_nga;
    ᬬᬾᬦ᭄ (pid == 0)
      exit(0);
  }

  ᬬᬾᬦ᭄ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  for (; n > 0; n--) {
    ᬬᬾᬦ᭄ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ᬬᬾᬦ᭄ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

puur_naa_nka
main(void)
{
  forktest();
  exit(0);
}
