// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"

#define N 1000

ശൂന്യം
print(സ്ഥിരം അക്ഷരം *s)
{
  ലിഖ(1, s, strlen(s));
}

ശൂന്യം
forktest(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ n, pid;

  print("fork test\n");

  വേണ്ടി (n = 0; n < N; n++) {
    pid = fork();
    എങ്കിൽ (pid < 0)
      നിർത്തുക;
    എങ്കിൽ (pid == 0)
      exit(0);
  }

  എങ്കിൽ (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  വേണ്ടി (; n > 0; n--) {
    എങ്കിൽ (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  എങ്കിൽ (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
  forktest();
  exit(0);
}
