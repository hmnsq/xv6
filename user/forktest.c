// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

#define N 1000

ଶୂନ୍ୟ
print(ଅଚଳ ଅକ୍ଷର *s)
{
  ଲିଖ(1, s, strlen(s));
}

ଶୂନ୍ୟ
forktest(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, pid;

  print("fork test\n");

  ପାଇଁ (n = 0; n < N; n++) {
    pid = fork();
    ଯଦି (pid < 0)
      ଭାଙ୍ଗ;
    ଯଦି (pid == 0)
      exit(0);
  }

  ଯଦି (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  ପାଇଁ (; n > 0; n--) {
    ଯଦି (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  ଯଦି (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
  forktest();
  exit(0);
}
