// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

#define N 1000

ಶೂನ್ಯ
print(ಧ್ರುವ ವರ್ಣ *s)
{
  ಲಿಖ(1, s, strlen(s));
}

ಶೂನ್ಯ
forktest(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ n, pid;

  print("fork test\n");

  ಪ್ರತಿ (n = 0; n < N; n++) {
    pid = fork();
    ಯದಿ (pid < 0)
      ಭಙ್ಗ;
    ಯದಿ (pid == 0)
      exit(0);
  }

  ಯದಿ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  ಪ್ರತಿ (; n > 0; n--) {
    ಯದಿ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ಯದಿ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
  forktest();
  exit(0);
}
