// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

#define N 1000

வெறுமை
print(மாறிலி எழுத்து *s)
{
  லிக(1, s, strlen(s));
}

வெறுமை
forktest(வெறுமை)
{
  முழு n, pid;

  print("fork test\n");

  க்கு (n = 0; n < N; n++) {
    pid = fork();
    எனில் (pid < 0)
      நிறுத்து;
    எனில் (pid == 0)
      exit(0);
  }

  எனில் (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  க்கு (; n > 0; n--) {
    எனில் (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  எனில் (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

முழு
முதன்மை(வெறுமை)
{
  forktest();
  exit(0);
}
