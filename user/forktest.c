// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ N 1000

ਖਾਲੀ
print(ਅਚਲ ਅੱਖਰ *s)
{
  ਲਿਖ(1, s, ਲੜੀ_ਲੰਬਾਈ(s));
}

ਖਾਲੀ
forktest(ਖਾਲੀ)
{
  ਪੂਰਨ n, pid;

  print("fork test\n");

  ਲਈ (n = 0; n < N; n++) {
    pid = fork();
    ਜੇ (pid < 0)
      ਰੋਕੋ;
    ਜੇ (pid == 0)
      ਬਾਹਰ_ਨਿਕਲੋ(0);
  }

  ਜੇ (n == N) {
    print("fork claimed to work N times!\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਲਈ (; n > 0; n--) {
    ਜੇ (wait(0) < 0) {
      print("wait stopped early\n");
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
  }

  ਜੇ (wait(0) != -1) {
    print("wait got too many\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  print("fork test OK\n");
}

ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
  forktest();
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
