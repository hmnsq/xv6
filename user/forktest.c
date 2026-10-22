// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

#define N 1000

શૂન્ય
print(અચલ અક્ષર *s)
{
  લિખ(1, s, strlen(s));
}

શૂન્ય
forktest(શૂન્ય)
{
  પૂર્ણાંક n, pid;

  print("fork test\n");

  માટે (n = 0; n < N; n++) {
    pid = fork();
    જો (pid < 0)
      તોડો;
    જો (pid == 0)
      exit(0);
  }

  જો (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  માટે (; n > 0; n--) {
    જો (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  જો (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
  forktest();
  exit(0);
}
