// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

#ವ್ಯಾಖ್ಯಾನಿಸು N 1000

ಶೂನ್ಯ
print(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
  ಲಿಖ(1, s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s));
}

ಶೂನ್ಯ
forktest(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ n, pid;

  print("fork test\n");

  ಗಾಗಿ (n = 0; n < N; n++) {
    pid = fork();
    ಆದರೆ (pid < 0)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ (pid == 0)
      ನಿರ್ಗಮಿಸು(0);
  }

  ಆದರೆ (n == N) {
    print("fork claimed to work N times!\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  ಗಾಗಿ (; n > 0; n--) {
    ಆದರೆ (wait(0) < 0) {
      print("wait stopped early\n");
      ನಿರ್ಗಮಿಸು(1);
    }
  }

  ಆದರೆ (wait(0) != -1) {
    print("wait got too many\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  print("fork test OK\n");
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
  forktest();
  ನಿರ್ಗಮಿಸು(0);
}
